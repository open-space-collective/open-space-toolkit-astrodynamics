/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Model.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Spherical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Sun.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
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

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Position;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::environment::gravitational::Moon;
using ostk::physics::environment::gravitational::Spherical;
using ostk::physics::environment::gravitational::Sun;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Time;
using GravitationalModel = ostk::physics::environment::gravitational::Model;

using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

ThirdBodyGravity::ThirdBodyGravity(const Shared<const Celestial>& aCelestialObjectSPtr)
    : ThirdBodyGravity(aCelestialObjectSPtr, String::Format("Third Body Gravity [{}]", aCelestialObjectSPtr->getName()))
{
}

ThirdBodyGravity::ThirdBodyGravity(const Shared<const Celestial>& aCelestialObjectSPtr, const String& aName)
    : Dynamics(aName),
      celestialObjectSPtr_(aCelestialObjectSPtr),
      usesPointMassGravitationalModel_(false)
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

    usesPointMassGravitationalModel_ = ThirdBodyGravity::IsPointMassGravitationalModel(*celestialObjectSPtr_);
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
    const Vector3d positionCoordinates = Vector3d(x[0], x[1], x[2]);

    Vector3d gravitationalAccelerationSI;

    if (usesPointMassGravitationalModel_)
    {
        // For a point-mass (spherical) gravitational model, the field does not depend on the body's
        // orientation. The third-body perturbation can therefore be computed directly from the body's
        // position in the working frame (a pure translation lookup) rather than by rotating the query
        // position into the body's ephemeris-attached frame and rotating the resulting acceleration back
        // out, twice.
        const Vector3d thirdBodyPositionCoordinates =
            celestialObjectSPtr_->getPositionIn(aFrameSPtr, anInstant).getCoordinates();
        const Vector3d relativePositionCoordinates = thirdBodyPositionCoordinates - positionCoordinates;

        const Real mu = celestialObjectSPtr_->getGravitationalParameter().in(GravitationalParameterSIUnit);

        gravitationalAccelerationSI =
            mu * (relativePositionCoordinates / std::pow(relativePositionCoordinates.norm(), 3) -
                  thirdBodyPositionCoordinates / std::pow(thirdBodyPositionCoordinates.norm(), 3));
    }
    else
    {
        // Obtain 3rd body effect on center of Central Body (origin in GCRF) aka 3rd body correction
        // TBI: This fails for the earth as we cannot calculate the acceleration at the origin of the GCRF
        gravitationalAccelerationSI =
            -celestialObjectSPtr_->getGravitationalFieldAt(Position::Meters({0.0, 0.0, 0.0}, aFrameSPtr), anInstant)
                 .inFrame(aFrameSPtr, anInstant)
                 .getValue();

        gravitationalAccelerationSI +=
            celestialObjectSPtr_->getGravitationalFieldAt(Position::Meters(positionCoordinates, aFrameSPtr), anInstant)
                .inFrame(aFrameSPtr, anInstant)
                .getValue();
    }

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

bool ThirdBodyGravity::IsPointMassGravitationalModel(const Celestial& aCelestial)
{
    const Shared<const GravitationalModel> gravitationalModelSPtr = aCelestial.accessGravitationalModel();

    // The generic point-mass model, potentially used directly by third bodies without a dedicated
    // higher-fidelity gravitational model class (e.g., a custom `Celestial` built with `Spherical`).
    if (dynamic_cast<const Spherical*>(gravitationalModelSPtr.get()) != nullptr)
    {
        return true;
    }

    // Sun and Moon only ever support a point-mass (`Spherical`) gravitational model in this library, but
    // their gravitational models are their own classes (not `Spherical`) so they must be checked explicitly.
    if (const Sun* sunGravitationalModelPtr = dynamic_cast<const Sun*>(gravitationalModelSPtr.get()))
    {
        return sunGravitationalModelPtr->getType() == Sun::Type::Spherical;
    }

    if (const Moon* moonGravitationalModelPtr = dynamic_cast<const Moon*>(gravitationalModelSPtr.get()))
    {
        return moonGravitationalModelPtr->getType() == Moon::Type::Spherical;
    }

    // Earth supports higher-fidelity (spherical harmonics) models, so only its `Spherical` type qualifies.
    // Earth cannot actually reach this class as a third body (see the constructor), but this is kept for
    // completeness/defensiveness should that restriction ever be lifted.
    if (const Earth* earthGravitationalModelPtr = dynamic_cast<const Earth*>(gravitationalModelSPtr.get()))
    {
        return earthGravitationalModelPtr->getType() == Earth::Type::Spherical;
    }

    return false;
}

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk
