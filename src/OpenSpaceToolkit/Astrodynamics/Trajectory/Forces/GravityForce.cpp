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

GravityForce::GravityForce(const Shared<const GravitationalModel>& aGravitationalModelSPtr)
    : gravitationalModelSPtr_(aGravitationalModelSPtr)
{
}

GravityForce::~GravityForce() {}

GravityForce* GravityForce::clone() const
{
    return new GravityForce(*this);
}

Vector3d GravityForce::getContribution(const Position& aPosition, const Instant& anInstant) const
{

    Vector3d positionCoordinates = aPosition.getCoordinates();
    Vector3d gravitationalModelFieldValue = this->gravitationalModelSPtr_->getFieldValueAt(positionCoordinates, anInstant);  // vectorPosition is a Vector3d

    std::cout << "Force Grav" << std::endl;
    std::cout << gravitationalModelFieldValue << std::endl ;

    return gravitationalModelFieldValue;

}

}  // namespace force
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
