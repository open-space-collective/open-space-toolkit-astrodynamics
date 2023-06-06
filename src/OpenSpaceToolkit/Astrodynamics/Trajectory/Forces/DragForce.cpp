/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Forces/DragForce.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace force
{

DragForce::DragForce(const Shared<const Celestial>& aCelestialSPtr)
    :   celestialSPtr_(aCelestialSPtr)
{
}

DragForce::~DragForce() {}

DragForce* DragForce::clone() const
{
    return new DragForce(*this);
}

Vector3d DragForce::getContribution(const Position& aPosition, const Instant& anInstant, const Velocity& aVelocity, const Real& aSurfaceArea, const Real& aDragCoefficient, const Real& aMass) const
{

    (void) anInstant;
    (void) aMass;

    // Link with OSTk Physics MR
    // if (!((this->celestialSPtr_->accessAtmosphericModel()).isDefined()))
    // {
    //     throw ostk::core::error::runtime::Undefined("Environment");
    // }

    // Atmospheric Density
    const Real atmosphericDensityValue = this->celestialSPtr_->getAtmosphericDensityAt(aPosition).getValue();

    // Relative Velocity
    const Vector3d earthAngularVelocity = {0, 0, 7.2921159e-5};  // rad/s, TBI: use angular velocity from frames transforms
    const Vector3d relativeVelocity = aVelocity.getCoordinates() - earthAngularVelocity.cross(aVelocity.getCoordinates());

    // Calculate drag Force
    const Vector3d dragForce = -(0.5) * aDragCoefficient * aSurfaceArea * atmosphericDensityValue *
                                        relativeVelocity.norm() * relativeVelocity;

    return dragForce ;

}

}
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
