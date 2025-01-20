/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriterion.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

VisibilityCriterion::AERInterval::AERInterval(
    const Interval<Real>& anAzimuthInterval,
    const Interval<Real>& anElevationInterval,
    const Interval<Real>& aRangeInterval
)
    : azimuth(anAzimuthInterval),
      elevation(anElevationInterval),
      range(aRangeInterval)
{
    if (!azimuth.isDefined() || !elevation.isDefined() || !range.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval");
    }

    if (azimuth.getLowerBound() < 0.0 || azimuth.getUpperBound() > 360.0)
    {
        throw ostk::core::error::RuntimeError(
            "The azimuth interval [{}, {}] must be in the range [0, 360] deg",
            azimuth.accessLowerBound(),
            azimuth.accessUpperBound()
        );
    }

    if (elevation.getLowerBound() < -90.0 || elevation.getUpperBound() > 90.0)
    {
        throw ostk::core::error::RuntimeError(
            "The elevation interval [{}, {}] must be in the range [-90, 90] deg.",
            elevation.accessLowerBound(),
            elevation.accessUpperBound()
        );
    }

    if (range.getLowerBound() < 0.0)
    {
        throw ostk::core::error::RuntimeError(
            "The range interval [{}, {}] must be positive.", range.accessLowerBound(), range.accessUpperBound()
        );
    }

    azimuth = Interval<Real>(
        anAzimuthInterval.getLowerBound() * M_PI / 180.0,
        anAzimuthInterval.getUpperBound() * M_PI / 180.0,
        anAzimuthInterval.getType()
    );
    elevation = Interval<Real>(
        anElevationInterval.getLowerBound() * M_PI / 180.0,
        anElevationInterval.getUpperBound() * M_PI / 180.0,
        anElevationInterval.getType()
    );
}

bool VisibilityCriterion::AERInterval::isSatisfied(const AER& anAer) const
{
    return this->isSatisfied(
        anAer.getAzimuth().inRadians(), anAer.getElevation().inRadians(), anAer.getRange().inMeters()
    );
}

bool VisibilityCriterion::AERInterval::isSatisfied(
    const Real& anAzimuth_Radians, const Real& anElevation_Radians, const Real& aRange_Meters
) const
{
    return this->azimuth.contains(anAzimuth_Radians) && this->elevation.contains(anElevation_Radians) &&
           this->range.contains(aRange_Meters);
}

bool VisibilityCriterion::AERInterval::operator==(const AERInterval& anAerInterval) const
{
    return this->azimuth == anAerInterval.azimuth && this->elevation == anAerInterval.elevation &&
           this->range == anAerInterval.range;
}

bool VisibilityCriterion::AERInterval::operator!=(const AERInterval& anAerInterval) const
{
    return !((*this) == anAerInterval);
}

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk
