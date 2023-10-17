/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::physics::Unit;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;
using ostk::physics::coord::Position;

using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

AtmosphericDrag::AtmosphericDrag(const Shared<const Celestial>& aCelestialSPtr)
    : AtmosphericDrag(aCelestialSPtr, String::Format("Atmospheric Drag [{}]", aCelestialSPtr->getName()))
{
}

AtmosphericDrag::AtmosphericDrag(const Shared<const Celestial>& aCelestialSPtr, const String& aName)
    : Dynamics(aName),
      celestialObjectSPtr_(aCelestialSPtr)
{
    if (!celestialObjectSPtr_ || !celestialObjectSPtr_->atmosphericModelIsDefined())
    {
        throw ostk::core::error::runtime::Undefined("Atmospheric Model");
    }
}

AtmosphericDrag::~AtmosphericDrag() {}

std::ostream& operator<<(std::ostream& anOutputStream, const AtmosphericDrag& anAtmosphericDrag)
{
    anAtmosphericDrag.print(anOutputStream);

    return anOutputStream;
}

bool AtmosphericDrag::isDefined() const
{
    return celestialObjectSPtr_->isDefined();
}

Shared<const Celestial> AtmosphericDrag::getCelestial() const
{
    if (!celestialObjectSPtr_->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Celestial Object");
    }

    return celestialObjectSPtr_;
}

Array<Shared<const CoordinatesSubset>> AtmosphericDrag::getReadCoordinatesSubsets() const
{
    return {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
        CoordinatesSubset::SurfaceArea(),
        CoordinatesSubset::DragCoefficient(),
    };
}

Array<Shared<const CoordinatesSubset>> AtmosphericDrag::getWriteCoordinatesSubsets() const
{
    return {
        CartesianVelocity::Default(),
    };
}

VectorXd AtmosphericDrag::computeContribution(
    const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
) const
{
    Vector3d positionCoordinates = Vector3d(x[0], x[1], x[2]);
    Vector3d velocityCoordinates = Vector3d(x[3], x[4], x[5]);
    const Real mass = x[6];         // kg
    const Real surfaceArea = x[7];  // m^2
    const Real dragCoefficient = x[8];

    // Get atmospheric density
    const Real atmosphericDensity =
        celestialObjectSPtr_->getAtmosphericDensityAt(Position::Meters(positionCoordinates, aFrameSPtr), anInstant)
            .inUnit(Unit::Derived(Derived::Unit::MassDensity(Mass::Unit::Kilogram, Length::Unit::Meter)))
            .getValue();

    const Vector3d earthAngularVelocity =
        aFrameSPtr->getTransformTo(Frame::ITRF(), anInstant).getAngularVelocity();  // rad/s

    const Vector3d relativeVelocity = velocityCoordinates - earthAngularVelocity.cross(positionCoordinates);

    // Compute drag contribution to state derivative
    const Vector3d dragAccelerationSI =
        -(0.5 / mass) * surfaceArea * dragCoefficient * atmosphericDensity * relativeVelocity.norm() * relativeVelocity;

    // Compute contribution
    VectorXd contribution(3);
    contribution << dragAccelerationSI[0], dragAccelerationSI[1], dragAccelerationSI[2];

    return contribution;
}

void AtmosphericDrag::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Atmospheric Drag Dynamics") : void();

    Dynamics::print(anOutputStream, false);

    // TBI: Print Celestial once we have a proper implementation of Celestial::print

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace astro
}  // namespace ostk
