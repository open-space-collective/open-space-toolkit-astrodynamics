/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AnalyticalThirdBodyGravity.hpp>
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
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Time;

using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

AnalyticalThirdBodyGravity::AnalyticalThirdBodyGravity(const Shared<const Celestial>& aCelestialObjectSPtr)
    : AnalyticalThirdBodyGravity(
          aCelestialObjectSPtr, String::Format("Analytical Third Body Gravity [{}]", aCelestialObjectSPtr->getName())
      )
{
}

AnalyticalThirdBodyGravity::AnalyticalThirdBodyGravity(
    const Shared<const Celestial>& aCelestialObjectSPtr, const String& aName
)
    : Dynamics(aName),
      celestialObjectSPtr_(aCelestialObjectSPtr),
      gravitationalParameter_SI_(Real::Undefined())
{
    if (!celestialObjectSPtr_ || !celestialObjectSPtr_->gravitationalModelIsDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational Model");
    }

    // The third body perturbation is expressed with respect to the central body,
    // and is therefore singular at the origin of the central body

    if (celestialObjectSPtr_->getName() == "Earth")
    {
        throw ostk::core::error::RuntimeError("Cannot calculate third body acceleration for the Earth yet.");
    }

    // Fail at construction, rather than during a propagation, if the celestial object provides no analytical model

    celestialObjectSPtr_->computeAnalyticalPosition(Instant::J2000());

    gravitationalParameter_SI_ = celestialObjectSPtr_->getGravitationalParameter().in(GravitationalParameterSIUnit);
}

AnalyticalThirdBodyGravity::~AnalyticalThirdBodyGravity() {}

std::ostream& operator<<(std::ostream& anOutputStream, const AnalyticalThirdBodyGravity& anAnalyticalThirdBodyGravity)
{
    anAnalyticalThirdBodyGravity.print(anOutputStream);

    return anOutputStream;
}

bool AnalyticalThirdBodyGravity::isDefined() const
{
    return celestialObjectSPtr_->isDefined();
}

Shared<const Celestial> AnalyticalThirdBodyGravity::getCelestial() const
{
    if (!celestialObjectSPtr_->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Celestial");
    }

    return celestialObjectSPtr_;
}

Array<Shared<const CoordinateSubset>> AnalyticalThirdBodyGravity::getReadCoordinateSubsets() const
{
    return {
        CartesianPosition::Default(),
    };
}

Array<Shared<const CoordinateSubset>> AnalyticalThirdBodyGravity::getWriteCoordinateSubsets() const
{
    return {
        CartesianVelocity::Default(),
    };
}

VectorXd AnalyticalThirdBodyGravity::computeContribution(
    const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
) const
{
    // Position of the third body with respect to the origin of the central body, from the analytical model

    const Vector3d thirdBodyPositionCoordinates =
        celestialObjectSPtr_->computeAnalyticalPosition(anInstant).inFrame(aFrameSPtr, anInstant).getCoordinates();

    // Position of the spacecraft with respect to the third body

    const Vector3d positionCoordinates = {x[0], x[1], x[2]};

    const Vector3d relativePositionCoordinates = positionCoordinates - thirdBodyPositionCoordinates;

    // Third body perturbation: the direct attraction of the third body on the spacecraft,
    // and the indirect effect from its attraction on the central body

    const Vector3d gravitationalAccelerationSI =
        -gravitationalParameter_SI_ *
        ((relativePositionCoordinates / std::pow(relativePositionCoordinates.norm(), 3)) +
         (thirdBodyPositionCoordinates / std::pow(thirdBodyPositionCoordinates.norm(), 3)));

    // Compute contribution
    VectorXd contribution(3);
    contribution << gravitationalAccelerationSI[0], gravitationalAccelerationSI[1], gravitationalAccelerationSI[2];

    return contribution;
}

void AnalyticalThirdBodyGravity::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Analytical Third Body Gravitational Dynamics")
                     : void();

    Dynamics::print(anOutputStream, false);

    // TBI: Print Celestial once we have a proper implementation of Celestial::print

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk
