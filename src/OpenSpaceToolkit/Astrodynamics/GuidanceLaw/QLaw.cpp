/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanShort.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::type::Index;

using ostk::mathematics::object::Vector6d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;

using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanShort;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

QLaw::Parameters::Parameters(
    const Map<COE::Element, Tuple<double, double>>& anElementWeightsMap,
    const Size& aMValue,
    const Size& aNValue,
    const Size& aRValue,
    const double& aBValue,
    const Size& aKValue,
    const double& aPeriapsisWeight,
    const Length& minimumPeriapsisradius,
    const Real& anAbsoluteEffectivityThreshold,
    const Real& aRelativeEffectivityThreshold
)
    : m(aMValue),
      n(aNValue),
      r(aRValue),
      b(aBValue),
      k(aKValue),
      periapsisWeight(aPeriapsisWeight),
      absoluteEffectivityThreshold(anAbsoluteEffectivityThreshold),
      relativeEffectivityThreshold(aRelativeEffectivityThreshold),
      minimumPeriapsisRadius_(minimumPeriapsisradius.inMeters()),
      convergenceThresholds_(Vector5d::Ones() * 1e-10),
      controlWeights_(Vector5d::Zero())
{
    if (anElementWeightsMap.empty())
    {
        throw ostk::core::error::RuntimeError("ElementWeights is empty. Must target at least one element.");
    }

    for (const auto& it : anElementWeightsMap)
    {
        if (!validElements_.contains(it.first))
        {
            throw ostk::core::error::RuntimeError("Cannot target [" + COE::StringFromElement(it.first) + "].");
        }
    }

    Index i = 0;
    for (const COE::Element& element : validElements_)
    {
        if (anElementWeightsMap.count(element))
        {
            controlWeights_(i) = std::get<0>(anElementWeightsMap.at(element));
            convergenceThresholds_(i) = std::get<1>(anElementWeightsMap.at(element));
        }
        ++i;
    }

    if (anAbsoluteEffectivityThreshold.isDefined())
    {
        if (anAbsoluteEffectivityThreshold < 0.0 || anAbsoluteEffectivityThreshold > 1.0)
        {
            throw ostk::core::error::RuntimeError("Absolute effectivity threshold must be within range [0.0, 1.0].");
        }
    }

    if (aRelativeEffectivityThreshold.isDefined())
    {
        if (aRelativeEffectivityThreshold < 0.0 || aRelativeEffectivityThreshold > 1.0)
        {
            throw ostk::core::error::RuntimeError("Absolute effectivity threshold must be within range [0.0, 1.0].");
        }
    }
}

Vector5d QLaw::Parameters::getControlWeights() const
{
    return controlWeights_;
}

Length QLaw::Parameters::getMinimumPeriapsisRadius() const
{
    return Length::Meters(minimumPeriapsisRadius_);
}

QLaw::QLaw(
    const COE& aCOE,
    const Derived& aGravitationalParameter,
    const QLaw::Parameters& aParameterSet,
    const GradientStrategy& aGradientStrategy
)
    : GuidanceLaw("Q-Law"),
      parameters_(aParameterSet),
      mu_(aGravitationalParameter.in(Derived::Unit::MeterCubedPerSecondSquared())),
      targetCOEVector_(aCOE.getSIVector(COE::AnomalyType::True)),
      gravitationalParameter_(aGravitationalParameter),
      gradientStrategy_(aGradientStrategy),
      finiteDifferenceSolver_(
          FiniteDifferenceSolver(FiniteDifferenceSolver::Type::Central, 1e-3, Duration::Seconds(1e-6))
      ),
      stateBuilder_(Frame::GCRF(), {std::make_shared<CoordinateSubset>("QLaw Element Vector", 5)}),
      coeDomain_(COEDomain::Osculating)
{
}

QLaw::~QLaw() {}

std::ostream& operator<<(std::ostream& anOutputStream, const QLaw& aGuidanceLaw)
{
    aGuidanceLaw.print(anOutputStream);
    return anOutputStream;
}

void QLaw::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "QLaw") : void();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

QLaw::Parameters QLaw::getParameters() const
{
    return parameters_;
}

COE QLaw::getTargetCOE() const
{
    return COE::FromSIVector(targetCOEVector_, COE::AnomalyType::True);
}

QLaw::GradientStrategy QLaw::getGradientStrategy() const
{
    return gradientStrategy_;
}

QLaw::COEDomain QLaw::getCOEDomain() const
{
    return coeDomain_;
}

void QLaw::setCOEDomain(const COEDomain& aCOEDomain)
{
    coeDomain_ = aCOEDomain;
}

Vector3d QLaw::calculateThrustAccelerationAt(
    [[maybe_unused]] const Instant& anInstant,
    const Vector3d& aPositionCoordinates,
    const Vector3d& aVelocityCoordinates,
    const Real& aThrustAcceleration,
    const Shared<const Frame>& outputFrameSPtr
) const
{
    const COE::CartesianState cartesianState = {
        Position::Meters(aPositionCoordinates, outputFrameSPtr),
        Velocity::MetersPerSecond(aVelocityCoordinates, outputFrameSPtr)
    };

    Vector6d coeVector;

    switch (coeDomain_)
    {
        case COEDomain::Osculating:
        {
            const COE coe = COE::Cartesian(cartesianState, gravitationalParameter_);
            coeVector = coe.getSIVector(COE::AnomalyType::True);
            break;
        }
        case COEDomain::BrouwerLyddaneMeanLong:
        {
            const BrouwerLyddaneMeanLong coe =
                BrouwerLyddaneMeanLong::Cartesian(cartesianState, gravitationalParameter_);
            coeVector = coe.getSIVector(COE::AnomalyType::True);
            break;
        }
        case COEDomain::BrouwerLyddaneMeanShort:
        {
            const BrouwerLyddaneMeanShort coe =
                BrouwerLyddaneMeanShort::Cartesian(cartesianState, gravitationalParameter_);
            coeVector = coe.getSIVector(COE::AnomalyType::True);
            break;
        }
        default:
        {
            throw ostk::core::error::runtime::ToBeImplemented("COE Domain");
        }
    }

    coeVector[1] = std::max(coeVector[1], 1e-4);
    coeVector[2] = std::max(coeVector[2], 1e-4);

    const Vector3d thrustDirection = computeThrustDirection(coeVector, aThrustAcceleration);

    const Matrix3d R_thetaRH_GCRF = QLaw::ThetaRHToGCRF(aPositionCoordinates, aVelocityCoordinates);

    return aThrustAcceleration * R_thetaRH_GCRF * thrustDirection;
}

Vector5d QLaw::compute_dQ_dOE(const Vector5d& aCOEVector, const double& aThrustAcceleration) const
{
    if (gradientStrategy_ == GradientStrategy::Analytical)
    {
        return computeAnalytical_dQ_dOE(aCOEVector, aThrustAcceleration);
    }
    else
    {
        return computeNumerical_dQ_dOE(aCOEVector, aThrustAcceleration);
    }
}

Vector3d QLaw::computeThrustDirection(const Vector6d& aCOEVector, const double& aThrustAcceleration) const
{
    const Vector5d coeVectorSegment = aCOEVector.segment(0, 5);

    const Vector5d deltaCOE = computeDeltaCOE(coeVectorSegment);

    // Within tolerance of all targeted elements. No need to thrust.
    if ((parameters_.controlWeights_.array() * deltaCOE.array().abs() <= parameters_.convergenceThresholds_.array())
            .all())
    {
        return {0.0, 0.0, 0.0};
    }

    const Matrix53d derivativeMatrix = QLaw::Compute_dOE_dF(aCOEVector, gravitationalParameter_);

    const Vector5d dQ_dOE = compute_dQ_dOE(coeVectorSegment, aThrustAcceleration);

    if (dQ_dOE.array().isNaN().any())
    {
        throw ostk::core::error::RuntimeError("NaN encountered in dQ_dOE calcluation.");
    }

    const Vector3d thrustDirection = dQ_dOE.transpose() * derivativeMatrix;

    if (parameters_.relativeEffectivityThreshold.isDefined() || parameters_.absoluteEffectivityThreshold.isDefined())
    {
        double etaRelative = 0.0;
        double etaAbsolute = 0.0;

        std::tie(etaRelative, etaAbsolute) = computeEffectivity(aCOEVector, thrustDirection, dQ_dOE);

        // If the relative effectivity is below the threshold, do not thrust.
        if ((parameters_.relativeEffectivityThreshold.isDefined()) &&
            (etaRelative < parameters_.relativeEffectivityThreshold))
        {
            return {0.0, 0.0, 0.0};
        }

        // If the absolute effectivity is below the threshold, do not thrust.
        if ((parameters_.absoluteEffectivityThreshold.isDefined()) &&
            (etaAbsolute < parameters_.absoluteEffectivityThreshold))
        {
            return {0.0, 0.0, 0.0};
        }
    }

    return -thrustDirection.normalized();
}

double QLaw::computeQ(const Vector5d& aCOEVector, const double& aThrustAcceleration) const
{
    //                                                2
    //                                    ⎛  ⎛      T⎞⎞
    //                                    ⎜d ⎝oe, oe ⎠⎟
    //                    ___             ⎜───────────⎟
    // Q = ⎛1 + W  ⋅ P⎞ ⋅ ╲   W   ⋅ S   ⋅ ⎜     .     ⎟
    //     ⎝     P    ⎠   ╱    oe    oe   ⎜   oe      ⎟
    //                    ‾‾‾             ⎝     xx    ⎠
    //                    oe

    // (1 + W_p * P)
    const double periapsisRadius = aCOEVector[0] * (1.0 - aCOEVector[1]);
    const double P = std::exp(parameters_.k * (1.0 - (periapsisRadius / parameters_.minimumPeriapsisRadius_)));

    const double periapsisScaling = (1.0 + (parameters_.periapsisWeight * P));

    //   ⎛      T⎞
    // d ⎝oe, oe ⎠
    const Vector5d deltaCOE = computeDeltaCOE(aCOEVector);

    // S_oe
    const Vector5d scalingCOE = {
        std::pow(
            (1.0 + std::pow(deltaCOE[0] / (parameters_.m * targetCOEVector_[0]), parameters_.n)), 1.0 / parameters_.r
        ),
        1.0,
        1.0,
        1.0,
        1.0,
    };

    // oedot_xx
    const Vector5d maximalCOE = computeOrbitalElementsMaximalChange(aCOEVector, aThrustAcceleration);

    const Vector5d deltaCOE_divided_maximalCOE = (deltaCOE.cwiseQuotient(maximalCOE));

    return periapsisScaling * (parameters_.controlWeights_.cwiseProduct(scalingCOE)
                                   .cwiseProduct(deltaCOE_divided_maximalCOE.cwiseProduct(deltaCOE_divided_maximalCOE)))
                                  .sum();
}

Vector5d QLaw::computeOrbitalElementsMaximalChange(const Vector5d& aCOEVector, const double& aThrustAcceleration) const
{
    const double& semiMajorAxis = aCOEVector[0];
    const double& eccentricity = aCOEVector[1];
    const double& inclination = aCOEVector[2];
    const double& argumentOfPeriapsis = aCOEVector[4];

    const double semiLatusRectum = COE::ComputeSemiLatusRectum(semiMajorAxis, eccentricity);
    const double angularMomentum = COE::ComputeAngularMomentum(semiLatusRectum, gravitationalParameter_);

    // common grouped terms
    const double eccentricitySquared = eccentricity * eccentricity;
    const double aop_sin = std::sin(argumentOfPeriapsis);
    const double aop_cos = std::cos(argumentOfPeriapsis);

    // Semi-Major Axis
    //
    //                            ___________________________________
    //                           ╱              3
    //                          ╱  semiMajorAxis ⋅(eccentricity + 1)
    // 2⋅aThrustAcceleration⋅  ╱   ─────────────────────────────────
    //                       ╲╱           μ⋅(1 - eccentricity)
    //

    const double semiMajorAxis_xx =
        (2.0 * aThrustAcceleration *
         std::sqrt(std::pow(semiMajorAxis, 3) * (1.0 + eccentricity) / (mu_ * (1.0 - eccentricity))));

    // Eccentricity
    //
    // 2⋅aThrustAcceleration⋅semiLatusRectum
    // ─────────────────────────────────────
    //           angularMomentum

    const double eccentricity_xx = 2.0 * semiLatusRectum * aThrustAcceleration / angularMomentum;

    // Inclination
    //                                         aThrustAcceleration⋅semiLatusRectum
    // ───────────────────────────────────────────────────────────────────────────────────────────────────────────────
    //                 ⎛                                              _______________________________________________⎞
    //                 ⎜                                             ╱               2      2                        ⎟
    // angularMomentum⋅⎝-eccentricity⋅│cos(argumentOfPeriapsis)│ + ╲╱ 1 - eccentricity ⋅ sin(argumentOfPeriapsis)    ⎠

    const double inclination_xx = (semiLatusRectum * aThrustAcceleration) /
                                  (angularMomentum * (std::sqrt(1.0 - (eccentricitySquared * aop_sin * aop_sin)) -
                                                      eccentricity * std::abs(aop_cos)));

    // Right Ascension of the Ascending Node
    // clang-format off
    //                                              aThrustAcceleration⋅semiLatusRectum
    // ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
    //                 ⎛                                              ________________________________________⎞
    //                 ⎜                                             ╱               2    2                   ⎟
    // angularMomentum⋅⎝-eccentricity⋅│sin(argumentOfPeriapsis)│ + ╲╱1 - eccentricity ⋅ cos(argumentOfPeriapsis)⎠⋅sin(inclination)
    // clang-format on

    const double rightAscensionOfAscendingNode_xx =
        (semiLatusRectum * aThrustAcceleration) /
        (angularMomentum * std::sin(inclination) *
         (std::sqrt(1.0 - (eccentricitySquared * aop_cos * aop_cos)) - eccentricity * std::abs(aop_sin)));

    // Argument of Periapsis
    // Too complicated to print here. See the paper.
    const double alpha = (1.0 - eccentricitySquared) / (2.0 * std::pow(eccentricity, 3));
    const double beta = std::sqrt(alpha * alpha + 1.0 / 27.0);

    const double cosTheta_xx =
        std::pow((alpha + beta), 1.0 / 3.0) - std::pow((beta - alpha), 1.0 / 3.0) - 1.0 / eccentricity;
    const double r_xx = semiLatusRectum / (1.0 + (eccentricity * cosTheta_xx));

    const double cosTheta_xxSquared = cosTheta_xx * cosTheta_xx;

    const double argumentOfPeriapsisI_xx = (aThrustAcceleration / (eccentricity * angularMomentum)) *
                                           std::sqrt(
                                               semiLatusRectum * semiLatusRectum * cosTheta_xxSquared +
                                               std::pow((semiLatusRectum + r_xx), 2) * (1.0 - cosTheta_xxSquared)
                                           );

    const double argumentOfPeriapsisO_xx = rightAscensionOfAscendingNode_xx * std::abs(std::cos(inclination));
    const double argumentOfPeriapsis_xx =
        (argumentOfPeriapsisI_xx + parameters_.b * argumentOfPeriapsisO_xx) / (1.0 + parameters_.b);

    return {
        semiMajorAxis_xx,
        eccentricity_xx,
        inclination_xx,
        rightAscensionOfAscendingNode_xx,
        argumentOfPeriapsis_xx,
    };
}

Matrix3d QLaw::ThetaRHToGCRF(const Vector3d& aPositionCoordinates, const Vector3d& aVelocityCoordinates)
{
    const Vector3d R = aPositionCoordinates.normalized();
    const Vector3d H = (aPositionCoordinates.cross(aVelocityCoordinates)).normalized();
    const Vector3d theta = H.cross(R);

    Matrix3d rotationMatrix;
    rotationMatrix.col(0) = theta;
    rotationMatrix.col(1) = R;
    rotationMatrix.col(2) = H;

    return rotationMatrix;
}

Matrix53d QLaw::Compute_dOE_dF(const Vector6d& aCOEVector, const Derived& aGravitationalParameter)
{
    const double& semiMajorAxis = aCOEVector[0];
    const double& eccentricity = aCOEVector[1];
    const double& inclination = aCOEVector[2];
    const double& argumentOfPeriapsis = aCOEVector[4];
    const double& trueAnomaly = aCOEVector[5];

    const double semiLatusRectum = COE::ComputeSemiLatusRectum(semiMajorAxis, eccentricity);
    const double angularMomentum = COE::ComputeAngularMomentum(semiLatusRectum, aGravitationalParameter);
    const double radialDistance = COE::ComputeRadialDistance(semiMajorAxis, eccentricity, trueAnomaly);

    // columns: Orbital elements
    // rows: theta, radial, angular momentum directions
    Matrix53d derivativeMatrix = Matrix53d::Zero();

    // Common grouped operations
    const double trueAnomaly_sin = std::sin(trueAnomaly);
    const double trueAnomaly_cos = std::cos(trueAnomaly);
    const double trueAnomaly_ArgumentOfPeriapsis_sin = std::sin(trueAnomaly + argumentOfPeriapsis);
    const double trueAnomaly_ArgumentOfPeriapsis_cos = std::cos(trueAnomaly + argumentOfPeriapsis);

    // Semi-major axis
    const double sma_alpha = (2.0 * semiMajorAxis * semiMajorAxis / angularMomentum);
    derivativeMatrix(0, 0) = sma_alpha * semiLatusRectum / radialDistance;
    derivativeMatrix(0, 1) = sma_alpha * eccentricity * trueAnomaly_sin;

    // Eccentricity
    derivativeMatrix(1, 0) =
        (((semiLatusRectum + radialDistance) * trueAnomaly_cos) + (radialDistance * eccentricity)) / angularMomentum;
    derivativeMatrix(1, 1) = (semiLatusRectum * trueAnomaly_sin / angularMomentum);

    // Inclination
    derivativeMatrix(2, 2) = (radialDistance * trueAnomaly_ArgumentOfPeriapsis_cos / angularMomentum);

    // Right Ascension of the Ascending Node
    derivativeMatrix(3, 2) =
        (radialDistance * trueAnomaly_ArgumentOfPeriapsis_sin) / (angularMomentum * std::sin(inclination));

    // Argument of Periapsis
    const double aop_alpha = 1.0 / (eccentricity * angularMomentum);
    derivativeMatrix(4, 0) = (semiLatusRectum + radialDistance) * trueAnomaly_sin * aop_alpha;
    derivativeMatrix(4, 1) = -semiLatusRectum * trueAnomaly_cos * aop_alpha;
    derivativeMatrix(4, 2) = (-radialDistance * trueAnomaly_ArgumentOfPeriapsis_sin * std::cos(inclination)) /
                             (angularMomentum * std::sin(inclination));

    return derivativeMatrix;
}

