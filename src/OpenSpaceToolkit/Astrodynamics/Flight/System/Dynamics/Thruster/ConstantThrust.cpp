/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

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

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::trajectory::LocalOrbitalFrameFactory;
using ostk::astro::trajectory::LocalOrbitalFrameDirection;

ConstantThrust::ConstantThrust(
    const SatelliteSystem& aSatelliteSystem, const LocalOrbitalFrameDirection& aThrustDirection, const String& aName
)
    : Thruster(aSatelliteSystem, aName),
      localOrbitalFrameDirection_(aThrustDirection)
{
}

ConstantThrust::~ConstantThrust() {}

std::ostream& operator<<(std::ostream& anOutputStream, const ConstantThrust& aConstantThrust)
{
    aConstantThrust.print(anOutputStream);

    return anOutputStream;
}

bool ConstantThrust::isDefined() const
{
    return localOrbitalFrameDirection_.isDefined();
}

Scalar ConstantThrust::getThrust() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Constant Thrust Thruster");
    }

    return this->getSatelliteSystem().getPropulsionSystem().getThrust();
}

Array<Shared<const CoordinatesSubset>> ConstantThrust::getReadCoordinatesSubsets() const
{
    return {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
    };
}

Array<Shared<const CoordinatesSubset>> ConstantThrust::getWriteCoordinatesSubsets() const
{
    return {
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
    };
}

VectorXd ConstantThrust::computeContribution(
    const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
) const
{
    const Vector3d positionCoordinates = {x[0], x[1], x[2]};
    const Vector3d velocityCoordinates = {x[3], x[4], x[5]};

    // Get Rotation Matrix from Direction Local Orbital Frame (LOF) to Requested Frame
    Shared<const Frame> localOrbitalFrameSPtr =
        this->localOrbitalFrameDirection_.accessLocalOrbitalFrameFactory()->generateFrame(
            anInstant, positionCoordinates, velocityCoordinates
        );  // TBI: Assumes x is given in GCRF (which also must be the parentFrame for the LOFFactory definition)
    Quaternion q_requestedFrame_LOF =
        localOrbitalFrameSPtr->getTransformTo(aFrameSPtr, anInstant).getOrientation().normalize();

    const SatelliteSystem satelliteSystem = this->getSatelliteSystem();

    if (x[6] <= satelliteSystem.getMass().getValue())  // We compare against the dry mass of the Satellite
    {
        throw ostk::core::error::RuntimeError("Out of fuel.");
    }

    const PropulsionSystem propulsionSystem = satelliteSystem.getPropulsionSystem();

    const Vector3d acceleration_LOF =
        propulsionSystem.getAcceleration(Mass::Kilograms(x[6])).getValue() *
        this->localOrbitalFrameDirection_.getValue(
        );  // TBI: Can be optimized to not convert to Mass and back by doing the calculation inline here in SI units

    const Vector3d acceleration_requestedFrame = q_requestedFrame_LOF.rotateVector(acceleration_LOF);

    // Compute contribution
    VectorXd contribution(4);
    contribution << acceleration_requestedFrame[0], acceleration_requestedFrame[1], acceleration_requestedFrame[2],
        -propulsionSystem.getMassFlowRate().getValue();  // TBI: Can be optimized to cache the SI value as a Real

    return contribution;
}

void ConstantThrust::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "ConstantThrust Thruster Dynamics") : void();

    Dynamics::print(anOutputStream, false);

    ostk::core::utils::Print::Separator(anOutputStream, "Satellite System");
    this->getSatelliteSystem().print(anOutputStream, false);

    ostk::core::utils::Print::Separator(anOutputStream, "Thrust Direction");
    localOrbitalFrameDirection_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace thruster
}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
