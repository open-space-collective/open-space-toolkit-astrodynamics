/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriteria__
#define __OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriteria__

#include <optional>
#include <variant>

#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Interval.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/AER.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

using ostk::core::container::Map;
using ostk::core::type::Real;

using ostk::mathematics::object::Interval;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::spherical::AER;
using ostk::physics::Environment;
using ostk::physics::time::Instant;

class VisibilityCriteria
{
   public:
    struct AERInterval
    {
        Interval<Real> azimuth;    // radians
        Interval<Real> elevation;  // radians
        Interval<Real> range;      // meters

        AERInterval(const Interval<Real>& anAzimuth, const Interval<Real>& anElevation, const Interval<Real>& aRange);

        bool isSatisfied(const AER& anAer) const;

        bool isSatisfied(const Real& anAzimuthRadians, const Real& anElevationRadians, const Real& aRangeMeters) const;
    };

    struct AERMask
    {
        Map<Real, Real> azimuthElevationMask;  // radians, radians
        Interval<Real> range;                  // meters

        AERMask(const Map<Real, Real>& anAzimuthElevationMask, const Interval<Real>& aRange);

        bool isSatisfied(const AER& anAer) const;

        bool isSatisfied(const Real& anAzimuthRadians, const Real& anElevationRadians, const Real& aRangeMeters) const;
    };

    struct LineOfSight
    {
        mutable Environment environment;

        LineOfSight(const Environment& anEnvironment);

        bool isSatisfied(
            const Instant& anInstant, const Vector3d& aFromPositionCoordinates, const Vector3d& aToPositionCoordinates
        ) const;
    };

    struct ElevationInterval
    {
        Interval<Real> elevation;  // radians

        ElevationInterval(const Interval<Real>& anElevationInterval);

        bool isSatisfied(const Real& anElevation) const;
    };

    static VisibilityCriteria FromAERInterval(
        const Interval<Real>& anAzimuthInterval,
        const Interval<Real>& anElevationInterval,
        const Interval<Real>& aRangeInterval
    );

    static VisibilityCriteria FromAERMask(
        const Map<Real, Real>& anAzimuthElevationMask, const Interval<Real>& aRangeInterval
    );

    static VisibilityCriteria FromLineOfSight(const Environment& anEnvironment);

    static VisibilityCriteria FromElevationInterval(const Interval<Real>& anElevationInterval);

    template <typename T>
    bool is() const
    {
        return std::holds_alternative<T>(visibilityCriteria_);
    }

    template <typename T>
    std::optional<T> as() const
    {
        if (const auto* constraintPtr = std::get_if<T>(&visibilityCriteria_))
        {
            return *constraintPtr;
        }

        return std::nullopt;
    }

   private:
    std::variant<AERInterval, AERMask, LineOfSight, ElevationInterval> visibilityCriteria_;

    template <typename T>
    explicit VisibilityCriteria(const T& aVisibilityCriteria)
        : visibilityCriteria_(aVisibilityCriteria)
    {
    }
};

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk

#endif
