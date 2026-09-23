/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Access_Generator__
#define __OpenSpaceToolkit_Astrodynamics_Access_Generator__

#include <optional>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Interval.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/AER.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriterion.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::Interval;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXi;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::AER;
using ostk::physics::Environment;
using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::Access;
using ostk::astrodynamics::access::VisibilityCriterion;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::State;

#define DEFAULT_STEP Duration::Minutes(1.0)
#define DEFAULT_TOLERANCE Duration::Microseconds(1.0)

/// Iteration ceiling for the closest-approach root solve. The bracketed solve converges in roughly a dozen
/// iterations; this only guards against a pathological bracket.
#define DEFAULT_TCA_MAXIMUM_ITERATION_COUNT 100

/// @brief Represents the configuration for an Access target, including azimuth, elevation, and range intervals, as well
/// as position and LLA (Latitude, Longitude, Altitude).
///
/// @details This class provides methods to retrieve the trajectory, position, LLA, and intervals for azimuth,
/// elevation, and range. It also includes a method to get the SEZ (South-East-Zenith) rotation matrix.
class AccessTarget
{
   public:
    enum class Type
    {
        Fixed,
        Trajectory
    };

    /// @brief Access the type
    ///
    /// @code{.cpp}
    ///              AccessTarget accessTarget = { ... } ;
    ///              AccessTarget::Type type = accessTarget.accessType();
    /// @endcode
    ///
    /// @return The type
    const Type& accessType() const;

    /// @brief Access the visibility criteria
    ///
    /// @code{.cpp}
    ///              AccessTarget accessTarget = { ... } ;
    ///              VisibilityCriterion visibilityCriterion = accessTarget.accessVisibilityCriterion();
    /// @endcode
    ///
    /// @return The visibility criteria
    const VisibilityCriterion& accessVisibilityCriterion() const;

    /// @brief Access the trajectory
    ///
    /// @code{.cpp}
    ///              AccessTarget accessTarget = { ... } ;
    ///              const Trajectory& trajectory = accessTarget.accessTrajectory();
    /// @endcode
    ///
    /// @return The trajectory
    const Trajectory& accessTrajectory() const;

    /// @brief Get the position
    ///
    /// @code{.cpp}
    ///              AccessTarget accessTarget = { ... } ;
    ///              Position position = accessTarget.getPosition();
    /// @endcode
    ///
    /// @return The position
    [[deprecated("Use getPosition(getPosition(const Shared<const Celestial>& aCelestialSPtr) instead.")]]
    Position getPosition() const;

    /// @brief Get the position
    ///
    /// @code{.cpp}
    ///              AccessTarget accessTarget = { ... } ;
    ///              Shared<const Celestial> celestialSPtr = ...;
    ///              Position position = accessTarget.getPosition(celestialSPtr);
    /// @endcode
    ///
    /// @return The position
    Position getPosition(const Shared<const Celestial>& aCelestialSPtr) const;

    /// @brief Get the latitude, longitude, and altitude (LLA)
    ///
    /// @code{.cpp}
    ///              AccessTarget accessTarget = { ... } ;
    ///              LLA lla = accessTarget.getLLA();
    /// @endcode
    ///
    /// @param aCelestialSPtr A celestial body
    ///
    /// @return The latitude, longitude, and altitude (LLA)
    LLA getLLA(const Shared<const Celestial>& aCelestialSPtr) const;

    /// @brief Get the rotation matrix (Matrix3d) from ECEF (Earth-Centered-Earth-Fixed) to SEZ (South-East-Zenith)
    /// frame
    ///
    /// @code{.cpp}
    ///              AccessTarget accessTarget = { ... } ;
    ///              Matrix3d sezRotation = accessTarget.computeR_SEZ_ECEF();
    /// @endcode
    ///
    /// @return The SEZ rotation matrix
    Matrix3d computeR_SEZ_ECEF(const Shared<const Celestial>& aCelestialSPtr) const;

    /// @brief Construct an Access Target from an LLA (Latitude, Longitude, Altitude)
    ///
    /// @code{.cpp}
    ///              AccessTarget accessTarget = AccessTarget::FromLLA(
    ///                  visibilityCriterion, lla, celestialSPtr
    ///              );
    /// @endcode
    ///
    /// @param aVisibilityCriterion
    /// @param anLLA
    /// @param aCelestialSPtr
    /// @return Access target
    static AccessTarget FromLLA(
        const VisibilityCriterion& aVisibilityCriterion, const LLA& anLLA, const Shared<const Celestial>& aCelestialSPtr
    );

    /// @brief Construct an Access Target from a position
    ///
    /// @code{.cpp}
    ///              AccessTarget accessTarget = AccessTarget::FromPosition(
    ///                  visibilityCriterion, position
    ///              );
    /// @endcode
    ///
    /// @param aVisibilityCriterion
    /// @param aPosition
    /// @return Access target
    static AccessTarget FromPosition(const VisibilityCriterion& aVisibilityCriterion, const Position& aPosition);

    /// @brief Construct an Access Target from a trajectory
    ///
    /// @code{.cpp}
    ///              AccessTarget accessTarget = AccessTarget::FromTrajectory(
    ///                  visibilityCriterion, trajectory
    ///              );
    /// @endcode
    ///
    /// @param aVisibilityCriterion
    /// @param aTrajectory
    /// @return Access target
    static AccessTarget FromTrajectory(const VisibilityCriterion& aVisibilityCriterion, const Trajectory& aTrajectory);

   private:
    Type type_;
    VisibilityCriterion visibilityCriterion_;
    Trajectory trajectory_;

    AccessTarget(const Type& aType, const VisibilityCriterion& aVisibilityCriterion, const Trajectory& aTrajectory);
};

/// @brief Computes access intervals between a trajectory and one or more access targets.
///
/// @details The Generator evaluates visibility between a "from" trajectory (e.g. a satellite) and
/// one or more AccessTarget objects over a given time interval. It steps through the interval at a
/// configurable cadence, detects crossings of the visibility condition, and then refines each
/// crossing to within a configurable tolerance. Optional access-level and state-level filter
/// functions allow further pruning of the resulting Access objects.
class Generator
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     Generator generator = {
    ///         environment,
    ///         Duration::Minutes(1.0),
    ///         Duration::Microseconds(1.0),
    ///         accessFilter,
    ///         stateFilter
    ///     };
    /// @endcode
    ///
    /// @param anEnvironment The space environment used for frame transformations and celestial body
    /// access.
    /// @param aStep The time step used when sampling the interval to detect visibility crossings.
    /// Defaults to 1 minute.
    /// @param aTolerance The temporal tolerance used when refining the precise time of an access
    /// crossing. Defaults to 1 microsecond.
    /// @param anAccessFilter An optional predicate applied to each candidate Access object; only
    /// accesses for which the predicate returns true are retained.
    /// @param aStateFilter An optional predicate applied to the pair of from/to States at each
    /// sample; samples for which the predicate returns false are treated as non-visible.
    Generator(
        const Environment& anEnvironment,
        const Duration& aStep = DEFAULT_STEP,
        const Duration& aTolerance = DEFAULT_TOLERANCE,
        const std::function<bool(const Access&)>& anAccessFilter = {},
        const std::function<bool(const State&, const State&)>& aStateFilter = {}
    );

    /// @brief Check whether this Generator is defined.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     bool defined = generator.isDefined();
    /// @endcode
    ///
    /// @return True if the Generator is defined, false otherwise.
    bool isDefined() const;

    /// @brief Get the time step used when sampling the interval.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     Duration step = generator.getStep();
    /// @endcode
    ///
    /// @return The sampling time step.
    Duration getStep() const;

    /// @brief Get the temporal tolerance used for crossing refinement.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     Duration tolerance = generator.getTolerance();
    /// @endcode
    ///
    /// @return The crossing refinement tolerance.
    Duration getTolerance() const;

    /// @brief Get the access filter predicate.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     std::function<bool(const Access&)> filter = generator.getAccessFilter();
    /// @endcode
    ///
    /// @return The access filter function, or an empty function if none was set.
    std::function<bool(const Access&)> getAccessFilter() const;

    /// @brief Get the state filter predicate.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     std::function<bool(const State&, const State&)> filter = generator.getStateFilter();
    /// @endcode
    ///
    /// @return The state filter function, or an empty function if none was set.
    std::function<bool(const State&, const State&)> getStateFilter() const;

    /// @brief Get a boolean condition function that evaluates visibility at a given instant.
    ///
    /// @details Returns a callable that, when invoked with an Instant, evaluates whether the
    /// given access target is visible from the "to" trajectory at that moment. This is primarily
    /// used internally to drive the crossing-detection logic, but is exposed for advanced use
    /// cases such as custom solvers.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     std::function<bool(const Instant&)> condition =
    ///         generator.getConditionFunction(accessTarget, toTrajectory);
    ///     bool visible = condition(instant);
    /// @endcode
    ///
    /// @param anAccessTarget The access target to evaluate visibility against.
    /// @param aToTrajectory The trajectory of the observer (e.g. satellite).
    /// @return A function that returns true when the target is visible at a given instant.
    std::function<bool(const Instant&)> getConditionFunction(
        const AccessTarget& anAccessTarget, const Trajectory& aToTrajectory
    ) const;

    /// @brief Compute accesses between a single access target and a trajectory over a time interval.
    ///
    /// @details Steps through the given interval at the configured step size, detects transitions
    /// in the visibility condition, and refines each transition to within the configured tolerance.
    /// The optional access filter and state filter are applied during this process.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     Array<Access> accesses = generator.computeAccesses(
    ///         interval, accessTarget, toTrajectory
    ///     );
    /// @endcode
    ///
    /// @param anInterval The time interval over which to compute accesses.
    /// @param anAccessTarget The access target to evaluate visibility against.
    /// @param aToTrajectory The trajectory of the observer (e.g. satellite).
    /// @param coarse If true, skips precise crossing refinement and returns coarse intervals only.
    /// Defaults to false.
    /// @return An array of Access objects representing each visibility window.
    Array<Access> computeAccesses(
        const physics::time::Interval& anInterval,
        const AccessTarget& anAccessTarget,
        const Trajectory& aToTrajectory,
        const bool& coarse = false
    ) const;

    /// @brief Compute accesses between multiple access targets and a trajectory over a time interval.
    ///
    /// @details Equivalent to calling the single-target overload for each element of
    /// someAccessTargets, but may use a more efficient batched implementation for fixed targets.
    /// Returns one array of Access objects per target, in the same order as someAccessTargets.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     Array<Array<Access>> allAccesses = generator.computeAccesses(
    ///         interval, accessTargets, toTrajectory
    ///     );
    /// @endcode
    ///
    /// @param anInterval The time interval over which to compute accesses.
    /// @param someAccessTargets The array of access targets to evaluate visibility against.
    /// @param aToTrajectory The trajectory of the observer (e.g. satellite).
    /// @param coarse If true, skips precise crossing refinement and returns coarse intervals only.
    /// Defaults to false.
    /// @return An array of access arrays, one per access target, in the same order as
    /// someAccessTargets.
    Array<Array<Access>> computeAccesses(
        const physics::time::Interval& anInterval,
        const Array<AccessTarget>& someAccessTargets,
        const Trajectory& aToTrajectory,
        const bool& coarse = false
    ) const;

    /// @brief Set the time step used when sampling the interval.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     generator.setStep(Duration::Seconds(30.0));
    /// @endcode
    ///
    /// @param aStep The new sampling time step.
    void setStep(const Duration& aStep);

    /// @brief Set the temporal tolerance used for crossing refinement.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     generator.setTolerance(Duration::Milliseconds(1.0));
    /// @endcode
    ///
    /// @param aTolerance The new crossing refinement tolerance.
    void setTolerance(const Duration& aTolerance);

    /// @brief Set the access filter predicate.
    ///
    /// @details The filter is applied to each candidate Access object after all crossings have been
    /// refined. Only accesses for which the predicate returns true are included in the result.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     generator.setAccessFilter([](const Access& access) {
    ///         return access.getDuration() > Duration::Minutes(5.0);
    ///     });
    /// @endcode
    ///
    /// @param anAccessFilter The new access filter function.
    void setAccessFilter(const std::function<bool(const Access&)>& anAccessFilter);

    /// @brief Set the state filter predicate.
    ///
    /// @details The filter is evaluated at each sample instant. If the predicate returns false for
    /// a given pair of from/to States, that sample is treated as non-visible regardless of the
    /// geometric visibility criterion.
    ///
    /// @code{.cpp}
    ///     Generator generator = { ... };
    ///     generator.setStateFilter([](const State& fromState, const State& toState) {
    ///         return someCustomCheck(fromState, toState);
    ///     });
    /// @endcode
    ///
    /// @param aStateFilter The new state filter function.
    void setStateFilter(const std::function<bool(const State&, const State&)>& aStateFilter);

    /// @brief Construct an undefined Generator.
    ///
    /// @code{.cpp}
    ///     Generator generator = Generator::Undefined();
    /// @endcode
    ///
    /// @return An undefined Generator instance.
    static Generator Undefined();

   private:
    Environment environment_;

    Duration step_;
    Duration tolerance_;

    std::function<bool(const Access&)> accessFilter_;
    std::function<bool(const State&, const State&)> stateFilter_;

    /// @brief Compute accesses between a single trajectory-type access target and a trajectory.
    ///
    /// @details Solves the generic visibility condition from getConditionFunction with a TemporalConditionSolver
    /// configured with this Generator's step and tolerance, then builds Access objects from the resulting intervals.
    /// Used for targets that move, where the batched fixed-target path does not apply.
    ///
    /// @param anInterval The time interval over which to compute accesses.
    /// @param anAccessTarget The trajectory-type access target to evaluate visibility against.
    /// @param aToTrajectory The trajectory of the observer (e.g. satellite).
    /// @return An array of Access objects that pass the access filter.
    Array<Access> computeAccessesForTrajectoryTarget(
        const physics::time::Interval& anInterval, const AccessTarget& anAccessTarget, const Trajectory& aToTrajectory
    ) const;

    /// @brief Compute accesses between multiple fixed access targets and a trajectory, in one batched sweep.
    ///
    /// @details Precomputes each target's position and SEZ rotation in the central celestial object's frame, then
    /// samples the analysis interval once at the configured step: at each instant the observer position is
    /// transformed into that frame a single time and the visibility criterion is evaluated for all targets at once.
    /// The state filter, if set, is applied per target. Coarse intervals are then refined with
    /// computePreciseCrossings unless coarse is true.
    ///
    /// @param anInterval The time interval over which to compute accesses.
    /// @param someAccessTargets The fixed access targets. All must share the same VisibilityCriterion type.
    /// @param aToTrajectory The trajectory of the observer (e.g. satellite).
    /// @param coarse If true, skips precise crossing refinement and returns coarse intervals only.
    /// Defaults to false.
    /// @return One array of Access objects per target, in the same order as someAccessTargets.
    /// @throw ostk::core::error::RuntimeError If the targets do not all share the same VisibilityCriterion type.
    Array<Array<Access>> computeAccessesForFixedTargets(
        const physics::time::Interval& anInterval,
        const Array<AccessTarget>& someAccessTargets,
        const Trajectory& aToTrajectory,
        const bool& coarse = false
    ) const;

    /// @brief Build Access objects from access intervals and apply the access filter.
    ///
    /// @details Calls GenerateAccess on each interval, using the central celestial object of the environment (or
    /// Earth if none is set), then drops any Access rejected by the access filter.
    ///
    /// @param someIntervals The access intervals (from acquisition to loss of signal).
    /// @param anInterval The overall analysis interval, used to classify each access as complete or partial.
    /// @param aFromTrajectory The trajectory of the access target.
    /// @param aToTrajectory The trajectory of the observer (e.g. satellite).
    /// @param aFixedFromPositionCoordinates The target's coordinates in the central celestial object's frame, when
    /// the target is fixed. Supplying them lets the closest-approach search skip re-evaluating a constant.
    /// @return An array of Access objects that pass the access filter.
    Array<Access> generateAccessesFromIntervals(
        const Array<physics::time::Interval>& someIntervals,
        const physics::time::Interval& anInterval,
        const Trajectory& aFromTrajectory,
        const Trajectory& aToTrajectory,
        const std::optional<Vector3d>& aFixedFromPositionCoordinates = std::nullopt
    ) const;

    /// @brief Refine the endpoints of coarse access intervals of a fixed target to within the configured tolerance.
    ///
    /// @details Each coarse interval's start is bracketed between the preceding sample (one step earlier, but never
    /// before the previous interval's end plus one step) and its first in-access sample, and its end between its last
    /// in-access sample and the next sample (clamped to the analysis interval). A root solver then locates the
    /// visibility transition in each bracket. Endpoints whose bracket falls outside the analysis interval are set to
    /// the corresponding analysis interval bound.
    ///
    /// @param accessIntervals The coarse access intervals, as sampled at the configured step.
    /// @param anAnalysisInterval The overall analysis interval.
    /// @param fromPositionCoordinate_ITRF The fixed target's coordinates in the central celestial object's frame.
    /// @param aToTrajectory The trajectory of the observer (e.g. satellite).
    /// @param anAccessTarget The fixed access target, providing the visibility criterion and SEZ rotation.
    /// @param aCelestialSPtr The central celestial object whose frame the coordinates are expressed in.
    /// @return The refined access intervals, closed, in the same order as accessIntervals.
    /// @throw ostk::core::error::RuntimeError If the target's VisibilityCriterion type is not supported.
    Array<physics::time::Interval> computePreciseCrossings(
        const Array<physics::time::Interval>& accessIntervals,
        const physics::time::Interval& anAnalysisInterval,
        const Vector3d& fromPositionCoordinate_ITRF,
        const Trajectory& aToTrajectory,
        const AccessTarget& anAccessTarget,
        const Shared<const Celestial>& aCelestialSPtr
    ) const;

    /// @brief Convert a sampled in-access flag series into contiguous access intervals.
    ///
    /// @details Each run of consecutive in-access samples becomes one closed interval spanning its first and last
    /// in-access instants. A run still open at the last sample ends at that sample.
    ///
    /// @param inAccess Per-sample in-access flags (1 when in access, 0 otherwise), aligned with instants.
    /// @param instants The sampled instants.
    /// @return The coarse access intervals, in chronological order.
    static Array<physics::time::Interval> ComputeIntervals(const VectorXi& inAccess, const Array<Instant>& instants);

    /// @brief Build a single Access from an access interval.
    ///
    /// @details The access is Complete when neither of its endpoints coincides with the corresponding endpoint of the
    /// global interval, and Partial otherwise. The time of closest approach is found with FindTimeOfClosestApproach,
    /// and the maximum elevation is the elevation at that instant.
    ///
    /// @param anAccessInterval The access interval, from acquisition to loss of signal.
    /// @param aGlobalInterval The overall analysis interval.
    /// @param aFromTrajectory The trajectory of the access target.
    /// @param aToTrajectory The trajectory of the observer (e.g. satellite).
    /// @param aTolerance The temporal tolerance used when solving for the time of closest approach.
    /// @param aCelestialSPtr The central celestial object whose frame the geometry is computed in.
    /// @param aFixedFromPositionCoordinates The target's coordinates in the central celestial object's frame, when
    /// the target is fixed.
    /// @return The Access. For a Partial access whose closest approach did not converge, the time of closest approach
    /// and maximum elevation are undefined.
    /// @throw ostk::core::error::RuntimeError If the closest approach of a Complete access does not converge.
    static Access GenerateAccess(
        const physics::time::Interval& anAccessInterval,
        const physics::time::Interval& aGlobalInterval,
        const Trajectory& aFromTrajectory,
        const Trajectory& aToTrajectory,
        const Duration& aTolerance,
        const Shared<const Celestial>& aCelestialSPtr,
        const std::optional<Vector3d>& aFixedFromPositionCoordinates = std::nullopt
    );

    /// @brief Find the time of closest approach within an access interval.
    ///
    /// @details Range is smooth across an access and has a single minimum, so its time derivative changes sign exactly
    /// once. This solves for that sign change rather than minimizing the range itself: a range minimum is locally
    /// flat, which makes it expensive to localize, whereas the derivative crosses zero once. When the range rate
    /// does not change sign (the access is clipped by the analysis interval), the closest approach is the start or
    /// end of the interval.
    ///
    /// @param anAccessInterval The access interval to search.
    /// @param aFromTrajectory The trajectory of the access target.
    /// @param aToTrajectory The trajectory of the observer (e.g. satellite).
    /// @param aTolerance The temporal tolerance of the root solve.
    /// @param aCelestialSPtr The central celestial object whose frame the range is computed in.
    /// @param aFixedFromPositionCoordinates The target's coordinates in the central celestial object's frame, when
    /// the target is fixed. Supplying them skips evaluating aFromTrajectory at each iteration.
    /// @return The time of closest approach, or an undefined Instant if the root solve did not converge.
    static Instant FindTimeOfClosestApproach(
        const physics::time::Interval& anAccessInterval,
        const Trajectory& aFromTrajectory,
        const Trajectory& aToTrajectory,
        const Duration& aTolerance,
        const Shared<const Celestial>& aCelestialSPtr,
        const std::optional<Vector3d>& aFixedFromPositionCoordinates = std::nullopt
    );

    /// @brief Calculate the geocentric elevation of the observer as seen from the access target at an instant.
    ///
    /// @details Elevation is measured above the plane normal to the target's position vector in the central
    /// celestial object's frame (i.e. relative to the geocentric, not geodetic, vertical).
    ///
    /// @param anInstant The instant at which to evaluate the elevation.
    /// @param aFromTrajectory The trajectory of the access target.
    /// @param aToTrajectory The trajectory of the observer (e.g. satellite).
    /// @param aCelestialSPtr The central celestial object whose frame the geometry is computed in.
    /// @param aFixedFromPositionCoordinates The target's coordinates in the central celestial object's frame, when
    /// the target is fixed. Supplying them skips evaluating aFromTrajectory.
    /// @return The elevation angle.
    static Angle CalculateElevationAt(
        const Instant& anInstant,
        const Trajectory& aFromTrajectory,
        const Trajectory& aToTrajectory,
        const Shared<const Celestial>& aCelestialSPtr,
        const std::optional<Vector3d>& aFixedFromPositionCoordinates = std::nullopt
    );

    /// @brief Calculate the azimuth, elevation and range from one position to another.
    ///
    /// @details Both positions are expressed in the local NED frame at the geodetic location of aFromPosition on the
    /// given celestial object.
    ///
    /// @param anInstant The instant at which to perform the frame transformations.
    /// @param aFromPosition The reference position (e.g. the access target).
    /// @param aToPosition The observed position (e.g. the satellite).
    /// @param aCelestialSPtr The celestial object defining the geodetic model and NED frame.
    /// @return The AER of aToPosition relative to aFromPosition.
    static AER CalculateAer(
        const Instant& anInstant,
        const Position& aFromPosition,
        const Position& aToPosition,
        const Shared<const Celestial>& aCelestialSPtr
    );
};

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk

#endif
