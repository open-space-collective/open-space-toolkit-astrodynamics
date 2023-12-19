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

using ostk::math::object::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

CartesianVelocity::CartesianVelocity(const Shared<const CartesianPosition>& aCartesianPositionSPtr, const String& aName)
    : CoordinatesSubset(aName, aCartesianPositionSPtr->getSize()),
      cartesianPositionSPtr_(aCartesianPositionSPtr)
{
}

CartesianVelocity::~CartesianVelocity() {}

VectorXd CartesianVelocity::add(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    return aCoordinatesBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this) +
           aCoordinatesBrokerSPtr->extractCoordinate(anotherFullCoordinatesVector, *this);
}

VectorXd CartesianVelocity::subtract(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    return aCoordinatesBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this) -
           aCoordinatesBrokerSPtr->extractCoordinate(anotherFullCoordinatesVector, *this);
}

VectorXd CartesianVelocity::inFrame(
    const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const Shared<const Frame>& fromFrame,
    const Shared<const Frame>& toFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    const VectorXd positionCoordinates =
        aCoordinatesBrokerSPtr->extractCoordinate(aFullCoordinatesVector, this->cartesianPositionSPtr_);
    const VectorXd velocityCoordinates = aCoordinatesBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this);

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

Shared<const CartesianVelocity> CartesianVelocity::Default()
{
    static const Shared<const CartesianVelocity> cartesianVelocity =
        std::make_shared<CartesianVelocity>(CartesianPosition::Default(), "CARTESIAN_VELOCITY");
    return cartesianVelocity;
}

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
