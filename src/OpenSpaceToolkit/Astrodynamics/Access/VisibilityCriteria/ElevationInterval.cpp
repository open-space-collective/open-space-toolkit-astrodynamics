/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriteria.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

VisibilityCriteria::ElevationInterval::ElevationInterval(const Interval<Real>& anElevationInterval)
    : elevation(anElevationInterval)
{
    if (elevation.getLowerBound() < -90.0 || elevation.getUpperBound() > 90.0)
    {
        throw ostk::core::error::RuntimeError(
            "The elevation interval [{}, {}] must be in the range [-90, 90] deg.",
            elevation.accessLowerBound(),
            elevation.accessUpperBound()
        );
    }

    elevation = Interval<Real>(
        anElevationInterval.getLowerBound() * M_PI / 180.0,
        anElevationInterval.getUpperBound() * M_PI / 180.0,
        anElevationInterval.getType()
    );
}

bool VisibilityCriteria::ElevationInterval::isSatisfied(const Real& anElevation) const
{
    return elevation.contains(anElevation);
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
