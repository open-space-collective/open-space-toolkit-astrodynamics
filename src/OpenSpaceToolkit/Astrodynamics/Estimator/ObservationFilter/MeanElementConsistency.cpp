/// Apache License 2.0

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter/MeanElementConsistency.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
{
namespace observationfilter
{

using ostk::core::type::Size;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::unit::Derived;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

namespace
{

constexpr Size ChannelCount = 6;
constexpr double MinimumPairSeparationFraction = 0.25;  // Pairs closer than this fraction of the arc span are skipped
constexpr Size MaximumPairCount = 250000;               // Pairs are strided beyond this count
constexpr double MadToSigma = 1.4826;                   // MAD to standard deviation, normal distribution

double Median(std::vector<double> aValueArray)
{
    if (aValueArray.empty())
    {
        return 0.0;
    }

    const Size midIndex = aValueArray.size() / 2;

    std::nth_element(aValueArray.begin(), aValueArray.begin() + midIndex, aValueArray.end());

    const double upper = aValueArray[midIndex];

    if (aValueArray.size() % 2 == 1)
    {
        return upper;
    }

    const double lower = *std::max_element(aValueArray.begin(), aValueArray.begin() + midIndex);

    return 0.5 * (lower + upper);
}

std::vector<double> Select(const VectorXd& aVector, const std::vector<bool>& aValidMask)
{
    std::vector<double> values;
    values.reserve(aVector.size());

    for (Eigen::Index i = 0; i < aVector.size(); ++i)
    {
        if (aValidMask[i])
        {
            values.push_back(aVector(i));
        }
    }

    return values;
}

double Wrap(const double& anAngle)
{
    return std::remainder(anAngle, 2.0 * static_cast<double>(Real::Pi()));  // (-pi, pi]
}

/// Theil-Sen median slope restricted to observation pairs separated by at least a fraction of the arc span, so that
/// the many short-separation pairs of a dense burst cannot dominate the estimate.
double RobustSlope(const VectorXd& aTimeVector, const VectorXd& aValueVector, const std::vector<bool>& aValidMask)
{
    const Size count = static_cast<Size>(aTimeVector.size());

    double minimumTime = std::numeric_limits<double>::infinity();
    double maximumTime = -std::numeric_limits<double>::infinity();

    for (Size i = 0; i < count; ++i)
    {
        if (aValidMask[i])
        {
            minimumTime = std::min(minimumTime, aTimeVector(i));
            maximumTime = std::max(maximumTime, aTimeVector(i));
        }
    }

    const double span = maximumTime - minimumTime;

    if (!(span > 0.0))
    {
        return 0.0;
    }

    const double minimumSeparation = MinimumPairSeparationFraction * span;
    const double pairCount = 0.5 * static_cast<double>(count) * static_cast<double>(count - 1);
    const Size stride = std::max<Size>(1, static_cast<Size>(std::ceil(std::sqrt(pairCount / MaximumPairCount))));

    std::vector<double> slopes;

    for (Size i = 0; i < count; i += stride)
    {
        if (!aValidMask[i])
        {
            continue;
        }

        for (Size j = i + 1; j < count; j += stride)
        {
            if (!aValidMask[j])
            {
                continue;
            }

            const double timeDifference = aTimeVector(j) - aTimeVector(i);

            if (std::abs(timeDifference) < minimumSeparation)
            {
                continue;
            }

            slopes.push_back((aValueVector(j) - aValueVector(i)) / timeDifference);
        }
    }

    return Median(slopes);
}

}  // namespace

MeanElementConsistency::Analysis::Analysis(
    const VectorXi& anInlierMask, const MatrixXd& aDeviationMatrix, const VectorXd& aGateVector
)
    : ObservationFilter::Analysis(anInlierMask),
      deviations(aDeviationMatrix),
      gates(aGateVector)
{
}

void MeanElementConsistency::Analysis::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Mean Element Consistency Analysis");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Inlier count:" << getInlierCount();
    ostk::core::utils::Print::Line(anOutputStream) << "Outlier count:" << getOutlierCount();

    ostk::core::utils::Print::Separator(anOutputStream, "Gates [m]");

    const Array<String> channelNames = MeanElementConsistency::ChannelNames();

    for (Eigen::Index i = 0; i < gates.size(); ++i)
    {
        ostk::core::utils::Print::Line(anOutputStream) << (channelNames[i] + ":") << gates(i);
    }

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

MeanElementConsistency::MeanElementConsistency(const Real& aGateFactor, const Length& aMinimumDeviation)
    : gateFactor_(aGateFactor),
      minimumDeviation_(aMinimumDeviation)
{
    if (!gateFactor_.isDefined() || (gateFactor_ <= 0.0))
    {
        throw ostk::core::error::RuntimeError("Gate factor must be strictly positive.");
    }

    if (!minimumDeviation_.isDefined() || (minimumDeviation_.inMeters() <= 0.0))
    {
        throw ostk::core::error::RuntimeError("Minimum deviation must be strictly positive.");
    }
}

Real MeanElementConsistency::getGateFactor() const
{
    return gateFactor_;
}

Length MeanElementConsistency::getMinimumDeviation() const
{
    return minimumDeviation_;
}

Shared<const ObservationFilter::Analysis> MeanElementConsistency::filter(const Array<State>& anObservationStateArray
) const
{
    const Size count = anObservationStateArray.getSize();
    const double minimumDeviation = minimumDeviation_.inMeters();
    const double nan = std::numeric_limits<double>::quiet_NaN();

    if (count == 0)
    {
        return std::make_shared<Analysis>(
            VectorXi(0), MatrixXd(0, ChannelCount), VectorXd::Constant(ChannelCount, minimumDeviation)
        );
    }

    const EarthGravitationalModel::Parameters& earthParameters = EarthGravitationalModel::EGM2008;
    const Derived& gravitationalParameter = earthParameters.gravitationalParameter_;
    const double equatorialRadius = earthParameters.equatorialRadius_.inMeters();
    const double j2 = earthParameters.J2_;

    // Brouwer-Lyddane mean elements [a, e, i, raan, aop, M] and elapsed time per observation

    VectorXd time(count);
    MatrixXd elements(count, 6);
    std::vector<bool> valid(count, true);

    const Instant& epoch = anObservationStateArray.accessFirst().accessInstant();

    for (Size i = 0; i < count; ++i)
    {
        const State& state = anObservationStateArray[i];

        if (!state.isDefined())
        {
            throw ostk::core::error::RuntimeError(
                "Observation state at index [" + std::to_string(i) + "] is undefined."
            );
        }

        const State stateInGCRF = state.inFrame(Frame::GCRF());

        time(i) = (stateInGCRF.accessInstant() - epoch).inSeconds();

        try
        {
            const BrouwerLyddaneMeanLong meanElements = BrouwerLyddaneMeanLong::Cartesian(
                {stateInGCRF.getPosition(), stateInGCRF.getVelocity()}, gravitationalParameter
            );

            elements(i, 0) = meanElements.getSemiMajorAxis().inMeters();
            elements(i, 1) = meanElements.getEccentricity();
            elements(i, 2) = meanElements.getInclination().inRadians();
            elements(i, 3) = meanElements.getRaan().inRadians();
            elements(i, 4) = meanElements.getAop().inRadians();
            elements(i, 5) = meanElements.getMeanAnomaly().inRadians();

            valid[i] = elements.row(i).allFinite() && (elements(i, 0) > 0.0) && (elements(i, 1) < 1.0);
        }
        catch (const ostk::core::error::Exception&)
        {
            valid[i] = false;
        }
    }

    VectorXi inlierMask(count);
    MatrixXd deviations = MatrixXd::Constant(count, ChannelCount, nan);
    VectorXd gates = VectorXd::Constant(ChannelCount, minimumDeviation);

    for (Size i = 0; i < count; ++i)
    {
        inlierMask(i) = valid[i] ? 1 : 0;
    }

    if (std::none_of(
            valid.begin(),
            valid.end(),
            [](bool isValid)
            {
                return isValid;
            }
        ))
    {
        return std::make_shared<Analysis>(inlierMask, deviations, gates);
    }

    // Reference elements and J2 secular rates from the medians

    const double referenceSemiMajorAxis = Median(Select(elements.col(0), valid));
    const double referenceEccentricity = Median(Select(elements.col(1), valid));
    const double referenceInclination = Median(Select(elements.col(2), valid));
    const double referenceTime = Median(Select(time, valid));

    const double raanRate = COE::ComputeNodalPrecessionRate(
        referenceSemiMajorAxis,
        referenceEccentricity,
        referenceInclination,
        gravitationalParameter,
        equatorialRadius,
        j2
    );
    const double longitudeRate = raanRate + COE::ComputeArgumentOfLatitudeAngularRate(
                                                referenceSemiMajorAxis,
                                                referenceEccentricity,
                                                referenceInclination,
                                                gravitationalParameter,
                                                equatorialRadius,
                                                j2
                                            );

    const VectorXd centeredTime = time.array() - referenceTime;

    // Wrapped angular channels are first referred to their circular mean so that the cluster never straddles ±pi

    VectorXd nodeAngle(count);
    VectorXd phaseAngle(count);
    double nodeSineSum = 0.0, nodeCosineSum = 0.0, phaseSineSum = 0.0, phaseCosineSum = 0.0;

    for (Size i = 0; i < count; ++i)
    {
        nodeAngle(i) = Wrap(elements(i, 3) - raanRate * centeredTime(i));
        phaseAngle(i) = Wrap(elements(i, 3) + elements(i, 4) + elements(i, 5) - longitudeRate * centeredTime(i));

        if (valid[i])
        {
            nodeSineSum += std::sin(nodeAngle(i));
            nodeCosineSum += std::cos(nodeAngle(i));
            phaseSineSum += std::sin(phaseAngle(i));
            phaseCosineSum += std::cos(phaseAngle(i));
        }
    }

    const double nodeReference = std::atan2(nodeSineSum, nodeCosineSum);
    const double phaseReference = std::atan2(phaseSineSum, phaseCosineSum);

    // Channels in metres

    MatrixXd channels(count, ChannelCount);

    for (Size i = 0; i < count; ++i)
    {
        const double eccentricity = elements(i, 1);
        const double longitudeOfPeriapsis = elements(i, 3) + elements(i, 4);

        channels(i, 0) = elements(i, 0);
        channels(i, 1) = referenceSemiMajorAxis * eccentricity * std::cos(longitudeOfPeriapsis);
        channels(i, 2) = referenceSemiMajorAxis * eccentricity * std::sin(longitudeOfPeriapsis);
        channels(i, 3) = referenceSemiMajorAxis * elements(i, 2);
        channels(i, 4) = referenceSemiMajorAxis * std::sin(referenceInclination) * Wrap(nodeAngle(i) - nodeReference);
        channels(i, 5) = referenceSemiMajorAxis * Wrap(phaseAngle(i) - phaseReference);
    }

    // Per channel: robust linear detrend, robust scale, gate

    for (Size channel = 0; channel < ChannelCount; ++channel)
    {
        const double slope = RobustSlope(centeredTime, channels.col(channel), valid);

        VectorXd residual = channels.col(channel) - slope * centeredTime;
        residual.array() -= Median(Select(residual, valid));

        const double sigma = MadToSigma * Median(Select(residual.cwiseAbs(), valid));

        gates(channel) = std::max(static_cast<double>(gateFactor_) * sigma, minimumDeviation);

        for (Size i = 0; i < count; ++i)
        {
            if (!valid[i])
            {
                continue;
            }

            deviations(i, channel) = residual(i);

            if (std::abs(residual(i)) > gates(channel))
            {
                inlierMask(i) = 0;
            }
        }
    }

    return std::make_shared<Analysis>(inlierMask, deviations, gates);
}

Array<String> MeanElementConsistency::ChannelNames()
{
    return {"semi_major_axis", "eccentricity_x", "eccentricity_y", "inclination", "node", "phase"};
}

}  // namespace observationfilter
}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk
