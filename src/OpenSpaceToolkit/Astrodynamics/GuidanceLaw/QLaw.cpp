/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace guidancelaw
{

using ostk::math::obj::Vector6d;
using ostk::math::obj::VectorXd;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesSubset;

QLaw::Parameters::Parameters(
    const Map<COE::Element, Real>& anElementWeights,
    const Size& aMValue,
    const Size& aNValue,
    const Size& aRValue,
    const Real& aBValue
)
    : m(aMValue),
      n(aNValue),
      r(aRValue),
      b(aBValue)
{
    if (anElementWeights.count(COE::Element::TrueAnomaly))
    {
        throw ostk::core::error::RuntimeError("Cannot target True Anomaly.");
    }

    if (anElementWeights.count(COE::Element::MeanAnomaly))
    {
        throw ostk::core::error::RuntimeError("Cannot target Mean Anomaly.");
    }

    if (anElementWeights.count(COE::Element::EccentricAnomaly))
    {
        throw ostk::core::error::RuntimeError("Cannot target Eccentric Anomaly.");
    }

    if (anElementWeights.empty())
    {
        throw ostk::core::error::RuntimeError("ElementWeights is empty. Must target at least one element.");
    }

    if (anElementWeights.count(COE::Element::SemiMajorAxis))
    {
        controlWeights_(0) = anElementWeights.at(COE::Element::SemiMajorAxis);
    }

    if (anElementWeights.count(COE::Element::Eccentricity))
    {
        controlWeights_(1) = anElementWeights.at(COE::Element::Eccentricity);
    }

    if (anElementWeights.count(COE::Element::Inclination))
    {
        controlWeights_(2) = anElementWeights.at(COE::Element::Inclination);
    }

    if (anElementWeights.count(COE::Element::Raan))
    {
        controlWeights_(3) = anElementWeights.at(COE::Element::Raan);
    }

    if (anElementWeights.count(COE::Element::Aop))
    {
        controlWeights_(4) = anElementWeights.at(COE::Element::Aop);
    }
}

Vector5d QLaw::Parameters::getControlWeights() const
{
    return controlWeights_;
}

QLaw::QLaw(
    const COE& aCOE,
    const Derived& aGravitationalParameter,
    const QLaw::Parameters& aParameterSet,
    const FiniteDifferenceSolver& aFiniteDifferenceSolver
)
    : GuidanceLaw("Q-Law"),
      parameters_(aParameterSet),
      mu_(aGravitationalParameter.in(aGravitationalParameter.getUnit())),
      targetCOEVector_(aCOE.getSIVector(COE::AnomalyType::True)),
      gravitationalParameter_(aGravitationalParameter),
      finiteDifferenceSolver_(aFiniteDifferenceSolver),
      stateBuilder_(Frame::GCRF(), {std::make_shared<CoordinatesSubset>("QLaw Element Vector", 5)})
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

Vector3d QLaw::calculateThrustAccelerationAt(
    [[maybe_unused]] const Instant& anInstant,
    const Vector3d& aPositionCoordinates,
    const Vector3d& aVelocityCoordinates,
    const Real& aThrustAcceleration,
    [[maybe_unused]] const Shared<const Frame>& outputFrameSPtr
) const
{
    const COE coe = COE::Cartesian(
        {Position::Meters(aPositionCoordinates, Frame::GCRF()),
         Velocity::MetersPerSecond(aVelocityCoordinates, Frame::GCRF())},
        gravitationalParameter_
    );

    Vector6d coeVector = coe.getSIVector(COE::AnomalyType::True);

    static const double singularityTolerance = 1e-4;
    coeVector[1] = std::max(coeVector[1], singularityTolerance);
    coeVector[2] = std::max(singularityTolerance, std::min(coeVector[2], singularityTolerance));

    const Vector3d thrustDirection = computeThrustDirection(coeVector, aThrustAcceleration);

    const Matrix3d R_thetaRH_GCRF = QLaw::thetaRHToGCRF(aPositionCoordinates, aVelocityCoordinates);

    return aThrustAcceleration * R_thetaRH_GCRF * thrustDirection;
}

Vector3d QLaw::computeThrustDirection(const Vector6d& currentCOEVector, const Real& aThrustAcceleration) const
{
    const Matrix53d derivativeMatrix = computeDOEWithF(currentCOEVector);

    const auto getQ = [this,
                       &aThrustAcceleration](const State& aState, [[maybe_unused]] const Instant& anInstant) -> VectorXd
    {
        const Vector5d& coeVector = aState.accessCoordinates();

        VectorXd coordinates(1);
        coordinates << this->computeQ(coeVector, aThrustAcceleration);

        return coordinates;
    };

    const MatrixXd jacobian = finiteDifferenceSolver_.computeJacobian(
        stateBuilder_.build(Instant::J2000(), currentCOEVector.segment(0, 5)), Instant::J2000(), getQ, 1
    );

    const MatrixXd D = jacobian * derivativeMatrix;

    Vector3d thrustDirection = {
        -D.col(0).sum(),
        -D.col(1).sum(),
        -D.col(2).sum(),
    };

    thrustDirection.normalize();

    return thrustDirection;
}

Real QLaw::computeQ(const Vector5d& currentCOEVector, const Real& aThrustAcceleration) const
{
    const Vector5d deltaCOE = {
        (currentCOEVector[0] - targetCOEVector_[0]),
        (currentCOEVector[1] - targetCOEVector_[1]),
        (currentCOEVector[2] - targetCOEVector_[2]),
        (std::acos(std::cos((currentCOEVector[3] - targetCOEVector_[3])))),
        (std::acos(std::cos((currentCOEVector[4] - targetCOEVector_[4])))),
    };

    const Vector5d scalingCOE = {
        std::pow(
            (1.0 + std::pow(deltaCOE[0] / (parameters_.m * targetCOEVector_[0]), parameters_.n)), 1.0 / parameters_.r
        ),
        1.0,
        1.0,
        1.0,
        1.0,
    };

    const Vector5d maximalCOE = computeOrbitalElementsMaximalChange(currentCOEVector, aThrustAcceleration);

    // TBI: Include Penalty function for minimum periapsis

    const Vector5d deltaCOE_divided_maximalCOE = (deltaCOE.cwiseQuotient(maximalCOE));

    return (parameters_.getControlWeights()
                .cwiseProduct(scalingCOE)
                .cwiseProduct(deltaCOE_divided_maximalCOE.cwiseProduct(deltaCOE_divided_maximalCOE)))
        .sum();
}

Vector5d QLaw::computeOrbitalElementsMaximalChange(const Vector5d& aCOEVector, const Real& aThrustAcceleration) const
{
    const Real& semiMajorAxis = aCOEVector[0];
    const Real& eccentricity = aCOEVector[1];
    const Real& inclination = aCOEVector[2];
    const Real& argumentOfPeriapsis = aCOEVector[4];

    const Real semiLatusRectum = COE::ComputeSemiLatusRectum(semiMajorAxis, eccentricity);
    const Real angularMomentum = COE::ComputeAngularMomentum(semiLatusRectum, gravitationalParameter_);

    // common grouped terms
    const Real eccentricitySquared = eccentricity * eccentricity;
    const Real aop_sin = std::sin(argumentOfPeriapsis);
    const Real aop_cos = std::cos(argumentOfPeriapsis);

    // Semi-Major Axis
    const Real semiMajorAxis_xx =
        (2.0 * aThrustAcceleration *
         std::sqrt(std::pow(semiMajorAxis, 3) * (1.0 + eccentricity) / (mu_ * (1.0 - eccentricity))));

    // Eccentricity
    const Real eccentricity_xx = 2.0 * semiLatusRectum * aThrustAcceleration / angularMomentum;

    // Inclination
    const Real inclination_xx = (semiLatusRectum * aThrustAcceleration) /
                                    (angularMomentum * std::sqrt(1.0 - (eccentricitySquared * aop_sin * aop_sin))) -
                                eccentricity * std::abs(aop_cos);

    // Right Ascension of the Ascending Node
    const Real rightAscensionOfAscendingNode_xx =
        (semiLatusRectum * aThrustAcceleration) /
        (angularMomentum * std::sin(inclination) *
         ((1.0 - (eccentricitySquared * aop_cos * aop_cos)) - eccentricity * std::abs(aop_sin)));

    // Argument of Periapsis
    const Real alpha = (1.0 - eccentricitySquared) / (2.0 * std::pow(eccentricity, 3));
    const Real beta = std::sqrt(alpha * alpha + 1.0 / 27.0);

    const Real cosTheta_xx =
        std::pow((alpha + beta), 1.0 / 3.0) - std::pow((beta - alpha), 1.0 / 3.0) - 1.0 / eccentricity;
    const Real r_xx = semiLatusRectum / (1.0 + (eccentricity * cosTheta_xx));

    const Real cosTheta_xxSquared = cosTheta_xx * cosTheta_xx;

    const Real argumentOfPeriapsisI_xx = (aThrustAcceleration / (eccentricity * angularMomentum)) *
                                         std::sqrt(
                                             semiLatusRectum * semiLatusRectum * cosTheta_xxSquared +
                                             std::pow((semiLatusRectum + r_xx), 2) * (1.0 - cosTheta_xxSquared)
                                         );
    const Real argumentOfPeriapsisO_xx = rightAscensionOfAscendingNode_xx * std::abs(std::cos(inclination));
    const Real argumentOfPeriapsis_xx =
        (argumentOfPeriapsisI_xx + parameters_.b * argumentOfPeriapsisO_xx) / (1.0 + parameters_.b);

    return {
        semiMajorAxis_xx,
        eccentricity_xx,
        inclination_xx,
        rightAscensionOfAscendingNode_xx,
        argumentOfPeriapsis_xx,
    };
}

Matrix3d QLaw::thetaRHToGCRF(const Vector3d& aPositionCoordinates, const Vector3d& aVelocityCoordinates)
{
    const Vector3d R = aPositionCoordinates.normalized();
    const Vector3d H = aPositionCoordinates.cross(aVelocityCoordinates).normalized();
    const Vector3d theta = H.cross(R);

    Matrix3d rotationMatrix;
    rotationMatrix.col(0) = theta;
    rotationMatrix.col(1) = R;
    rotationMatrix.col(2) = H;

    return rotationMatrix;
}

Matrix53d QLaw::computeDOEWithF(const Vector6d& aCOEVector) const
{
    const Real& semiMajorAxis = aCOEVector[0];
    const Real& eccentricity = aCOEVector[1];
    const Real& inclination = aCOEVector[2];
    const Real& argumentOfPeriapsis = aCOEVector[4];
    const Real& trueAnomaly = aCOEVector[5];

    const Real semiLatusRectum = COE::ComputeSemiLatusRectum(semiMajorAxis, eccentricity);
    const Real angularMomentum = COE::ComputeAngularMomentum(semiLatusRectum, gravitationalParameter_);
    const Real radialDistance = COE::ComputeRadialDistance(semiMajorAxis, eccentricity, trueAnomaly);

    // columns: Orbital elements
    // rows: theta, radial, angular momentum directions
    Matrix53d derivativeMatrix = Matrix53d::Zero();

    // Common grouped operations
    const Real trueAnomaly_sin = std::sin(trueAnomaly);
    const Real trueAnomaly_cos = std::cos(trueAnomaly);
    const Real trueAnomaly_ArgumentOfPeriapsis_sin = std::sin(trueAnomaly + argumentOfPeriapsis);
    const Real trueAnomaly_ArgumentOfPeriapsis_cos = std::cos(trueAnomaly + argumentOfPeriapsis);

    // Semi-major axis
    const Real sma_alpha = (2.0 * semiMajorAxis * semiMajorAxis / angularMomentum);
    derivativeMatrix(0, 0) = sma_alpha * semiLatusRectum / radialDistance;
    derivativeMatrix(0, 1) = sma_alpha * eccentricity * trueAnomaly_sin;

    // Eccentricity
    derivativeMatrix(1, 0) =
        ((semiLatusRectum + radialDistance) * trueAnomaly_cos + (radialDistance * eccentricity)) / angularMomentum;
    derivativeMatrix(1, 1) = (semiLatusRectum * trueAnomaly_sin / angularMomentum);

    // Inclination
    derivativeMatrix(2, 2) = (radialDistance * trueAnomaly_ArgumentOfPeriapsis_cos / angularMomentum);

    // Right Ascension of the Ascending Node
    derivativeMatrix(3, 2) =
        (radialDistance * trueAnomaly_ArgumentOfPeriapsis_sin) / (angularMomentum * std::sin(inclination));

    // Argument of Periapsis
    const Real aop_alpha = 1.0 / (eccentricity * angularMomentum);
    derivativeMatrix(4, 0) = (semiLatusRectum + radialDistance) * trueAnomaly_sin * aop_alpha;
    derivativeMatrix(4, 1) = -semiLatusRectum * trueAnomaly_cos * aop_alpha;
    derivativeMatrix(4, 2) = (-radialDistance * trueAnomaly_ArgumentOfPeriapsis_sin * std::cos(inclination)) /
                             (angularMomentum * std::sin(inclination));

    return derivativeMatrix;
}

}  // namespace guidancelaw
}  // namespace astro
}  // namespace ostk
