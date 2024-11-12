/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Access_Constraint__
#define __OpenSpaceToolkit_Astrodynamics_Access_Constraint__

#include <optional>
#include <variant>

#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Interval.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/AER.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

using ostk::core::container::Map;
using ostk::core::type::Real;

using ostk::mathematics::object::Interval;
using ostk::mathematics::object::Vector2d;

using ostk::physics::coordinate::spherical::AER;

class Constraint
{
   public:
    struct IntervalConstraint
    {
        Interval<Real> azimuth;    // degrees
        Interval<Real> elevation;  // degrees
        Interval<Real> range;      // meters

        IntervalConstraint(
            const Interval<Real>& anAzimuth, const Interval<Real>& anElevation, const Interval<Real>& aRange
        );

        bool isSatisfied(const AER& anAer) const;
    };

    struct MaskConstraint
    {
        Map<Real, Real> azimuthElevationMask;  // degrees, degrees
        Interval<Real> range;                  // meters

        MaskConstraint(const Map<Real, Real>& anAzimuthElevationMask, const Interval<Real>& aRange);

        bool isSatisfied(const AER& anAer) const;
    };

    static Constraint FromIntervals(
        const Interval<Real>& azimuth, const Interval<Real>& elevation, const Interval<Real>& range
    );

    static Constraint FromMask(const Map<Real, Real>& azimuthElevationMask, const Interval<Real>& range);

    bool isSatisfied(const AER& aer) const;

    bool isMaskBased() const;

    bool isIntervalBased() const;

    std::optional<IntervalConstraint> getIntervalConstraint() const;

    std::optional<MaskConstraint> getMaskConstraint() const;

    Interval<Real> getRangeInterval() const;

   private:
    std::variant<IntervalConstraint, MaskConstraint> constraint_;

    explicit Constraint(IntervalConstraint constraint);

    explicit Constraint(MaskConstraint constraint);
};

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk

#endif