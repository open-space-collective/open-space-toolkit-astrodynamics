/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::mathematics::object::VectorXd;
using ostk::mathematics::object::Vector3d;

using ostk::physics::unit::Mass;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

Thruster::Thruster(
    const SatelliteSystem& aSatelliteSystem, const Shared<const GuidanceLaw>& aGuidanceLaw, const String& aName
)
    : Dynamics(aName),
      satelliteSystem_(aSatelliteSystem),
      guidanceLaw_(aGuidanceLaw),
      massFlowRateCache_(
          aSatelliteSystem.isDefined() ? aSatelliteSystem.accessPropulsionSystem().getMassFlowRate() : Real::Undefined()
      )
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

Array<Shared<const CoordinateSubset>> Thruster::getReadCoordinateSubsets() const
{
    return {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    };
}

Array<Shared<const CoordinateSubset>> Thruster::getWriteCoordinateSubsets() const
{
    return {
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    };
}

bool Thruster::isDefined() const
{
    return satelliteSystem_.isDefined();
}

VectorXd Thruster::computeContribution(
    const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
) const
{
    const Vector3d positionCoordinates = {x[0], x[1], x[2]};
    const Vector3d velocityCoordinates = {x[3], x[4], x[5]};

    if (x[6] <= satelliteSystem_.getMass().inKilograms())  // We compare against the dry mass of the Satellite
    {
        throw ostk::core::error::RuntimeError("Out of fuel.");
    }

    const Real maximumThrustAccelerationMagnitude =
        satelliteSystem_.accessPropulsionSystem().getAcceleration(Mass::Kilograms(x[6]));

    const Vector3d acceleration = guidanceLaw_->calculateThrustAccelerationAt(
        anInstant, positionCoordinates, velocityCoordinates, maximumThrustAccelerationMagnitude, aFrameSPtr
    );

    const Real effectiveThrustFraction = acceleration.norm() / maximumThrustAccelerationMagnitude;

    // Compute contribution
    VectorXd contribution(4);
    contribution << acceleration[0], acceleration[1], acceleration[2], -effectiveThrustFraction * massFlowRateCache_;

    return contribution;
}

void Thruster::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Thruster") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;
    satelliteSystem_.print(anOutputStream, false);
    guidanceLaw_->print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk
