/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Forces/GravityForce.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace force
{

GravityForce::GravityForce(const Shared<const Celestial>& aCelestialSPtr)
    :   celestialSPtr_(aCelestialSPtr)
{
}

GravityForce::~GravityForce() {}

GravityForce* GravityForce::clone() const
{
    return new GravityForce(*this);
}

Vector3d GravityForce::getContribution(const Position& aPosition, const Instant& anInstant, const Velocity& aVelocity, const Real& aSurfaceArea, const Real& aDragCoefficient, const Real& aMass) const
{

    (void) aVelocity;
    (void) aSurfaceArea;
    (void) aDragCoefficient;
    (void) aMass;

    Vector3d gravitationalModelFieldValue;

    const String celestialObjectName = celestialSPtr_->accessName();

    if (celestialObjectName == "Earth")
    {
        gravitationalModelFieldValue = celestialSPtr_->getGravitationalFieldAt(aPosition).inFrame(Frame::GCRF(), anInstant).getValue();  // vectorPosition is a Vector3d
    }
    else
    {
        gravitationalModelFieldValue = - celestialSPtr_->getGravitationalFieldAt(Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF())).inFrame(Frame::GCRF(), anInstant).getValue();
    }

    return aMass * gravitationalModelFieldValue;

}

}  // namespace force
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
