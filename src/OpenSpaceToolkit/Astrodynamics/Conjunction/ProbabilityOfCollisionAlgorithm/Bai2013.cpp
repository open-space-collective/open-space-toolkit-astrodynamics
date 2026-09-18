/// Apache License 2.0

#include <cmath>
#include <optional>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Bai2013.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace probabilityofcollisionalgorithm
{

using ostk::core::container::Tuple;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;
using ostk::mathematics::object::Vector3d;

using FrameManager = ostk::physics::coordinate::frame::Manager;
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;

using ostk::astrodynamics::estimator::CovarianceMatrix;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;

const Shared<const CoordinateSubset> Bai2013::CartesianPositionSubsetSPtr = CartesianPosition::Default();

Bai2013::Bai2013(
    const Derived& aRelativeVelocityThreshold,
    const Real& anEccentricityThreshold,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter
)
    : ProbabilityOfCollisionAlgorithm("Bai 2013"),
      relativeVelocityThreshold_(aRelativeVelocityThreshold),
      eccentricityThreshold_(anEccentricityThreshold),
      frameSPtr_(aFrameSPtr),
      gravitationalParameter_(aGravitationalParameter)
{
    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!aFrameSPtr->isQuasiInertial())
    {
        throw ostk::core::error::runtime::Wrong("Frame", aFrameSPtr->getName());
    }
}

Array<String> Bai2013::identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const
{
    Array<String> unsatisfiedAssumptions = Array<String>::Empty();

    const std::optional<CovarianceMatrix> object1CovarianceMatrix = aCloseApproach.getObject1CovarianceMatrix();
    if (!object1CovarianceMatrix.has_value() ||
        !object1CovarianceMatrix->getCoordinateSubsets().contains(Bai2013::CartesianPositionSubsetSPtr))
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 1");
    }

    const std::optional<CovarianceMatrix> object2CovarianceMatrix = aCloseApproach.getObject2CovarianceMatrix();
    if (!object2CovarianceMatrix.has_value() ||
        !object2CovarianceMatrix->getCoordinateSubsets().contains(Bai2013::CartesianPositionSubsetSPtr))
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 2");
    }

    const Real relativeVelocityMetersPerSecond =
        aCloseApproach.getRelativeVelocity().in(Derived::Unit::MeterPerSecond());
    const Real relativeVelocityThresholdMetersPerSecond =
        relativeVelocityThreshold_.in(Derived::Unit::MeterPerSecond());

    if (relativeVelocityMetersPerSecond < relativeVelocityThresholdMetersPerSecond)
    {
        unsatisfiedAssumptions.add(String::Format(
            "Relative velocity [{} m/s] is below the threshold [{} m/s]",
            relativeVelocityMetersPerSecond.toString(),
            relativeVelocityThresholdMetersPerSecond.toString()
        ));
    }

    // TODO: check alignment of principal axes

    return unsatisfiedAssumptions;
}

Real Bai2013::computeProbabilityOfCollision(
    const CloseApproach& aCloseApproach,
    const Shared<const HardBody>& aHardBody1SPtr,
    const Shared<const HardBody>& aHardBody2SPtr
) const
{
    const State object1State = aCloseApproach.getObject1State();
    const State object2State = aCloseApproach.getObject2State();

    // Compute the combined hard-body radius (enclosing radius of the combined cross section in the encounter plane)
    const Shared<const Frame> encounterFrameSPtr = aCloseApproach.getEncounterFrame(frameSPtr_);

    const HardBody::CrossSection combinedCrossSection = HardBody::CrossSection::combine(
        aHardBody1SPtr->calculateCrossSectionAt(object1State, encounterFrameSPtr),
        aHardBody2SPtr->calculateCrossSectionAt(object2State, encounterFrameSPtr)
    );

    const Real combinedHardBodyRadius = combinedCrossSection.getEnclosingRadius();

    if ((this->computeEccentricity(object1State) < eccentricityThreshold_) &&
        (this->computeEccentricity(object2State) < eccentricityThreshold_))
    {
        return this->computeExplicitProbabilityOfCollision(aCloseApproach, combinedHardBodyRadius);
    }

    return this->computeModifiedProbabilityOfCollision(aCloseApproach, combinedHardBodyRadius);
}

Real Bai2013::computeExplicitProbabilityOfCollision(
    const CloseApproach& aCloseApproach, const Real& aCombinedHardBodyRadius
) const
{
    const State object1State = aCloseApproach.getObject1State();
    const State object2State = aCloseApproach.getObject2State();

    // RSW frames (equivalent to QSW frames)
    const Shared<const LocalOrbitalFrameFactory> qswFrameFactorySPtr = LocalOrbitalFrameFactory::QSW(frameSPtr_);

    const Shared<const Frame> object1QSWFrameSPtr = qswFrameFactorySPtr->generateFrame(object1State);
    const Shared<const Frame> object2QSWFrameSPtr = qswFrameFactorySPtr->generateFrame(object2State);

    // Compute miss distance components (r, s, w)
    const Tuple<Length, Length, Length> missDistanceComponents =
        aCloseApproach.computeMissDistanceComponentsInFrame(object1QSWFrameSPtr);

    const Real r = std::get<0>(missDistanceComponents).inMeters();
    const Real s = std::get<1>(missDistanceComponents).inMeters();
    const Real w = std::get<2>(missDistanceComponents).inMeters();

    // Compute angle between inertial velocity vectors (phi)
    const Real phi =
        Angle::Between(
            object1State.inFrame(frameSPtr_).getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates(),
            object2State.inFrame(frameSPtr_).getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates()
        )
            .inRadians();

    // Compute RSW position uncertainty matrices
    const CovarianceMatrix object1RSWPositionUncertainty = object1State.accessCovarianceMatrix()
                                                               .reduce({Bai2013::CartesianPositionSubsetSPtr})
                                                               .rotate(object1QSWFrameSPtr);
    const CovarianceMatrix object2RSWPositionUncertainty = object2State.accessCovarianceMatrix()
                                                               .reduce({Bai2013::CartesianPositionSubsetSPtr})
                                                               .rotate(object2QSWFrameSPtr);

    const Real sigma_r_squared = object1RSWPositionUncertainty.accessCoordinates()(0, 0) +
                                 object2RSWPositionUncertainty.accessCoordinates()(0, 0);
    const Real sigma_s_squared = object1RSWPositionUncertainty.accessCoordinates()(1, 1) +
                                 object2RSWPositionUncertainty.accessCoordinates()(1, 1);
    const Real sigma_w_squared = object1RSWPositionUncertainty.accessCoordinates()(2, 2) +
                                 object2RSWPositionUncertainty.accessCoordinates()(2, 2);

    const Real sigma_sw_squared =
        sigma_s_squared * std::pow(std::cos(0.5 * phi), 2) + sigma_w_squared * std::pow(std::sin(0.5 * phi), 2);

    // Compute the probability of collision
    const Real ra = aCombinedHardBodyRadius;
    const Real sigma_r = std::sqrt(sigma_r_squared);
    const Real sigma_sw = std::sqrt(sigma_sw_squared);

    return std::exp(-0.5 * (r * r / sigma_r_squared + (s * s + w * w) / sigma_sw_squared)) *
           (1.0 - std::exp(-ra * ra / (2.0 * sigma_r * sigma_sw)));
}

Real Bai2013::computeModifiedProbabilityOfCollision(
    const CloseApproach& aCloseApproach, const Real& aCombinedHardBodyRadius
) const
{
    const State object1State = aCloseApproach.getObject1State();
    const State object2State = aCloseApproach.getObject2State();

    // NTW frames
    const Shared<const Frame> object1NTWFrameSPtr = this->generateNTWFrame(object1State);
    const Shared<const Frame> object2NTWFrameSPtr = this->generateNTWFrame(object2State);

    // Compute miss distance components in the primary NTW frame (n, t, w) - Eq. (33)
    const Vector3d missDistanceComponents =
        object2State.inFrame(object1NTWFrameSPtr).getPosition().inMeters().getCoordinates();

    const Real n = missDistanceComponents(0);
    const Real t = missDistanceComponents(1);
    const Real w = missDistanceComponents(2);

    // Compute the conjunction geometry: flight-path angles (theta1, theta2), ratio of velocity magnitudes (k) and
    // angle between orbital planes (phi)
    const State object1StateInFrame = object1State.inFrame(frameSPtr_);
    const State object2StateInFrame = object2State.inFrame(frameSPtr_);

    const Vector3d r1 = object1StateInFrame.getPosition().inMeters().getCoordinates();
    const Vector3d v1 = object1StateInFrame.getVelocity().inUnit(Velocity::Unit::MeterPerSecond).getCoordinates();
    const Vector3d r2 = object2StateInFrame.getPosition().inMeters().getCoordinates();
    const Vector3d v2 = object2StateInFrame.getVelocity().inUnit(Velocity::Unit::MeterPerSecond).getCoordinates();

    const Real theta1 = std::asin(r1.normalized().dot(v1.normalized()));
    const Real theta2 = std::asin(r2.normalized().dot(v2.normalized()));

    const Real k = v2.norm() / v1.norm();

    // Eqs. (39) to (42) implicitly treat phi (and hence psi) as a signed rotation about the encounter x-axis, which
    // points to the primary N-axis side of the common perpendicular to both velocities (see Fig. 9). The paper only
    // illustrates the positive case: without this sign, Case B of the paper cannot be reproduced.
    const Vector3d object1NormalDirection = v1.normalized().cross(r1.cross(v1).normalized());
    const Real phiSign = (v1.cross(v2).dot(object1NormalDirection) < 0.0) ? -1.0 : 1.0;

    const Real phi = phiSign * Angle::Between(r1.cross(v1), r2.cross(v2)).inRadians();

    // Compute angle between velocity vectors (psi) - Eq. (39)
    const Real psi =
        phiSign * std::acos(std::sin(theta1) * std::sin(theta2) + std::cos(theta1) * std::cos(theta2) * std::cos(phi));

    // Compute rotating angles from the modified NTW frames to the encounter frame (psi1, psi2) - Eq. (40)
    const Real psi1 = std::atan((1.0 - k * std::cos(psi)) / (k * std::sin(psi)));
    const Real psi2 = std::atan((k - std::cos(psi)) / std::sin(psi));

    // Compute rotating angles from the NTW frames to the modified NTW frames (phi1, phi2) - Eq. (41)
    const Real phi1 = std::atan(
        (std::cos(theta1) * std::sin(theta2) - std::sin(theta1) * std::cos(theta2) * std::cos(phi)) /
        (std::cos(theta2) * std::sin(phi))
    );
    const Real phi2 = std::atan(
        (std::cos(theta2) * std::sin(theta1) - std::sin(theta2) * std::cos(theta1) * std::cos(phi)) /
        (std::cos(theta1) * std::sin(phi))
    );

    // Compute NTW position uncertainty matrices - Eq. (35)
    const CovarianceMatrix object1NTWPositionUncertainty = object1State.accessCovarianceMatrix()
                                                               .reduce({Bai2013::CartesianPositionSubsetSPtr})
                                                               .rotate(object1NTWFrameSPtr);
    const CovarianceMatrix object2NTWPositionUncertainty = object2State.accessCovarianceMatrix()
                                                               .reduce({Bai2013::CartesianPositionSubsetSPtr})
                                                               .rotate(object2NTWFrameSPtr);

    const Real sigma_1n_squared = object1NTWPositionUncertainty.accessCoordinates()(0, 0);
    const Real sigma_1t_squared = object1NTWPositionUncertainty.accessCoordinates()(1, 1);
    const Real sigma_1w_squared = object1NTWPositionUncertainty.accessCoordinates()(2, 2);
    const Real sigma_2n_squared = object2NTWPositionUncertainty.accessCoordinates()(0, 0);
    const Real sigma_2t_squared = object2NTWPositionUncertainty.accessCoordinates()(1, 1);
    const Real sigma_2w_squared = object2NTWPositionUncertainty.accessCoordinates()(2, 2);

    // Compute the probability integral parameters in the encounter plane, ignoring the off-diagonal terms of the
    // combined covariance - Eqs. (34), (37) and (38)
    const Real mu_x = n * std::cos(phi1) - w * std::sin(phi1);
    const Real mu_y = n * std::sin(psi1) * std::sin(phi1) + t * std::cos(psi1) + w * std::sin(psi1) * std::cos(phi1);

    const Real sigma_x_squared =
        sigma_1n_squared * std::pow(std::cos(phi1), 2) + sigma_2n_squared * std::pow(std::cos(phi2), 2) +
        sigma_1w_squared * std::pow(std::sin(phi1), 2) + sigma_2w_squared * std::pow(std::sin(phi2), 2);
    const Real sigma_y_squared =
        (sigma_1n_squared * std::pow(std::sin(phi1), 2) + sigma_1w_squared * std::pow(std::cos(phi1), 2)) *
            std::pow(std::sin(psi1), 2) +
        sigma_1t_squared * std::pow(std::cos(psi1), 2) +
        (sigma_2n_squared * std::pow(std::sin(phi2), 2) + sigma_2w_squared * std::pow(std::cos(phi2), 2)) *
            std::pow(std::sin(psi2), 2) +
        sigma_2t_squared * std::pow(std::cos(psi2), 2);

    // Compute the probability of collision - Eq. (38)
    const Real ra = aCombinedHardBodyRadius;
    const Real sigma_x = std::sqrt(sigma_x_squared);
    const Real sigma_y = std::sqrt(sigma_y_squared);

    return std::exp(-0.5 * (mu_x * mu_x / sigma_x_squared + mu_y * mu_y / sigma_y_squared)) *
           (1.0 - std::exp(-ra * ra / (2.0 * sigma_x * sigma_y)));
}

