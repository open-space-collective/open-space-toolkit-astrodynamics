/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::physics::coordinate::Position;
using ostk::physics::Unit;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Time;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

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

Array<Shared<const CoordinateSubset>> AtmosphericDrag::getReadCoordinateSubsets() const
{
    return {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
        CoordinateSubset::SurfaceArea(),
        CoordinateSubset::DragCoefficient(),
    };
}

Array<Shared<const CoordinateSubset>> AtmosphericDrag::getWriteCoordinateSubsets() const
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
        aFrameSPtr->getTransformTo(celestialObjectSPtr_->accessFrame(), anInstant).getAngularVelocity();  // rad/s

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
}  // namespace astrodynamics
}  // namespace ostk
