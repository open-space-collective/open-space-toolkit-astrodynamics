/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/GravitationalDynamics.hpp>

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

using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

GravitationalDynamics::GravitationalDynamics(const Celestial& aCelestial)
    : Dynamics(),
      celestial_(aCelestial)
{
}

GravitationalDynamics::GravitationalDynamics(const GravitationalDynamics& aGravitationalDynamics)
    : Dynamics(aGravitationalDynamics),
      celestial_(aGravitationalDynamics.celestial_)
{
}

GravitationalDynamics::~GravitationalDynamics() {}

GravitationalDynamics* GravitationalDynamics::clone() const
{
    return new GravitationalDynamics(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const GravitationalDynamics& aGravitationalDynamics)
{
    aGravitationalDynamics.print(anOutputStream);

    return anOutputStream;
}

bool GravitationalDynamics::isDefined() const
{
    return celestial_.isDefined();
}

void GravitationalDynamics::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Gravitational Dynamics") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Celestial:" << celestial_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

void GravitationalDynamics::update(
    const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant
)
{
    // Initialize gravitational acceleration vector
    Vector3d totalGravitationalAcceleration_SI = {0.0, 0.0, 0.0};

    if (celestial_.accessName() != "Earth")
    {
        // Obtain 3rd body effect on center of Earth (origin in GCRF) aka 3rd body correction
        const Vector gravitationalAcceleration3rdBodyCorrection =
            celestial_.getGravitationalFieldAt(Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_));

        // Subtract 3rd body correct from total gravitational acceleration
        totalGravitationalAcceleration_SI -=
            gravitationalAcceleration3rdBodyCorrection.inFrame(gcrfSPtr_, anInstant).getValue();
    }

    // Obtain gravitational acceleration from current object
    const Vector gravitationalAcceleration =
        celestial_.getGravitationalFieldAt(Position::Meters({x[0], x[1], x[2]}, gcrfSPtr_));

    // Add object's gravity to total gravitational acceleration
    totalGravitationalAcceleration_SI += gravitationalAcceleration.inFrame(gcrfSPtr_, anInstant).getValue();

    // TBI: Maybe we need a way to set the index in the state vector to generalize this

    // Integrate position and velocity states
    dxdt[0] = x[3];
    dxdt[1] = x[4];
    dxdt[2] = x[5];
    dxdt[3] = totalGravitationalAcceleration_SI[0];
    dxdt[4] = totalGravitationalAcceleration_SI[1];
    dxdt[5] = totalGravitationalAcceleration_SI[2];
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
