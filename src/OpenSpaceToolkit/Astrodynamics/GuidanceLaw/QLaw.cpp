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

using ostk::core::types::Index;

using ostk::math::object::Vector6d;
using ostk::math::object::VectorXd;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesSubset;

QLaw::Parameters::Parameters(
    const Map<COE::Element, Tuple<double, double>>& anElementWeightsMap,
    const Size& aMValue,
    const Size& aNValue,
    const Size& aRValue,
    const double& aBValue,
    const Size& aKValue,
    const double& aPeriapsisWeight,
    const Length& minimumPeriapsisradius
)
    : m(aMValue),
      n(aNValue),
      r(aRValue),
      b(aBValue),
      k(aKValue),
      periapsisWeight(aPeriapsisWeight),
      minimumPeriapsisRadius_(minimumPeriapsisradius.inMeters()),
      convergenceThresholds_(Vector5d::Zero()),
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
    const Shared<const Frame>& outputFrameSPtr
) const
{
    const COE coe = COE::Cartesian(
        {Position::Meters(aPositionCoordinates, outputFrameSPtr),
         Velocity::MetersPerSecond(aVelocityCoordinates, outputFrameSPtr)},
        gravitationalParameter_
    );

    Vector6d coeVector = coe.getSIVector(COE::AnomalyType::True);

    coeVector[1] = std::max(coeVector[1], 1e-4);
    coeVector[2] = std::max(coeVector[2], 1e-4);

    const Vector3d thrustDirection = computeThrustDirection(coeVector, aThrustAcceleration);

    const Matrix3d R_thetaRH_GCRF = QLaw::ThetaRHToGCRF(aPositionCoordinates, aVelocityCoordinates);

    return aThrustAcceleration * R_thetaRH_GCRF * thrustDirection;
}

Vector5d QLaw::compute_dQ_dOE(const Vector5d& aCOEVector, const double& aThrustAcceleration) const
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
            .computeJacobian(stateBuilder_.build(Instant::J2000(), aCOEVector), Instant::J2000(), getQ, 1)
            .data(),
        5
    );

    return jacobian;
}

Vector3d QLaw::computeThrustDirection(const Vector6d& aCOEVector, const double& aThrustAcceleration) const
{
    const Matrix53d derivativeMatrix = QLaw::Compute_dOE_dF(aCOEVector, gravitationalParameter_);

    const Vector5d jacobian = compute_dQ_dOE(aCOEVector.segment(0, 5), aThrustAcceleration);

    const Vector3d D = -(jacobian.transpose() * derivativeMatrix).normalized();

    return D;
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
    Vector5d deltaCOE = {
        (aCOEVector[0] - targetCOEVector_[0]),
        (aCOEVector[1] - targetCOEVector_[1]),
        (aCOEVector[2] - targetCOEVector_[2]),
        (std::acos(std::cos((aCOEVector[3] - targetCOEVector_[3])))),
        (std::acos(std::cos((aCOEVector[4] - targetCOEVector_[4])))),
    };

    for (Index i = 0; i < 5; ++i)
    {
        if (std::abs(deltaCOE[i]) < parameters_.convergenceThresholds_[i])
        {
            deltaCOE[i] = 0.0;
        }
    }

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

}  // namespace guidancelaw
}  // namespace astro
}  // namespace ostk
