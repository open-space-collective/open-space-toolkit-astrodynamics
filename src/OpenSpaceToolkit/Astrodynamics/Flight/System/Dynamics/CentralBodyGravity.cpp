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
    : CentralBodyGravity(aCelestialObjectSPtr, String::Format("Central Body Gravity [{}]", aCelestialObjectSPtr->getName()))
{
}

CentralBodyGravity::CentralBodyGravity(const Shared<const Celestial>& aCelestialObjectSPtr, const String& aName)
    : Dynamics(aName),
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

void CentralBodyGravity::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Gravitational Dynamics") : void();

    Dynamics::print(anOutputStream, false);

    ostk::core::utils::Print::Line(anOutputStream) << "Celestial:" << celestialObjectSPtr_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

bool CentralBodyGravity::isDefined() const
{
    return celestialObjectSPtr_->isDefined();
}

Shared<const Celestial> CentralBodyGravity::getCelestial() const
{
    if (!celestialObjectSPtr_->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Celestial");
    }

    return celestialObjectSPtr_;
}

void CentralBodyGravity::applyContribution(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant) const
{
    // Obtain gravitational acceleration from current object
    const Vector3d gravitationalAcceleration =
        celestialObjectSPtr_->getGravitationalFieldAt(Position::Meters({x[0], x[1], x[2]}, gcrfSPtr_), anInstant).inFrame(gcrfSPtr_, anInstant).getValue();

    // Integrate velocity states
    dxdt[3] += gravitationalAcceleration[0];
    dxdt[4] += gravitationalAcceleration[1];
    dxdt[5] += gravitationalAcceleration[2];
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
