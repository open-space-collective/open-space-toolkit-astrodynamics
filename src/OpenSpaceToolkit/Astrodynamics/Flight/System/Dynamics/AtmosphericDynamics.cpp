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

using ostk::core::types::Real;

using ostk::physics::Unit;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;
using ostk::physics::coord::Position;
using ostk::physics::data::Scalar;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

AtmosphericDynamics::AtmosphericDynamics(const Shared<const Celestial>& aCelestial, const SatelliteSystem& aSatelliteSystem)
    : Dynamics(),
      celestialObjectSPtr_(aCelestial),
      satelliteSystem_(aSatelliteSystem)
{
    if (!celestialObjectSPtr_ || !celestialObjectSPtr_->atmosphericModelIsDefined())
    {
        throw ostk::core::error::runtime::Undefined("Atmospheric Model");
    }
}

AtmosphericDynamics::~AtmosphericDynamics() {}

AtmosphericDynamics* AtmosphericDynamics::clone() const
{
    return new AtmosphericDynamics(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const AtmosphericDynamics& anAtmosphericDynamics)
{
    anAtmosphericDynamics.print(anOutputStream);

    return anOutputStream;
}

bool AtmosphericDynamics::isDefined() const
{
    return celestialObjectSPtr_->isDefined() && satelliteSystem_.isDefined();
}

void AtmosphericDynamics::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Atmospheric Dynamics") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Celestial:" << celestialObjectSPtr_;

    ostk::core::utils::Print::Line(anOutputStream) << "Satellite System:" << satelliteSystem_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

void AtmosphericDynamics::update(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant)
{
    (void)anInstant;

    // Get atmospheric density
    const Real atmosphericDensity =
        celestialObjectSPtr_->getAtmosphericDensityAt(Position::Meters({x[0], x[1], x[2]}, gcrfSPtr_), anInstant)
            .inUnit(Unit::Derived(Derived::Unit::MassDensity(Mass::Unit::Kilogram, Length::Unit::Meter)))
            .getValue();

    const Vector3d earthAngularVelocity =
        gcrfSPtr_->getTransformTo(Frame::ITRF(), anInstant).getAngularVelocity();  // rad/s

    const Vector3d relativeVelocity =
        Vector3d(x[3], x[4], x[5]) - earthAngularVelocity.cross(Vector3d(x[0], x[1], x[2]));

    const Real mass = satelliteSystem_.getMass().inKilograms();  // TBI: Add wet mass from state vector
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();

    // Add object's gravity to total gravitational acceleration
    const Vector3d dragAcceleration_SI =
        -(0.5 / mass) * dragCoefficient * surfaceArea * atmosphericDensity * relativeVelocity.norm() * relativeVelocity;

    // Integrate velocity states
    dxdt[3] += dragAcceleration_SI[0];
    dxdt[4] += dragAcceleration_SI[1];
    dxdt[5] += dragAcceleration_SI[2];
}

Shared<const Celestial> AtmosphericDynamics::getCelestial() const
{
    return celestialObjectSPtr_;
}

SatelliteSystem AtmosphericDynamics::getSatelliteSystem() const
{
    if (!satelliteSystem_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Satellite System");
    }

    return satelliteSystem_;
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
