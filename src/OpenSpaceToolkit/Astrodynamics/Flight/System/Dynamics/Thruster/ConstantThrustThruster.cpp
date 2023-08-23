/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster/ConstantThrustThruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/Invariant.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrame.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{
namespace dynamics
{
namespace thruster
{

using ostk::math::geom::d3::trf::rot::Quaternion;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::trajectory::state::coordinatessubsets::Invariant;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::trajectory::LOFProvider;

ConstantThrustThruster::ConstantThrustThruster(const SatelliteSystem& aSatelliteSystem, const Direction& aThrustDirection, const String& aName)
    : Thruster(aSatelliteSystem, aName),
      direction_(aThrustDirection)
{
}

ConstantThrustThruster::~ConstantThrustThruster() {}

ConstantThrustThruster* ConstantThrustThruster::clone() const
{
    return new ConstantThrustThruster(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const ConstantThrustThruster& aConstantThrustThruster)
{
    aConstantThrustThruster.print(anOutputStream);

    return anOutputStream;
}

bool ConstantThrustThruster::isDefined() const
{
    return direction_.isDefined();
}

Scalar ConstantThrustThruster::getThrust() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Constant Thrust Thruster");
    }

    return this->getSatelliteSystem().getPropulsionSystem().getThrust();
}

Array<Shared<const CoordinatesSubset>> ConstantThrustThruster::getReadCoordinatesSubsets() const
{
    return {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        Invariant::Mass()
    };
}

Array<Shared<const CoordinatesSubset>> ConstantThrustThruster::getWriteCoordinatesSubsets() const
{
    return {
        CartesianVelocity::Default(),
        Invariant::Mass()
    };
}

VectorXd ConstantThrustThruster::computeContribution(
    [[maybe_unused]] const Instant& anInstant, const VectorXd& x, [[maybe_unused]] const Shared<const Frame>& aFrameSPtr
) const
{
    const Vector3d positionCoordinates = {x[0], x[1], x[2]};
    const Vector3d velocityCoordinates = {x[3], x[4], x[5]};

    const Position position_GCRF = {positionCoordinates, Position::Unit::Meter, Frame::GCRF()};
    const Velocity velocity_GCRF = {velocityCoordinates, Velocity::Unit::MeterPerSecond, Frame::GCRF()};

    // Get Rotation Matrix from Direction Local Orbital Frame (LOF) to GCRF (ECI)
    Shared<const Frame> frameSPtr = this->direction_.getFrame();
    Shared<const LOFProvider> providerSPtr = std::dynamic_pointer_cast<const LOFProvider>(frameSPtr->accessProvider());

    Quaternion q_GCRF_LOF = providerSPtr->getTransformAt(anInstant, position_GCRF, velocity_GCRF).getOrientation();

    // TBImproved
    if (x[6] <= 0.0)
    {
        throw ostk::core::error::RuntimeError("Out of fuel.");
    }

    const SatelliteSystem satelliteSystem = this->getSatelliteSystem();

    const Vector3d acceleration_LOF = satelliteSystem.getPropulsionSystem().getAcceleration(
                                          Mass::Kilograms(x[6] + satelliteSystem.getMass().inKilograms())
                                      ) *
                                      this->direction_.getValue();

    const Vector3d acceleration_GCRF = q_GCRF_LOF.rotateVector(acceleration_LOF);

    // Compute contribution
    VectorXd contribution(4);
    contribution << acceleration_GCRF[0], acceleration_GCRF[1], acceleration_GCRF[2], -satelliteSystem.getPropulsionSystem().getMassFlowRate().getValue(); // TBI: Everything is constant anyway as it comes from PropulsionSystem

    return contribution;
}

void ConstantThrustThruster::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "ConstantThrustThruster Dynamics") : void();

    // satelliteSystem_.print(anOutputStream, false);

    // TBI: Print direction

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace thruster
}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
