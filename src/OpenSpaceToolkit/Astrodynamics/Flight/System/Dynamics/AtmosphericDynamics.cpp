/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDynamics.hpp>

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

using ostk::physics::Unit;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;
using ostk::physics::data::Scalar;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

AtmosphericDynamics::AtmosphericDynamics(const Celestial& aCelestial, const SatelliteSystem& aSatelliteSystem)
    : Dynamics(),
      celestial_(aCelestial),
      satelliteSystem_(aSatelliteSystem)
{
}

AtmosphericDynamics::AtmosphericDynamics(const AtmosphericDynamics& aAtmosphericDynamics)
    : Dynamics(aAtmosphericDynamics),
      celestial_(aAtmosphericDynamics.celestial_),
      satelliteSystem_(aAtmosphericDynamics.satelliteSystem_)
{
}

AtmosphericDynamics::~AtmosphericDynamics() {}

AtmosphericDynamics* AtmosphericDynamics::clone() const
{
    return new AtmosphericDynamics(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const AtmosphericDynamics& aAtmosphericDynamics)
{
    aAtmosphericDynamics.print(anOutputStream);

    return anOutputStream;
}

bool AtmosphericDynamics::isDefined() const
{
    return celestial_.isDefined();
}

void AtmosphericDynamics::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Atmospheric Dynamics") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Celestial:" << celestial_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

void AtmosphericDynamics::update(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant)
{
    (void)anInstant;
    // Get atmospheric density
    const Real atmosphericDensity =
        celestial_.getAtmosphericDensityAt(Position::Meters({x[0], x[1], x[2]}, gcrfSPtr_))
            .inUnit(Unit::Derived(Derived::Unit::MassDensity(Mass::Unit::Kilogram, Length::Unit::Meter)))
            .getValue();

    const Vector3d velocity = {x[3], x[4], x[5]};
    const Real velocityMagnitude = velocity.squaredNorm();

    // Add object's gravity to total gravitational acceleration
    const Real dragAccelerationMagnitude = 0.5 * atmosphericDensity * satelliteSystem_.getCrossSectionalSurfaceArea() *
                                           satelliteSystem_.getDragCoefficient() * velocityMagnitude *
                                           velocityMagnitude / satelliteSystem_.getMass().inKilograms();

    const Vector3d dragAcceleration_SI = (velocity / velocityMagnitude) * dragAccelerationMagnitude;

    // TBI: Maybe we need a way to set the index in the state vector to generalize this

    // Integrate position and velocity states
    dxdt[3] += dragAcceleration_SI[0];
    dxdt[4] += dragAcceleration_SI[1];
    dxdt[5] += dragAcceleration_SI[2];
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
