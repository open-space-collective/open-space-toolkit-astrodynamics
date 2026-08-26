/// Apache License 2.0

#include <algorithm>
#include <cmath>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter/MeanElementConsistency.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
{
namespace observationfilter
{

using ostk::core::type::String;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

namespace
{

// Internal constants of the robust trend fits (not tuning parameters): number of least-squares
// refit rounds, the gate multiple beyond which a point is excluded from a refit, and the cap on
// the number of Theil-Sen slope pairs.
constexpr Size REFIT_ROUND_COUNT = 2;
constexpr double REFIT_EXCLUSION_MULTIPLE = 3.0;
constexpr Size MAX_THEIL_SEN_PAIR_COUNT = 250000;

const std::array<const char*, MeanElementConsistency::ChannelCount> CHANNEL_NAMES = {
    "Semi-major axis [m]", "ex [-]", "ey [-]", "Inclination [rad]", "Node [rad]", "Phase [rad]"
};

double computeMedian(VectorXd aVector)
{
    const Eigen::Index count = aVector.size();

    std::nth_element(aVector.data(), aVector.data() + count / 2, aVector.data() + count);
    const double upper = aVector(count / 2);

    if (count % 2 == 1)
    {
        return upper;
    }

    std::nth_element(aVector.data(), aVector.data() + count / 2 - 1, aVector.data() + count);
    return 0.5 * (aVector(count / 2 - 1) + upper);
}

double wrapAngle(const double anAngle)
{
    double wrapped = std::fmod(anAngle + M_PI, 2.0 * M_PI);
    if (wrapped < 0.0)
    {
        wrapped += 2.0 * M_PI;
    }
    return wrapped - M_PI;
}

// Theil-Sen line: slope = median of pairwise slopes (deterministically subsampled beyond the pair
// cap), intercept = median of (y - slope * t).
void computeTheilSenLine(const VectorXd& aTimeArray, const VectorXd& aValueArray, double& aSlope, double& anIntercept)
{
    const Size count = static_cast<Size>(aTimeArray.size());

    const Size pairCount = count * (count - 1) / 2;
    const Size stride = std::max(Size(1), (pairCount + MAX_THEIL_SEN_PAIR_COUNT - 1) / MAX_THEIL_SEN_PAIR_COUNT);

    std::vector<double> slopes;
    slopes.reserve(std::min(pairCount, MAX_THEIL_SEN_PAIR_COUNT) + 1);

    Size pairIndex = 0;
    for (Size i = 0; i < count; ++i)
    {
        for (Size j = i + 1; j < count; ++j, ++pairIndex)
        {
            if (pairIndex % stride != 0)
            {
                continue;
            }

            const double dt = aTimeArray(j) - aTimeArray(i);
            if (dt != 0.0)
            {
                slopes.push_back((aValueArray(j) - aValueArray(i)) / dt);
            }
        }
    }

    if (slopes.empty())
    {
        aSlope = 0.0;
    }
    else
    {
        aSlope = computeMedian(Eigen::Map<VectorXd>(slopes.data(), static_cast<Eigen::Index>(slopes.size())));
    }

    anIntercept = computeMedian(aValueArray - aSlope * aTimeArray);
}

// Least-squares polynomial fit (on the kept points only), evaluated as residuals for all points.
VectorXd computePolynomialFitResiduals(
    const VectorXd& aNormalizedTimeArray,
    const VectorXd& aValueArray,
    const std::vector<bool>& aKeepArray,
    const Size anOrder
)
{
    const Eigen::Index count = aValueArray.size();

    Eigen::Index keepCount = 0;
    for (Eigen::Index i = 0; i < count; ++i)
    {
        keepCount += aKeepArray[static_cast<Size>(i)] ? 1 : 0;
    }

    MatrixXd vandermonde(keepCount, static_cast<Eigen::Index>(anOrder) + 1);
    VectorXd values(keepCount);

    Eigen::Index row = 0;
    for (Eigen::Index i = 0; i < count; ++i)
    {
        if (!aKeepArray[static_cast<Size>(i)])
        {
            continue;
        }

        double power = 1.0;
        for (Size order = 0; order <= anOrder; ++order)
        {
            vandermonde(row, static_cast<Eigen::Index>(order)) = power;
            power *= aNormalizedTimeArray(i);
        }
        values(row) = aValueArray(i);
        ++row;
    }

    const VectorXd coefficients = vandermonde.colPivHouseholderQr().solve(values);

    VectorXd residuals(count);
    for (Eigen::Index i = 0; i < count; ++i)
    {
        double fitted = 0.0;
        double power = 1.0;
        for (Size order = 0; order <= anOrder; ++order)
        {
            fitted += coefficients(static_cast<Eigen::Index>(order)) * power;
            power *= aNormalizedTimeArray(i);
        }
        residuals(i) = aValueArray(i) - fitted;
    }

    return residuals;
}

// Robust channel trend removal and gating:
// Theil-Sen seed, REFIT_ROUND_COUNT least-squares refits excluding only points beyond
// REFIT_EXCLUSION_MULTIPLE × the gate, then final gate = gateFactor × max(1.4826 × MAD, floor).
void classifyChannel(
    const VectorXd& aTimeArray,
    const VectorXd& aValueArray,
    const Size anOrder,
    const double aGateFactor,
    const double aFloor,
    VectorXd& aResidualArray,
    double& anAppliedGate,
    VectorXi& anInlierMask
)
{
    const Eigen::Index count = aValueArray.size();

    const Size order = std::min(anOrder, static_cast<Size>(std::max(Eigen::Index(0), count - 2)));

    if (order == 0)
    {
        aResidualArray = aValueArray.array() - computeMedian(aValueArray);
    }
    else
    {
        double slope = 0.0;
        double intercept = 0.0;
        computeTheilSenLine(aTimeArray, aValueArray, slope, intercept);
        aResidualArray = aValueArray - (slope * aTimeArray).matrix() - intercept * VectorXd::Ones(count);

        const double timeSpan = std::max(aTimeArray.maxCoeff() - aTimeArray.minCoeff(), 1.0);
        const VectorXd normalizedTimes = (aTimeArray.array() - aTimeArray.mean()) / timeSpan;

        for (Size round = 0; round < REFIT_ROUND_COUNT; ++round)
        {
            const double median = computeMedian(aResidualArray);
            const double mad = computeMedian((aResidualArray.array() - median).abs().matrix());
            const double scale = std::max(1.4826 * mad, aFloor);
            const double exclusionGate = REFIT_EXCLUSION_MULTIPLE * aGateFactor * scale;

            std::vector<bool> keep(static_cast<Size>(count), false);
            Size keepCount = 0;
            for (Eigen::Index i = 0; i < count; ++i)
            {
                keep[static_cast<Size>(i)] = std::abs(aResidualArray(i) - median) <= exclusionGate;
                keepCount += keep[static_cast<Size>(i)] ? 1 : 0;
            }

            if (keepCount < order + 2)
            {
                break;
            }

            aResidualArray = computePolynomialFitResiduals(normalizedTimes, aValueArray, keep, order);
        }
    }

    const double median = computeMedian(aResidualArray);
    const double mad = computeMedian((aResidualArray.array() - median).abs().matrix());
    anAppliedGate = aGateFactor * std::max(1.4826 * mad, aFloor);

    for (Eigen::Index i = 0; i < count; ++i)
    {
        if (std::abs(aResidualArray(i) - median) > anAppliedGate)
        {
            anInlierMask(i) = 0;
        }
    }
}

}  // namespace

MeanElementConsistency::Analysis::Analysis(
    const VectorXi& anInlierMask, const MatrixXd& aChannelResidualMatrix, const VectorXd& anAppliedChannelGateArray
)
    : ObservationFilter::Analysis(anInlierMask),
      channelResiduals(aChannelResidualMatrix),
      appliedChannelGates(anAppliedChannelGateArray)
{
}

void MeanElementConsistency::Analysis::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Mean Element Consistency Observation Filter Analysis");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Inlier count: " << getInlierCount();
    ostk::core::utils::Print::Line(anOutputStream) << "Outlier count: " << getOutlierCount();

    for (Size channel = 0; channel < ChannelCount; ++channel)
    {
        if (appliedChannelGates.size() == static_cast<Eigen::Index>(ChannelCount))
        {
            ostk::core::utils::Print::Line(anOutputStream) << (String("Gate — ") + CHANNEL_NAMES[channel] + ":")
                                                           << appliedChannelGates(static_cast<Eigen::Index>(channel));
        }
    }

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

MeanElementConsistency::MeanElementConsistency(
    const Real& aGateFactor, const Real& aNoiseFloor, const Derived& aGravitationalParameter
)
    : gateFactor_(aGateFactor),
      noiseFloor_(aNoiseFloor),
      gravitationalParameter_(aGravitationalParameter)
{
    if (!gateFactor_.isDefined() || gateFactor_ <= 0.0)
    {
        throw ostk::core::error::RuntimeError("Gate factor must be strictly positive.");
    }

    if (!noiseFloor_.isDefined() || noiseFloor_ < 0.0)
    {
        throw ostk::core::error::RuntimeError("Noise floor must be non-negative.");
    }

    if (!gravitationalParameter_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational parameter");
    }
}

Real MeanElementConsistency::getGateFactor() const
{
    return gateFactor_;
}

Real MeanElementConsistency::getNoiseFloor() const
{
    return noiseFloor_;
}

Derived MeanElementConsistency::getGravitationalParameter() const
{
    return gravitationalParameter_;
}

Shared<const ObservationFilter::Analysis> MeanElementConsistency::filter(const Array<State>& anObservationStateArray
) const
{
    const Size observationCount = anObservationStateArray.getSize();

    if (observationCount == 0)
    {
        return std::make_shared<const MeanElementConsistency::Analysis>(
            VectorXi(), MatrixXd(0, ChannelCount), VectorXd()
        );
    }

    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const auto positionSubsetSPtr = CartesianPosition::Default();
    const auto velocitySubsetSPtr = CartesianVelocity::Default();

    // Convert each observation to Brouwer-Lyddane mean elements (closed form, in GCRF)

    const Instant referenceInstant = anObservationStateArray.accessFirst().accessInstant();

    VectorXd times(observationCount);
    VectorXd smaArray(observationCount);
    VectorXd eccentricityArray(observationCount);
    VectorXd inclinationArray(observationCount);
    VectorXd raanArray(observationCount);
    VectorXd aopArray(observationCount);
    VectorXd meanAnomalyArray(observationCount);

    for (Size i = 0; i < observationCount; ++i)
    {
        const State& observation = anObservationStateArray[i];

        if (!observation.hasSubset(positionSubsetSPtr) || !observation.hasSubset(velocitySubsetSPtr))
        {
            throw ostk::core::error::RuntimeError(
                "Observations passed to MeanElementConsistency must contain Cartesian position and velocity subsets."
            );
        }

        const State observationInGcrf = observation.inFrame(gcrfSPtr);

        const BrouwerLyddaneMeanLong meanElements = BrouwerLyddaneMeanLong::Cartesian(
            {observationInGcrf.getPosition(), observationInGcrf.getVelocity()}, gravitationalParameter_
        );

        const Eigen::Index index = static_cast<Eigen::Index>(i);
        times(index) = Duration::Between(referenceInstant, observation.accessInstant()).inSeconds();
        smaArray(index) = meanElements.getSemiMajorAxis().inMeters();
        eccentricityArray(index) = meanElements.getEccentricity();
        inclinationArray(index) = meanElements.getInclination().inRadians();
        raanArray(index) = meanElements.getRaan().inRadians();
        aopArray(index) = meanElements.getAop().inRadians();
        meanAnomalyArray(index) = meanElements.getMeanAnomaly().inRadians();
    }

    // Secular rates from the median mean elements (closed form)

    const double medianSma = computeMedian(smaArray);
    const double medianEccentricity = computeMedian(eccentricityArray);
    const double medianInclination = computeMedian(inclinationArray);

    const double mu = gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared());
    const double j2 = EarthGravitationalModel::EGM2008.J2_;
    const double equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_.inMeters();

    const double meanMotion = std::sqrt(mu / (medianSma * medianSma * medianSma));
    const double semiLatusRectum = medianSma * (1.0 - medianEccentricity * medianEccentricity);
    const double j2Factor =
        1.5 * j2 * (equatorialRadius / semiLatusRectum) * (equatorialRadius / semiLatusRectum) * meanMotion;

    const double sinInclination = std::sin(medianInclination);
    const double nodeRate = -j2Factor * std::cos(medianInclination);
    const double phaseRate = nodeRate + j2Factor * (2.0 - 2.5 * sinInclination * sinInclination) + meanMotion +
                             j2Factor * std::sqrt(1.0 - medianEccentricity * medianEccentricity) *
                                 (1.0 - 1.5 * sinInclination * sinInclination);

    // Channels

    VectorXd exArray(observationCount);
    VectorXd eyArray(observationCount);
    VectorXd nodeArray(observationCount);
    VectorXd phaseArray(observationCount);

    const double referencePhase = raanArray(0) + aopArray(0) + meanAnomalyArray(0);
    const double referenceNode = raanArray(0);

    for (Eigen::Index i = 0; i < static_cast<Eigen::Index>(observationCount); ++i)
    {
        const double eccentricityAngle = aopArray(i) + raanArray(i);
        exArray(i) = eccentricityArray(i) * std::cos(eccentricityAngle);
        eyArray(i) = eccentricityArray(i) * std::sin(eccentricityAngle);
        nodeArray(i) = wrapAngle(raanArray(i) - nodeRate * times(i) - referenceNode);
        phaseArray(i) =
            wrapAngle(raanArray(i) + aopArray(i) + meanAnomalyArray(i) - phaseRate * times(i) - referencePhase);
    }

    // Re-center the wrapped channels on their medians so that residual trends sit away from the ±π seam

    const double nodeMedian = computeMedian(nodeArray);
    const double phaseMedian = computeMedian(phaseArray);
    for (Eigen::Index i = 0; i < static_cast<Eigen::Index>(observationCount); ++i)
    {
        nodeArray(i) = wrapAngle(nodeArray(i) - nodeMedian);
        phaseArray(i) = wrapAngle(phaseArray(i) - phaseMedian);
    }

    // Robust per-channel trend fits and gating

    const double noiseFloor = static_cast<double>(noiseFloor_);
    const double angularFloor = noiseFloor / medianSma;

    const std::array<std::tuple<const VectorXd*, Size, double>, ChannelCount> channels = {
        std::make_tuple(&smaArray, Size(1), 2.0 * noiseFloor),
        std::make_tuple(&exArray, Size(1), angularFloor),
        std::make_tuple(&eyArray, Size(1), angularFloor),
        std::make_tuple(&inclinationArray, Size(1), angularFloor),
        std::make_tuple(&nodeArray, Size(1), noiseFloor / (medianSma * std::max(sinInclination, 0.1))),
        std::make_tuple(&phaseArray, Size(2), angularFloor),
    };

    VectorXi inlierMask = VectorXi::Ones(observationCount);
    MatrixXd channelResiduals(observationCount, ChannelCount);
    VectorXd appliedChannelGates(ChannelCount);

    for (Size channel = 0; channel < ChannelCount; ++channel)
    {
        const auto& [valueArrayPtr, order, floor] = channels[channel];

        VectorXd residuals;
        double gate = 0.0;
        classifyChannel(
            times, *valueArrayPtr, order, static_cast<double>(gateFactor_), floor, residuals, gate, inlierMask
        );

        channelResiduals.col(static_cast<Eigen::Index>(channel)) = residuals;
        appliedChannelGates(static_cast<Eigen::Index>(channel)) = gate;
    }

    return std::make_shared<const MeanElementConsistency::Analysis>(inlierMask, channelResiduals, appliedChannelGates);
}

}  // namespace observationfilter
}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk
