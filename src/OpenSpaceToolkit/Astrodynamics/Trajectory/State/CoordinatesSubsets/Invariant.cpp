/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/Invariant.hpp>

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

Invariant::Invariant(const String& aName, const Size& aSize)
    : CoordinatesSubset(aName, aSize)
{
}

Invariant::~Invariant() {}

VectorXd Invariant::add(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    return aCoordinatesBrokerSPtr->extractCoordinates(aFullCoordinatesVector, *this) +
           aCoordinatesBrokerSPtr->extractCoordinates(anotherFullCoordinatesVector, *this);
}

VectorXd Invariant::subtract(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    return aCoordinatesBrokerSPtr->extractCoordinates(aFullCoordinatesVector, *this) -
           aCoordinatesBrokerSPtr->extractCoordinates(anotherFullCoordinatesVector, *this);
}

VectorXd Invariant::inFrame(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& fromFrame,
    [[maybe_unused]] const Shared<const Frame>& toFrame,
    [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    VectorXd invariantCoordinates = aCoordinatesBrokerSPtr->extractCoordinates(aFullCoordinatesVector, *this);

    return invariantCoordinates;
}

Shared<const Invariant> Invariant::Mass()
{
    static const Shared<const Invariant> mass =
        std::make_shared<Invariant>("MASS", 1);
    return mass;
}

}  // namespace coordinatessubsets
}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
