/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Access_Constraint__
#define __OpenSpaceToolkit_Astrodynamics_Access_Constraint__

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

class Constraint
{
   public:
    struct AERIntervalConstraint
    {
        Interval<Real> azimuth;    // radians
        Interval<Real> elevation;  // radians
        Interval<Real> range;      // meters

        AERIntervalConstraint(
            const Interval<Real>& anAzimuth, const Interval<Real>& anElevation, const Interval<Real>& aRange
        );

        bool isSatisfied(const AER& anAer) const;

        bool isSatisifed(const Real& anAzimuthRadians, const Real& anElevationRadians, const Real& aRangeMeters) const;
    };

    struct MaskConstraint
    {
        Map<Real, Real> azimuthElevationMask;  // radians, radians
        Interval<Real> range;                  // meters

        MaskConstraint(const Map<Real, Real>& anAzimuthElevationMask, const Interval<Real>& aRange);

        bool isSatisfied(const AER& anAer) const;

        bool isSatisfied(const Real& anAzimuthRadians, const Real& anElevationRadians, const Real& aRangeMeters) const;
    };

    struct LineOfSightConstraint
    {
        mutable Environment environment;

        LineOfSightConstraint(const Environment& anEnvironment);

        bool isSatisfied(
            const Instant& anInstant, const Vector3d& aFromPositionCoordinates, const Vector3d& aToPositionCoordinates
        ) const;
    };

    struct ElevationIntervalConstraint
    {
        Interval<Real> elevation;  // radians
        
        ElevationIntervalConstraint(const Interval<Real>& anElevationInterval);

        bool isSatisfied(const Real& anElevation) const;
    };

    static Constraint FromAERIntervals(
        const Interval<Real>& anAzimuthInterval, const Interval<Real>& anElevationInterval, const Interval<Real>& aRangeInterval
    );

    static Constraint FromMask(const Map<Real, Real>& anAzimuthElevationMask, const Interval<Real>& aRangeInterval);

    static Constraint FromLineOfSight(const Environment& anEnvironment);

    static Constraint FromElevationInterval(const Interval<Real>& anElevationInterval);

    bool isAERIntervalBased() const;

    bool isMaskBased() const;

    bool isLineOfSightBased() const;

    bool isElevationIntervalBased() const;

    std::optional<AERIntervalConstraint> getAERIntervalConstraint() const;

    std::optional<MaskConstraint> getMaskConstraint() const;

    std::optional<LineOfSightConstraint> getLineOfSightConstraint() const;

    std::optional<ElevationIntervalConstraint> getElevationIntervalConstraint() const;

   private:
    std::variant<AERIntervalConstraint, MaskConstraint, LineOfSightConstraint> constraint_;

    template <typename T>
    explicit Constraint(const T& aConstraint);
};

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk

#endif
