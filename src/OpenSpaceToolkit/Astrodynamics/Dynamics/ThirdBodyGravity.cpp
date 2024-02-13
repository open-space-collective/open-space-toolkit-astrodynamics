/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::core::type::String;

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

ThirdBodyGravity::ThirdBodyGravity(const Shared<const Celestial>& aCelestialObjectSPtr)
    : ThirdBodyGravity(aCelestialObjectSPtr, String::Format("Third Body Gravity [{}]", aCelestialObjectSPtr->getName()))
{
}

ThirdBodyGravity::ThirdBodyGravity(const Shared<const Celestial>& aCelestialObjectSPtr, const String& aName)
    : Dynamics(aName),
      celestialObjectSPtr_(aCelestialObjectSPtr)
{
    if (!celestialObjectSPtr_ || !celestialObjectSPtr_->gravitationalModelIsDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational Model");
    }

    // TBI: This fails for the earth as we cannot calculate the acceleration at the origin of the GCRF
    if (celestialObjectSPtr_->getName() == "Earth")
    {
        throw ostk::core::error::RuntimeError("Cannot calculate third body acceleration for the Earth yet.");
    }
}

ThirdBodyGravity::~ThirdBodyGravity() {}

std::ostream& operator<<(std::ostream& anOutputStream, const ThirdBodyGravity& aThirdBodyGravity)
{
    aThirdBodyGravity.print(anOutputStream);

    return anOutputStream;
}

bool ThirdBodyGravity::isDefined() const
{
    return celestialObjectSPtr_->isDefined();
}

Shared<const Celestial> ThirdBodyGravity::getCelestial() const
{
    if (!celestialObjectSPtr_->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Celestial");
    }

    return celestialObjectSPtr_;
}

Array<Shared<const CoordinateSubset>> ThirdBodyGravity::getReadCoordinateSubsets() const
{
    return {
        CartesianPosition::Default(),
    };
}

Array<Shared<const CoordinateSubset>> ThirdBodyGravity::getWriteCoordinateSubsets() const
{
    return {
        CartesianVelocity::Default(),
    };
}

VectorXd ThirdBodyGravity::computeContribution(
    const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
) const
{
    // Obtain 3rd body effect on center of Central Body (origin in GCRF) aka 3rd body correction
    // TBI: This fails for the earth as we cannot calculate the acceleration at the origin of the GCRF
    Vector3d gravitationalAccelerationSI =
        -celestialObjectSPtr_->getGravitationalFieldAt(Position::Meters({0.0, 0.0, 0.0}, aFrameSPtr), anInstant)
             .inFrame(aFrameSPtr, anInstant)
             .getValue();

    Vector3d positionCoordinates = Vector3d(x[0], x[1], x[2]);

    gravitationalAccelerationSI +=
        celestialObjectSPtr_->getGravitationalFieldAt(Position::Meters(positionCoordinates, aFrameSPtr), anInstant)
            .inFrame(aFrameSPtr, anInstant)
            .getValue();

    // Compute contribution
    VectorXd contribution(3);
    contribution << gravitationalAccelerationSI[0], gravitationalAccelerationSI[1], gravitationalAccelerationSI[2];

    return contribution;
}

void ThirdBodyGravity::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Third Body Gravitational Dynamics") : void();

    Dynamics::print(anOutputStream, false);

    // TBI: Print Celestial once we have a proper implementation of Celestial::print

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk
