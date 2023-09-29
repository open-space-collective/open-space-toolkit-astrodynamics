/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{

using ostk::core::types::String;

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

Array<Shared<const CoordinatesSubset>> ThirdBodyGravity::getReadCoordinatesSubsets() const
{
    return {
        CartesianPosition::Default(),
    };
}

Array<Shared<const CoordinatesSubset>> ThirdBodyGravity::getWriteCoordinatesSubsets() const
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

    // TBI: Print Celestial once there is a print method in OSTk physics
    ostk::core::utils::Print::Line(anOutputStream) << "Celestial:" << celestialObjectSPtr_->getName();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace astro
}  // namespace ostk
