/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion__
#define __OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion__

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
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

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
using ostk::physics::unit::Angle;

/// @brief Represents a visibility criterion for accesses between objects.
class VisibilityCriterion
{
   public:
    /// @brief Represents an AER interval visibility criterion.
    struct AERInterval
    {
        Interval<Real> azimuth;    /// Azimuth interval in radians.
        Interval<Real> elevation;  /// Elevation interval in radians.
        Interval<Real> range;      /// Range interval in meters.

        /// @brief Constructs an AERInterval.
        ///
        /// @param anAzimuth The azimuth interval.
        /// @param anElevation The elevation interval.
        /// @param aRange The range interval.
        AERInterval(const Interval<Real>& anAzimuth, const Interval<Real>& anElevation, const Interval<Real>& aRange);

        /// @brief Checks if the given AER satisfies the criterion.
        ///
        /// @param anAer The AER to check.
        /// @return True if the criterion is satisfied.
        bool isSatisfied(const AER& anAer) const;

        /// @brief Checks if the given azimuth, elevation, and range values satisfy the criterion.
        ///
        /// @param anAzimuthRadians The azimuth in radians.
        /// @param anElevationRadians The elevation in radians.
        /// @param aRangeMeters The range in meters.
        /// @return True if the criterion is satisfied.
        bool isSatisfied(const Real& anAzimuthRadians, const Real& anElevationRadians, const Real& aRangeMeters) const;

        /// @brief Equality operator.
        ///
        /// @param anAerInterval Another AERInterval.
        /// @return True if equal.
        bool operator==(const AERInterval& anAerInterval) const;

        /// @brief Inequality operator.
        ///
        /// @param anAerInterval Another AERInterval.
        /// @return True if not equal.
        bool operator!=(const AERInterval& anAerInterval) const;
    };

    /// @brief Represents an AER mask visibility criterion.
    struct AERMask
    {
        Map<Real, Real> azimuthElevationMask;  /// Map of azimuth angles to elevation angles in radians.
        Interval<Real> range;                  /// Range interval in meters.

        /// @brief Constructs an AERMask.
        ///
        /// @param anAzimuthElevationMask The azimuth-elevation mask.
        /// @param aRange The range interval.
        AERMask(const Map<Real, Real>& anAzimuthElevationMask, const Interval<Real>& aRange);

        /// @brief Checks if the given AER satisfies the criterion.
        ///
        /// @param anAer The AER to check.
        /// @return True if the criterion is satisfied.
        bool isSatisfied(const AER& anAer) const;

        /// @brief Checks if the given azimuth, elevation, and range values satisfy the criterion.
        ///
        /// @param anAzimuthRadians The azimuth in radians.
        /// @param anElevationRadians The elevation in radians.
        /// @param aRangeMeters The range in meters.
        /// @return True if the criterion is satisfied.
        bool isSatisfied(const Real& anAzimuthRadians, const Real& anElevationRadians, const Real& aRangeMeters) const;

        /// @brief Equality operator.
        ///
        /// @param anAerMask Another AERMask.
        /// @return True if equal.
        bool operator==(const AERMask& anAerMask) const;

        /// @brief Inequality operator.
        ///
        /// @param anAerMask Another AERMask.
        /// @return True if not equal.
        bool operator!=(const AERMask& anAerMask) const;
    };

    /// @brief Represents a line-of-sight visibility criterion.
    struct LineOfSight
    {
        mutable Environment environment;  /// The environment for line-of-sight calculations.

        /// @brief Constructs a LineOfSight criterion.
        ///
        /// @param anEnvironment The environment to consider.
        LineOfSight(const Environment& anEnvironment);

        /// @brief Checks if the line-of-sight criterion is satisfied between two positions at a given instant.
        ///
        /// @param anInstant The time at which to perform the check.
        /// @param aFromPositionCoordinates The position coordinates (in meters) of the observer.
        /// @param aToPositionCoordinates The position coordinates (in meters) of the target.
        /// @return True if there is a clear line of sight.
        bool isSatisfied(
            const Instant& anInstant, const Vector3d& aFromPositionCoordinates, const Vector3d& aToPositionCoordinates
        ) const;

        /// @brief Equality operator.
        ///
        /// @param aLineOfSight Another LineOfSight.
        /// @return True if equal.
        bool operator==(const LineOfSight& aLineOfSight) const;

        /// @brief Inequality operator.
        ///
        /// @param aLineOfSight Another LineOfSight.
        /// @return True if not equal.
        bool operator!=(const LineOfSight& aLineOfSight) const;
    };

    /// @brief Represents an elevation interval visibility criterion.
    struct ElevationInterval
    {
        Interval<Real> elevation;  /// Elevation interval in radians.

        /// @brief Constructs an ElevationInterval.
        ///
        /// @param anElevationInterval The elevation interval.
        ElevationInterval(const Interval<Real>& anElevationInterval);

        /// @brief Checks if the given elevation angle satisfies the criterion.
        ///
        /// @param anElevationAngle The elevation angle.
        /// @return True if the criterion is satisfied.
        bool isSatisfied(const Angle& anElevationAngle) const;

        /// @brief Checks if the given elevation angle satisfies the criterion.
        ///
        /// @param anElevation The elevation in radians.
        /// @return True if the criterion is satisfied.
        bool isSatisfied(const Real& anElevation) const;

        /// @brief Equality operator.
        ///
        /// @param anElevationInterval Another ElevationInterval.
        /// @return True if equal.
        bool operator==(const ElevationInterval& anElevationInterval) const;

        /// @brief Inequality operator.
        ///
        /// @param anElevationInterval Another ElevationInterval.
        /// @return True if not equal.
        bool operator!=(const ElevationInterval& anElevationInterval) const;
    };

    /// @brief Equality operator.
    ///
    /// @param aVisibilityCriterion Another VisibilityCriterion.
    /// @return True if equal.
    bool operator==(const VisibilityCriterion& aVisibilityCriterion) const;

    /// @brief Inequality operator.
    ///
    /// @param aVisibilityCriterion Another VisibilityCriterion.
    /// @return True if not equal.
    bool operator!=(const VisibilityCriterion& aVisibilityCriterion) const;

    /// @brief Creates a visibility criterion from azimuth, elevation, and range intervals.
    ///
    /// @param anAzimuthInterval Azimuth interval in radians.
    /// @param anElevationInterval Elevation interval in radians.
    /// @param aRangeInterval Range interval in meters.
    /// @return The visibility criterion instance.
    static VisibilityCriterion FromAERInterval(
        const Interval<Real>& anAzimuthInterval,
        const Interval<Real>& anElevationInterval,
        const Interval<Real>& aRangeInterval
    );

    /// @brief Creates a visibility criterion from an azimuth-elevation mask and range interval.
    ///
    /// @param anAzimuthElevationMask A map of azimuth angles to elevation angles in radians.
    /// @param aRangeInterval Range interval in meters.
    /// @return The visibility criterion instance.
    static VisibilityCriterion FromAERMask(
        const Map<Real, Real>& anAzimuthElevationMask, const Interval<Real>& aRangeInterval
    );

    /// @brief Creates a visibility criterion based on line-of-sight considerations.
    ///
    /// @param anEnvironment The environment to consider for line-of-sight calculations.
    /// @return The visibility criterion instance.
    static VisibilityCriterion FromLineOfSight(const Environment& anEnvironment);

    /// @brief Creates a visibility criterion from an elevation interval.
    ///
    /// @param anElevationInterval The elevation interval in radians.
    /// @return The visibility criterion instance.
    static VisibilityCriterion FromElevationInterval(const Interval<Real>& anElevationInterval);

    /// @brief Checks if the visibility criterion is of type T.
    ///
    /// @tparam T The type to check.
    /// @return True if the criterion is of type T.
    template <typename T>
    bool is() const
    {
        return std::holds_alternative<T>(visibilityCriterion_);
    }

    /// @brief Casts the visibility criterion to type T if possible.
    ///
    /// @tparam T The type to cast to.
    /// @return An optional containing the criterion cast to type T, or std::nullopt if the criterion is not of type T.
    template <typename T>
    std::optional<T> as() const
    {
        if (const auto* visibilityCriterionPtr = std::get_if<T>(&visibilityCriterion_))
        {
            return *visibilityCriterionPtr;
        }

        return std::nullopt;
    }

   private:
    std::variant<AERInterval, AERMask, LineOfSight, ElevationInterval> visibilityCriterion_;

    /// @brief Constructs a VisibilityCriterion from a specific type.
    ///
    /// @tparam T The type of the visibility criterion.
    /// @param aVisibilityCriterion The visibility criterion.
    template <typename T>
    explicit VisibilityCriterion(const T& aVisibilityCriterion)
        : visibilityCriterion_(aVisibilityCriterion)
    {
    }
};

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk

#endif
