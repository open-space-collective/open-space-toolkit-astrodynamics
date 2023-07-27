/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace state
{
namespace coordinatessubsets
{

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

const String CartesianVelocity::DEFAULT_NAME = "CARTESIAN_VELOCITY";

const Shared<const CartesianVelocity> CartesianVelocity::THREE_DIMENSIONAL =
    CartesianVelocity::FromPosition(CartesianPosition::ThreeDimensional());

CartesianVelocity::CartesianVelocity(const String& aName, const Shared<const CartesianPosition>& aCartesianPositionSPtr)
    : CoordinatesSubset(aName, aCartesianPositionSPtr->getSize()),
      cartesianPositionSPtr_(aCartesianPositionSPtr)
{
}

VectorXd CartesianVelocity::add(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBroker
) const
{
    return aCoordinatesBroker->extractCoordinates(aFullCoordinatesVector, *this) +
           aCoordinatesBroker->extractCoordinates(anotherFullCoordinatesVector, *this);
}

VectorXd CartesianVelocity::subtract(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBroker
) const
{
    return aCoordinatesBroker->extractCoordinates(aFullCoordinatesVector, *this) -
           aCoordinatesBroker->extractCoordinates(anotherFullCoordinatesVector, *this);
}

VectorXd CartesianVelocity::inFrame(
    const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const Shared<const Frame>& fromFrame,
    const Shared<const Frame>& toFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBroker
) const
{
    if (this->getSize() != 3)
    {
        throw ostk::core::error::runtime::ToBeImplemented("Frame Transformation");
    }

    const VectorXd positionCoordinates =
        aCoordinatesBroker->extractCoordinates(aFullCoordinatesVector, this->cartesianPositionSPtr_);
    const VectorXd velocityCoordinates = aCoordinatesBroker->extractCoordinates(aFullCoordinatesVector, *this);

    Vector3d toFrameCoordinates =
        Velocity::MetersPerSecond({velocityCoordinates(0), velocityCoordinates(1), velocityCoordinates(2)}, fromFrame)
            .inFrame(
                Position::Meters({positionCoordinates(0), positionCoordinates(1), positionCoordinates(2)}, fromFrame),
                toFrame,
                anInstant
            )
            .getCoordinates();

    return VectorXd::Map(toFrameCoordinates.data(), static_cast<Eigen::Index>(3));
}

Shared<const CartesianVelocity> CartesianVelocity::FromPosition(
    const Shared<const CartesianPosition>& aCartesianPositionSPtr
)
{
    const CartesianVelocity cartesianVelocity = CartesianVelocity(DEFAULT_NAME, aCartesianPositionSPtr);
    return std::make_shared<CartesianVelocity>(cartesianVelocity);
}

Shared<const CartesianVelocity> CartesianVelocity::ThreeDimensional()
{
    return CartesianVelocity::THREE_DIMENSIONAL;
}

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