Shared<const Frame> Bai2013::generateNTWFrame(const State& aState) const
{
    const State stateInFrame = aState.inFrame(frameSPtr_);

    const Instant& instant = stateInFrame.accessInstant();
    const Vector3d position = stateInFrame.getPosition().inMeters().getCoordinates();
    const Vector3d velocity = stateInFrame.getVelocity().inUnit(Velocity::Unit::MeterPerSecond).getCoordinates();

    const String name = "NTW@" + instant.toString() + position.toString() + velocity.toString();

    if (const Shared<const Frame> frameSPtr = FrameManager::Get().accessFrameWithName(name))
    {
        return frameSPtr;
    }

    const Vector3d tAxis = velocity.normalized();
    const Vector3d wAxis = position.cross(velocity).normalized();
    const Vector3d nAxis = tAxis.cross(wAxis);

    const Transform transform = {
        instant,
        -position,
        -velocity,
        Quaternion::RotationMatrix(RotationMatrix::Rows(nAxis, tAxis, wAxis)).toNormalized().rectify(),
        {0.0, 0.0, 0.0},
        Transform::Type::Passive
    };

    return Frame::Construct(
        name, false, frameSPtr_, std::make_shared<const LocalOrbitalFrameTransformProvider>(transform)
    );
}

Real Bai2013::computeEccentricity(const State& aState) const
{
    const State stateInFrame = aState.inFrame(frameSPtr_);

    const COE coe = COE::Cartesian({stateInFrame.getPosition(), stateInFrame.getVelocity()}, gravitationalParameter_);

    return coe.getEccentricity();
}

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
