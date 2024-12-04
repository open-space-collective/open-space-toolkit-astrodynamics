/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriterion.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

VisibilityCriterion::ElevationInterval::ElevationInterval(const Interval<Real>& anElevationInterval)
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

    this->elevation = Interval<Real>(
        anElevationInterval.getLowerBound() * M_PI / 180.0,
        anElevationInterval.getUpperBound() * M_PI / 180.0,
        anElevationInterval.getType()
    );
}

bool VisibilityCriterion::ElevationInterval::isSatisfied(const Real& anElevation) const
{
    return this->elevation.contains(anElevation);
}

bool VisibilityCriterion::ElevationInterval::operator==(const ElevationInterval& anElevationInterval) const
{
    return this->elevation == anElevationInterval.elevation;
}

bool VisibilityCriterion::ElevationInterval::operator!=(const ElevationInterval& anElevationInterval) const
{
    return !(*this == anElevationInterval);
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
