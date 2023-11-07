/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{

using ostk::math::obj::VectorXd;
using ostk::math::obj::Vector3d;

using ostk::physics::units::Mass;

using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

Thruster::Thruster(
    const SatelliteSystem& aSatelliteSystem, const Shared<const GuidanceLaw>& aGuidanceLaw, const String& aName
)
    : Dynamics(aName),
      satelliteSystem_(aSatelliteSystem),
      guidanceLaw_(aGuidanceLaw)
{
}

Thruster::~Thruster() {}

SatelliteSystem Thruster::getSatelliteSystem() const
{
    if (!satelliteSystem_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Satellite System");
    }

    return satelliteSystem_;
}

Shared<const GuidanceLaw> Thruster::getGuidanceLaw() const
{
    return guidanceLaw_;
}

Array<Shared<const CoordinatesSubset>> Thruster::getReadCoordinatesSubsets() const
{
    return {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
    };
}

Array<Shared<const CoordinatesSubset>> Thruster::getWriteCoordinatesSubsets() const
{
    return {
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
    };
}

bool Thruster::isDefined() const
{
    return satelliteSystem_.isDefined();
}

VectorXd Thruster::computeContribution(
    const Instant& anInstant, const VectorXd& x, [[maybe_unused]] const Shared<const Frame>& aFrameSPtr
) const
{
    const Vector3d positionCoordinates = {x[0], x[1], x[2]};
    const Vector3d velocityCoordinates = {x[3], x[4], x[5]};

    if (x[6] <= satelliteSystem_.getMass().inKilograms())  // We compare against the dry mass of the Satellite
    {
        throw ostk::core::error::RuntimeError("Out of fuel.");
    }

    const Real maximumThrustAccelerationMagnitude =
        satelliteSystem_.accessPropulsionSystem().getAcceleration(Mass::Kilograms(x[6])).getValue();

    const Vector3d acceleration = guidanceLaw_->calculateThrustAccelerationAt(
        anInstant, positionCoordinates, velocityCoordinates, maximumThrustAccelerationMagnitude, aFrameSPtr
    );

    // Compute contribution
    VectorXd contribution(4);
    // TBI: Can be optimized to cache the SI value of mass flow rate as a Real
    contribution << acceleration[0], acceleration[1], acceleration[2],
        -satelliteSystem_.accessPropulsionSystem().getMassFlowRate().getValue();

    return contribution;
}

void Thruster::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Thruster") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;
    ostk::core::utils::Print::Line(anOutputStream) << "Satellite System:" << satelliteSystem_;
    ostk::core::utils::Print::Line(anOutputStream) << "Guidance Law:" << guidanceLaw_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace astro
}  // namespace ostk
