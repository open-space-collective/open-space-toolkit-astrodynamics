/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/Invariant.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace coordinatessubsets
{

const String Invariant::MASS_DEFAULT_NAME = "MASS";

Invariant::Invariant(const String& aName, const Size& aSize)
    : CoordinatesSubset(aName, aSize)
{
}

VectorXd Invariant::inFrame(
    const Instant& anInstant,
    const VectorXd& allCoordinates,
    const Shared<const Frame>& fromFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBroker,
    const Shared<const Frame>& toFrame
) const
{
    return aCoordinatesBroker->extract(allCoordinates, *this);
}

Invariant Invariant::Mass()
{
    return {Invariant::MASS_DEFAULT_NAME, 1};
}

}  // namespace coordinatessubsets
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
