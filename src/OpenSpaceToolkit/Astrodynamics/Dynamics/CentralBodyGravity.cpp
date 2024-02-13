/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Position;
using ostk::physics::data::Vector;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Time;

using ostk::astrodynamics::trajectory::state::coordinatessubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatessubset::CartesianVelocity;

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

Array<Shared<const CoordinateSubset>> CentralBodyGravity::getReadCoordinateSubsets() const
{
    return {
        CartesianPosition::Default(),
    };
}

Array<Shared<const CoordinateSubset>> CentralBodyGravity::getWriteCoordinateSubsets() const
{
    return {
        CartesianVelocity::Default(),
    };
}

VectorXd CentralBodyGravity::computeContribution(
    const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
) const
{
    Vector3d positionCoordinates = {x[0], x[1], x[2]};

    // Obtain gravitational acceleration from current object
    const Vector3d gravitationalAccelerationSI = celestialObjectSPtr_
                                                     ->getGravitationalFieldAt(
                                                         Position::Meters(positionCoordinates, aFrameSPtr), anInstant
                                                     )  // TBI: Assumes x is given in GCRF
                                                     .inFrame(aFrameSPtr, anInstant)
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

    // TBI: Print Celestial once we have a proper implementation of Celestial::print

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk
