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

const Shared<const Invariant> Invariant::MASS = std::make_shared<Invariant>(Invariant::MASS_DEFAULT_NAME, 1);

Invariant::Invariant(const String& aName, const Size& aSize)
    : CoordinatesSubset(aName, aSize)
{
}

VectorXd Invariant::inFrame(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& allCoordinates,
    [[maybe_unused]] const Shared<const Frame>& fromFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBroker,
    [[maybe_unused]] const Shared<const Frame>& toFrame
) const
{
    return aCoordinatesBroker->extract(allCoordinates, *this);
}

Shared<const Invariant> Invariant::Mass()
{
    return Invariant::MASS;
}

}  // namespace coordinatessubsets
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
