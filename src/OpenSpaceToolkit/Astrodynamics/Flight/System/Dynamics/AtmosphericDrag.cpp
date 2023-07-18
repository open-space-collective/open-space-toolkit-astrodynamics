/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianVelocity.hpp>

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
using ostk::core::types::String;

using ostk::physics::Unit;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;
using ostk::physics::coord::Position;
using ostk::physics::data::Scalar;

using ostk::astro::trajectory::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::coordinatessubsets::CartesianVelocity;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

AtmosphericDrag::AtmosphericDrag(const Shared<const Celestial>& aCelestialSPtr, const SatelliteSystem& aSatelliteSystem)
    : AtmosphericDrag(
          aCelestialSPtr, aSatelliteSystem, String::Format("Atmospheric Drag [{}]", aCelestialSPtr->getName())
      )
{
}

AtmosphericDrag::AtmosphericDrag(
    const Shared<const Celestial>& aCelestialSPtr, const SatelliteSystem& aSatelliteSystem, const String& aName
)
    : Dynamics(aName),
      celestialObjectSPtr_(aCelestialSPtr),
      satelliteSystem_(aSatelliteSystem)
{
    if (!celestialObjectSPtr_ || !celestialObjectSPtr_->atmosphericModelIsDefined())
    {
        throw ostk::core::error::runtime::Undefined("Atmospheric Model");
    }

    if (!satelliteSystem_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Satellite System");
    }
}

AtmosphericDrag::~AtmosphericDrag() {}

AtmosphericDrag* AtmosphericDrag::clone() const
{
    return new AtmosphericDrag(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const AtmosphericDrag& anAtmosphericDrag)
{
    anAtmosphericDrag.print(anOutputStream);

    return anOutputStream;
}

bool AtmosphericDrag::isDefined() const
{
    return celestialObjectSPtr_->isDefined() && satelliteSystem_.isDefined();
}

Shared<const Celestial> AtmosphericDrag::getCelestial() const
{
    if (!celestialObjectSPtr_->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Celestial Object");
    }

    return celestialObjectSPtr_;
}

SatelliteSystem AtmosphericDrag::getSatelliteSystem() const
{
    if (!satelliteSystem_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Satellite System");
    }

    return satelliteSystem_;
}

Array<Shared<const CoordinatesSubset>> AtmosphericDrag::getReadCoordinateSubsets() const
{
    return {CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()};
}

Array<Shared<const CoordinatesSubset>> AtmosphericDrag::getWriteCoordinateSubsets() const
{
    return {CartesianVelocity::ThreeDimensional()};
}

void AtmosphericDrag::declareCoordinates(const Shared<CoordinatesBroker>& coordinatesBroker)
{
    this->positionIndex_ = coordinatesBroker->addSubset(CartesianPosition::ThreeDimensional());
    this->velocityIndex_ = coordinatesBroker->addSubset(CartesianVelocity::ThreeDimensional());
}

void AtmosphericDrag::applyContribution(
    const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant
) const
{
    (void)anInstant;

    Vector3d positionCoordinates = Vector3d(x[positionIndex_], x[positionIndex_ + 1], x[positionIndex_ + 2]);
    Vector3d velocityCoordinates = Vector3d(x[velocityIndex_], x[velocityIndex_ + 1], x[velocityIndex_ + 2]);

    // Get atmospheric density
    const Real atmosphericDensity =
        celestialObjectSPtr_->getAtmosphericDensityAt(Position::Meters(positionCoordinates, gcrfSPtr_), anInstant)
            .inUnit(Unit::Derived(Derived::Unit::MassDensity(Mass::Unit::Kilogram, Length::Unit::Meter)))
            .getValue();

    const Vector3d earthAngularVelocity =
        gcrfSPtr_->getTransformTo(Frame::ITRF(), anInstant).getAngularVelocity();  // rad/s

    const Vector3d relativeVelocity = velocityCoordinates - earthAngularVelocity.cross(positionCoordinates);

    const Real mass = satelliteSystem_.getMass().inKilograms();  // TBI: Add wet mass from state vector
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();

    // Add object's gravity to total gravitational acceleration
    const Vector3d dragAccelerationSI =
        -(0.5 / mass) * dragCoefficient * surfaceArea * atmosphericDensity * relativeVelocity.norm() * relativeVelocity;

    // Integrate velocity states
    dxdt[velocityIndex_] += dragAccelerationSI[0];
    dxdt[velocityIndex_ + 1] += dragAccelerationSI[1];
    dxdt[velocityIndex_ + 2] += dragAccelerationSI[2];
}

void AtmosphericDrag::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Atmospheric Drag Dynamics") : void();

    Dynamics::print(anOutputStream, false);

    // TBI: Print Celestial once there is a print method in OSTk physics
    ostk::core::utils::Print::Line(anOutputStream) << "Celestial:" << celestialObjectSPtr_->getName();

    ostk::core::utils::Print::Line(anOutputStream) << "Satellite System:" << satelliteSystem_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
