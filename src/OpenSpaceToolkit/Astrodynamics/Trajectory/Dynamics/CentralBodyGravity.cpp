/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Dynamics/CentralBodyGravity.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
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
    : CentralBodyGravity(
          aCelestialObjectSPtr, String::Format("Central Body Gravity [{}]", aCelestialObjectSPtr->getName())
      )
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

void CentralBodyGravity::applyContribution(
    const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant
) const
{
    // Obtain gravitational acceleration from current object
    const Vector3d gravitationalAccelerationSI =
        celestialObjectSPtr_->getGravitationalFieldAt(Position::Meters({x[0], x[1], x[2]}, gcrfSPtr_), anInstant)
            .inFrame(gcrfSPtr_, anInstant)
            .getValue();

    // Integrate velocity states
    dxdt[3] += gravitationalAccelerationSI[0];
    dxdt[4] += gravitationalAccelerationSI[1];
    dxdt[5] += gravitationalAccelerationSI[2];
}

void CentralBodyGravity::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Central Body Gravitational Dynamics") : void();

    Dynamics::print(anOutputStream, false);

    // TBI: Print Celestial once we have
    ostk::core::utils::Print::Line(anOutputStream) << "Celestial:" << celestialObjectSPtr_->getName();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