Vector5d QLaw::computeAnalytical_dQ_dOE(const Vector5d& aCOEVector, const double& aThrustAcceleration) const
{
    const double& semiMajorAxis = aCOEVector(0);
    const double& eccentricity = aCOEVector(1);
    const double& inclination = aCOEVector(2);
    const double& rightAscensionOfAscendingNode = aCOEVector(3);
    const double& argumentOfPeriapsis = aCOEVector(4);

    const double& semiMajorAxisTarget = targetCOEVector_(0);
    const double& eccentricityTarget = targetCOEVector_(1);
    const double& inclinationTarget = targetCOEVector_(2);
    const double& rightAscensionOfAscendingNodeTarget = targetCOEVector_(3);
    const double& argumentOfPeriapsisTarget = targetCOEVector_(4);

    const double& semiMajorAxisWeight = parameters_.controlWeights_(0);
    const double& eccentricityWeight = parameters_.controlWeights_(1);
    const double& inclinationWeight = parameters_.controlWeights_(2);
    const double& rightAscensionOfAscendingNodeWeight = parameters_.controlWeights_(3);
    const double& argumentOfPeriapsisWeight = parameters_.controlWeights_(4);

    const double& periapsisWeight = parameters_.periapsisWeight;
    const double& minimumPeriapsisRadius = parameters_.minimumPeriapsisRadius_;

    // common grouped expressions
    const double x0 = 1.0 / minimumPeriapsisRadius;
    const double x1 = eccentricity - 1.0;
    const double x2 = x0 * x1;
    const double x3 = eccentricity - eccentricityTarget;
    const double x4 = eccentricityWeight * std::pow(x3, 2.0);
    const double x5 = 1.0 / semiMajorAxis;
    const double x6 = std::pow(eccentricity, 2.0);
    const double x7 = x6 - 1.0;
    const double x8 = 1.0 / x7;
    const double x9 = x5 * x8;
    const double x10 = semiMajorAxis - semiMajorAxisTarget;
    const double x11 = std::pow(x10, 2.0);
    const double x12 = std::pow(semiMajorAxis, -3.0);
    const double x13 = eccentricity + 1.0;
    const double x14 = 1.0 / x13;
    const double x15 = std::pow(x10 / (parameters_.m * semiMajorAxisTarget), parameters_.n);
    const double x16 = x15 + 1.0;
    const double x17 = 1.0 / parameters_.r;
    const double x18 = std::pow(x16, x17);
    const double x19 = semiMajorAxisWeight * x12 * x14 * x18;
    const double x20 = x11 * x19;
    const double x21 = 4.0 * x9;
    const double x22 = inclination - inclinationTarget;
    const double x23 = std::pow(x22, 2.0);
    const double x24 = std::cos(argumentOfPeriapsis);
    const double x25 = std::fabs(x24);
    const double x26 = std::sin(argumentOfPeriapsis);
    const double x27 = std::pow(x26, 2.0);
    const double x28 = std::sqrt(-x27 * x6 + 1.0);
    const double x29 = eccentricity * x25 - x28;
    const double x30 = inclinationWeight * std::pow(x29, 2.0);
    const double x31 = x23 * x30;
    const double x32 = std::fabs(x26);
    const double x33 = std::pow(x24, 2.0);
    const double x34 = std::sqrt(-x33 * x6 + 1.0);
    const double x35 = eccentricity * x32 - x34;
    const double x36 = std::pow(x35, 2.0);
    const double x37 = rightAscensionOfAscendingNode - rightAscensionOfAscendingNodeTarget;
    const double x38 = std::cos(x37);
    const double x39 = std::acos(x38);
    const double x40 = std::pow(x39, 2.0);
    const double x41 = std::sin(inclination);
    const double x42 = std::pow(x41, 2.0);
    const double x43 = rightAscensionOfAscendingNodeWeight * x40 * x42;
    const double x44 = x36 * x43;
    const double x45 = semiMajorAxis * x7;
    const double x46 = std::pow(parameters_.b + 1.0, 2.0);
    const double x47 = 1.0 / x35;
    const double x48 = std::cos(inclination);
    const double x49 = std::fabs(x48);
    const double x50 = parameters_.b * x49 / x41;
    const double x51 = 1.0 / eccentricity;
    const double x52 = -x7;
    const double x53 = x52 / std::pow(eccentricity, 3.0);
    const double x54 = std::sqrt(0.14814814814814814 + std::pow(x52, 2.0) / std::pow(eccentricity, 6.0));
    const double x55 = x53 + x54;
    const double x56 = std::max(0.0, -x53 + x54);
    const double x57 =
        x51 - 0.79370052598409979 * std::pow(x55, (1.0 / 3.0)) + 0.79370052598409979 * std::pow(x56, (1.0 / 3.0));
    const double x58 = -x57;
    const double x59 = std::pow(x58, 2.0);
    const double x60 = x59 - 1.0;
    const double x61 = std::fabs(semiMajorAxis);
    const double x62 = std::fabs(x7);
    const double x63 = x51 * x61 * x62;
    const double x64 =
        x45 * x47 * x50 +
        x63 * std::sqrt(std::pow(x57, 2.0) - x60 * std::pow(1.0 + 1.0 / (-eccentricity * x57 + 1.0), 2.0));
    const double x65 = std::pow(x64, -2.0);
    const double x66 = argumentOfPeriapsis - argumentOfPeriapsisTarget;
    const double x67 = std::cos(x66);
    const double x68 = std::acos(x67);
    const double x69 = std::pow(x68, 2.0);
    const double x70 = 4.0 * x69;
    const double x71 = periapsisWeight * std::exp(parameters_.k * (semiMajorAxis * x2 + 1.0));
    const double x72 = parameters_.k * x71 *
                       (argumentOfPeriapsisWeight * x45 * x46 * x65 * x70 + x1 * x20 + x21 * x31 + x21 * x44 + x4 * x9);
    const double x73 = x71 + 1.0;
    const double x74 = mu_ * x4;
    const double x75 = std::pow(semiMajorAxis, 2.0);
    const double x76 = 1.0 / x52;
    const double x77 = x76 / x75;
    const double x78 = -x1;
    const double x79 = mu_ * x10 * x19 * x78;
    const double x80 = inclinationWeight * x23;
    const double x81 = 4.0 * mu_ * x77;
    const double x82 = mu_ * semiMajorAxisWeight * x11 * x18;
    const double x83 = -x35;
    const double x84 = std::pow(x83, 2.0);
    const double x85 = x50 / x83;
    const double x86 = x52 * x85;
    const double x87 = -x60;
    const double x88 = eccentricity * x58 + 1.0;
    const double x89 = 1.0 / x88;
    const double x90 = x89 + 1.0;
    const double x91 = std::pow(x90, 2.0);
    const double x92 = std::sqrt(x59 + x87 * x91);
    const double x93 = x51 * x52 / (x61 * x62 * x92);
    const double x94 = 2.0 * semiMajorAxis;
    const double x95 = mu_ * semiMajorAxis;
    const double x96 = x52 * x95;
    const double x97 = argumentOfPeriapsisWeight * x46 * x70;
    const double x98 = std::pow(aThrustAcceleration, -2.0);
    const double x99 = (1.0 / 4.0) * x98;
    const double x100 = mu_ * x9;
    const double x101 = 2.0 * eccentricity;
    const double x102 = std::pow(x7, 2.0);
    const double x103 = x5 / x102;
    const double x104 = eccentricity * mu_;
    const double x105 = 8.0 * x103 * x104;
    const double x106 = eccentricity / x28;
    const double x107 = 8.0 * x100;
    const double x108 = eccentricity / x34;
    const double x109 = x108 * x33 + x32;
    const double x110 = x35 * x43;
    const double x111 = std::pow(x64, -3.0);
    const double x112 = 1.0 / x6;
    const double x113 = 2.0 * x61 * x62 * x92;
    const double x114 = x112 * x52;
    const double x115 = (1.0 / 3.0) * x53 * (3.0 * x114 + 2.0) / x54;
    const double x116 = x112 * (1.0 - 1.0 * x6) + (2.0 / 3.0);
    const double x117 = std::pow(x55, -(2.0 / 3.0)) * (x115 + x116);
    const double x118 = std::pow(std::max(x56, 1e-15), -(2.0 / 3.0)) * (-x115 + x116);
    const double x119 = -1.5874010519681996 * x117 - 1.5874010519681996 * x118 + 2.0;
    const double x120 = rightAscensionOfAscendingNodeWeight * x100 * x36;
    const double x121 = argumentOfPeriapsisWeight * x102 * x111 * x46 * x69 * x75;
    const double x122 = 2.0 * x73 * x98;
    const double x123 = x24 * (x108 * x26 - (((x26) > 0) - ((x26) < 0)));

    double dQ_dSemiMajorAxis =
        -x99 * (mu_ * x2 * x72 + x73 * (-parameters_.n * x15 * x17 * x79 / x16 + std::pow(x29, 2.0) * x80 * x81 +
                                        x43 * x81 * x84 + x74 * x77 - 2.0 * x79 -
                                        x96 * x97 *
                                            (x5 * x51 * x61 * x62 * x92 - x86 -
                                             x93 * x94 * (x52 * x59 + x87 * x90 * (x52 * x89 - x6 + 1.0))) /
                                            std::pow(semiMajorAxis * x86 + x63 * x92, 3.0) +
                                        3.0 * x14 * x78 * x82 / std::pow(semiMajorAxis, 4.0)));

    double dQ_dEccentricity =
        -x99 *
        (x0 * x72 * x95 +
         x73 *
             (2.0 * eccentricityWeight * x100 * x3 + mu_ * x20 - x1 * x12 * x82 / std::pow(x13, 2.0) -
              x101 * x103 * x74 - x105 * x31 - x105 * x44 + x107 * x109 * x110 + x107 * x29 * x80 * (x106 * x27 + x25) +
              x111 * x97 * std::pow(-x7 * x95, 3.0 / 2.0) *
                  (-semiMajorAxis * x101 * x85 + x109 * x50 * x52 * x94 / x84 - x112 * x113 + x113 * x76 +
                   x75 * x93 *
                       (-4.0 * eccentricity * x59 + x112 * x119 * x52 * x58 - x114 * x119 * x58 * x91 +
                        2.0 * x87 * x90 *
                            (-x101 * x89 - x101 +
                             x52 * (-x51 * (-0.79370052598409979 * x117 - 0.79370052598409979 * x118 + 1.0) + x57) /
                                 std::pow(x88, 2.0)))) /
                  std::sqrt(x96)));

    double dQ_dInclination =
        -x122 * (mu_ * parameters_.b * x121 * x47 * ((((x48) > 0) - ((x48) < 0)) + x48 * x49 / x42) + x100 * x22 * x30 +
                 x120 * x40 * x41 * x48);

    double x124 = std::sin(x37) / std::sqrt(1.0 - std::pow(x38, 2.0));
    if (!std::isfinite(x124))
    {
        x124 = 0.0;
    }

    double dQ_dRightAscensionOfAscendingNode = -x120 * x122 * x39 * x42 * x124;

    double x125 = std::sin(x66) / std::sqrt(1.0 - std::pow(x67, 2.0));
    if (!std::isfinite(x125))
    {
        x125 = 0.0;
    }

    double dQ_dArgumentOfPeriapsis =
        -x122 * (argumentOfPeriapsisWeight * mu_ * semiMajorAxis * x46 * x65 * x68 * x7 * x125 +
                 eccentricity * inclinationWeight * mu_ * x23 * x26 * x29 * x5 * x8 *
                     (x106 * x24 - (((x24) > 0) - ((x24) < 0))) -
                 eccentricity * x100 * x110 * x123 - x104 * x121 * x123 * x50 / x36);

    return {
        dQ_dSemiMajorAxis,
        dQ_dEccentricity,
        dQ_dInclination,
        dQ_dRightAscensionOfAscendingNode,
        dQ_dArgumentOfPeriapsis,
    };
}

Vector5d QLaw::computeNumerical_dQ_dOE(const Vector5d& aCOEVector, const double& aThrustAcceleration) const
{
    const auto getQ = [this,
                       &aThrustAcceleration](const State& aState, [[maybe_unused]] const Instant& anInstant) -> VectorXd
    {
        const Vector5d& coeVector = aState.accessCoordinates();

        VectorXd coordinates(1);
        coordinates << this->computeQ(coeVector, aThrustAcceleration);

        return coordinates;
    };

    const Vector5d jacobian = Eigen::Map<Vector5d>(
        finiteDifferenceSolver_
            .computeStateTransitionMatrix(stateBuilder_.build(Instant::J2000(), aCOEVector), Instant::J2000(), getQ)
            .data(),
        5
    );

    return jacobian;
}

Vector5d QLaw::computeDeltaCOE(const Vector5d& aCOEVector) const
{
    return {
        (aCOEVector[0] - targetCOEVector_[0]),
        (aCOEVector[1] - targetCOEVector_[1]),
        (aCOEVector[2] - targetCOEVector_[2]),
        (std::acos(std::cos((aCOEVector[3] - targetCOEVector_[3])))),
        (std::acos(std::cos((aCOEVector[4] - targetCOEVector_[4])))),
    };
}

Tuple<double, double> QLaw::computeEffectivity(
    const Vector6d& aCOEVector, const Vector3d& currentThrustDirection, const Vector5d& dQ_dOE
) const
{
    // Note: As Q is a Lyapunov function, Q̇ is always negative. Therefore, the most effective thrust direction is the
    // one that minimizes Q̇.
    // Q̇ = D1*cos(β)*cos(⍺) + D2*cos(β)*sin(⍺) + D3*sin(β)
    const auto compute_dQn_dt = [](const Vector3d& aThrustDirection)
    {
        const double alphaStar = std::atan2(-aThrustDirection[1], -aThrustDirection[0]);
        const double betaStar = std::atan(
            -aThrustDirection[2] /
            std::sqrt(aThrustDirection[0] * aThrustDirection[0] + aThrustDirection[1] * aThrustDirection[1])
        );

        return aThrustDirection[0] * std::cos(alphaStar) * std::cos(betaStar) +
               aThrustDirection[1] * std::sin(alphaStar) * std::cos(betaStar) +
               aThrustDirection[2] * std::sin(betaStar);
    };

    Vector6d coeVector = aCOEVector;
    VectorXd dQ_dt(trueAnomalyAngles_.size());

    // For each true anomaly, compute Q̇
    // Coarse grid search is sufficient, no need to for finding the exact root.
    Index i = 0;
    for (const double& trueAnomalyAngle : trueAnomalyAngles_)
    {
        coeVector[5] = trueAnomalyAngle;

        const Matrix53d dOE_dF = QLaw::Compute_dOE_dF(coeVector, gravitationalParameter_);
        const Vector3d thrustDirection = dQ_dOE.transpose() * dOE_dF;

        dQ_dt[i] = compute_dQn_dt(thrustDirection);

        ++i;
    }

    // Q̇n = min(Q̇) for ⍺_* and β_* (i.e. the most effective thrust direction at the current true anomaly)
    const double dQn_dt = compute_dQn_dt(currentThrustDirection);

    // Q̇nn = min(Q̇) for ⍺_* and β_* (i.e. the most effective thrust direction at the true anomaly `n`)
    const double& dQnn_dt = dQ_dt.minCoeff();
    // Q̇nx = max(Q̇) for ⍺_* and β_* (i.e. the least effective thrust direction at the true anomaly `n`)
    const double& dQnx_dt = dQ_dt.maxCoeff();

    // η = Q̇n / Q̇nn -> current Q̇ / minimum Q̇ value
    const double etaAbsolute = (dQn_dt / dQnn_dt);
    // η = (Q̇n - Q̇nx) / (Q̇nn - Q̇nx) -> (current Q̇ - maximum Q̇) / (minimum Q̇ - maximum Q̇)
    const double etaRelative = (dQn_dt - dQnx_dt) / (dQnn_dt - dQnx_dt);

    return {etaAbsolute, etaRelative};
}

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
