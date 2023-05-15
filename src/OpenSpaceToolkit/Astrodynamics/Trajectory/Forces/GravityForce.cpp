/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Force.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

GravityForce::GravityForce() {}

GravityForce::~GravityForce() {}

// std::ostream& operator<<(std::ostream& anOutputStream, const GravityForce& aGravityForce)
// {
//     aGravityForce.print(anOutputStream);

//     return anOutputStream;
// }

Vector3d GravityForce::getAcceleration(const Position& aPosition, const Instant& anInstant) const
{

    Vector3d gravitationalModelFieldValue = this->gravitationalModelSPtr_->getFieldValueAt(vectorPosition, anInstant);  // vectorPosition is a Vector3d

}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
