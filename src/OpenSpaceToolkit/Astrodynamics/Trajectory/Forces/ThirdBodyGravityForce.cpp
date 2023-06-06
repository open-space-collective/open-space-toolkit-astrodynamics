/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Forces/ThirdBodyGravityForce.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace force
{

ThirdBodyGravityForce::ThirdBodyGravityForce(const Shared<const GravitationalModel>& aGravitationalModelSPtr)
    :   gravitationalModelSPtr_(aGravitationalModelSPtr)
{
}

ThirdBodyGravityForce::~ThirdBodyGravityForce() {}

ThirdBodyGravityForce* ThirdBodyGravityForce::clone() const
{
    return new ThirdBodyGravityForce(*this);
}

Vector3d ThirdBodyGravityForce::getContribution(const Position& aPosition, const Instant& anInstant, const Velocity& aVelocity, const Real& aSurfaceArea, const Real& aDragCoefficient, const Real& aMass) const
{

    // void aPosition

    // Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_)  Frame::GCRF() is where we want the value tbh
    // inFrame(gcrfSPtr_, currentInstant).getValue()

    Vector3d gravitationalModelFieldValue = -this->gravitationalModelSPtr_->getFieldValueAt({0.0, 0.0, 0.0}, anInstant);  // vectorPosition is a Vector3d

    return gravitationalModelFieldValue;

}

}  // namespace force
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
