/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Forces/CentralBodyGravityForce.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace force
{

CentralBodyGravityForce::CentralBodyGravityForce(const Shared<const GravitationalModel>& aGravitationalModelSPtr)
    :   gravitationalModelSPtr_(aGravitationalModelSPtr)
{
}

CentralBodyGravityForce::~CentralBodyGravityForce() {}

CentralBodyGravityForce* CentralBodyGravityForce::clone() const
{
    return new CentralBodyGravityForce(*this);
}

Vector3d CentralBodyGravityForce::getContribution(const Position& aPosition, const Instant& anInstant, const Velocity& aVelocity, const Real& aSurfaceArea, const Real& aDragCoefficient, const Real& aMass) const
{

    Vector3d positionCoordinates = aPosition.getCoordinates();
    Vector3d gravitationalModelFieldValue = this->gravitationalModelSPtr_->getFieldValueAt(positionCoordinates, anInstant);  // vectorPosition is a Vector3d

    return gravitationalModelFieldValue;

}

}  // namespace force
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
