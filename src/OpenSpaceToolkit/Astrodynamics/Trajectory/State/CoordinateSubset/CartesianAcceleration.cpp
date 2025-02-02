/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{
namespace coordinatesubset
{

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Velocity;

CartesianAcceleration::CartesianAcceleration(
    const Shared<const CartesianPosition>& aCartesianPositionSPtr,
    const Shared<const CartesianVelocity>& aCartesianVelocitySPtr,
    const String& aName
)
    : CoordinateSubset(aName, aCartesianPositionSPtr->getSize()),
      cartesianPositionSPtr_(aCartesianPositionSPtr),
      cartesianVelocitySPtr_(aCartesianVelocitySPtr)
{
}

CartesianAcceleration::~CartesianAcceleration() {}

VectorXd CartesianAcceleration::inFrame(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const Shared<const Frame>& fromFrame,
    const Shared<const Frame>& toFrame,
    const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
) const
{
    if (fromFrame == toFrame)
    {
        return aCoordinateBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this);
    }

    throw ostk::core::error::runtime::ToBeImplemented("CartesianAcceleration::inFrame");
}

Shared<const CartesianAcceleration> CartesianAcceleration::Default()
{
    static const Shared<const CartesianAcceleration> cartesianAcceleration = std::make_shared<CartesianAcceleration>(
        CartesianPosition::Default(), CartesianVelocity::Default(), "CARTESIAN_ACCELERATION"
    );

    return cartesianAcceleration;
}

Shared<const CartesianAcceleration> CartesianAcceleration::ThrustAcceleration()
{
    static const Shared<const CartesianAcceleration> cartesianAcceleration = std::make_shared<CartesianAcceleration>(
        CartesianPosition::Default(), CartesianVelocity::Default(), "THRUST_ACCELERATION"
    );

    return cartesianAcceleration;
}

}  // namespace coordinatesubset
}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
