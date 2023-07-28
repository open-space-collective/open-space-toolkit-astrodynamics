/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
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

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::data::Vector;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;

using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

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

Array<Shared<const CoordinatesSubset>> CentralBodyGravity::getReadCoordinatesSubsets() const
{
    return {CartesianPosition::Default()};
}

Array<Shared<const CoordinatesSubset>> CentralBodyGravity::getWriteCoordinatesSubsets() const
{
    return {CartesianVelocity::Default()};
}

VectorXd CentralBodyGravity::computeContribution(
    const Instant& anInstant, const VectorXd& reducedX, const Shared<const Frame>& aFrame
) const
{
    Vector3d positionCoordinates = Vector3d(reducedX[0], reducedX[1], reducedX[2]);

    // Obtain gravitational acceleration from current object
    const Vector3d gravitationalAccelerationSI =
        celestialObjectSPtr_->getGravitationalFieldAt(Position::Meters(positionCoordinates, aFrame), anInstant)
            .inFrame(aFrame, anInstant)
            .getValue();

    // Compute contribution
    VectorXd contribution(3);
    contribution << gravitationalAccelerationSI[0], gravitationalAccelerationSI[1], gravitationalAccelerationSI[2];

    return contribution;
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
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
