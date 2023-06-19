/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>

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

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::data::Vector;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

CentralBodyGravity::CentralBodyGravity(const Shared<const Celestial>& aCelestialObjectSPtr)
    : Dynamics(),
      celestialObjectSPtr_(aCelestialObjectSPtr)
{
    if (!celestialObjectSPtr_ || !celestialObjectSPtr_->gravitationalModelIsDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational Model");
    }
}

CentralBodyGravity::~CentralBodyGravity() {}

CentralBodyGravity* CentralBodyGravity::clone() const
{
    return new CentralBodyGravity(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const CentralBodyGravity& aCentralBodyGravity)
{
    aCentralBodyGravity.print(anOutputStream);

    return anOutputStream;
}

bool CentralBodyGravity::isDefined() const
{
    return celestialObjectSPtr_->isDefined();
}

void CentralBodyGravity::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Gravitational Dynamics") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Celestial:" << celestialObjectSPtr_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

void CentralBodyGravity::update(
    const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant
)
{
    // Obtain gravitational acceleration from current object
    const Vector gravitationalAcceleration =
        celestialObjectSPtr_->getGravitationalFieldAt(Position::Meters({x[0], x[1], x[2]}, gcrfSPtr_), anInstant);

    // Add object's gravity to total gravitational acceleration
    Vector3d gravitationalAcceleration_SI = gravitationalAcceleration.inFrame(gcrfSPtr_, anInstant).getValue();

    // Set acceleration
    dxdt[0] = x[3];
    dxdt[1] = x[4];
    dxdt[2] = x[5];
    dxdt[3] += gravitationalAcceleration_SI[0];
    dxdt[4] += gravitationalAcceleration_SI[1];
    dxdt[5] += gravitationalAcceleration_SI[2];
}

Shared<const Celestial> CentralBodyGravity::getCelestial() const
{
    if (!this->celestialObjectSPtr_->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Celestial");
    }

    return celestialObjectSPtr_;
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
