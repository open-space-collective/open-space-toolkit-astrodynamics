/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Access_Generator__
#define __OpenSpaceToolkit_Astrodynamics_Access_Generator__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Map.hpp>
#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Interval.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/AER.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>

namespace ostk
{
namespace astro
{
namespace access
{

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Map;
using ostk::core::ctnr::Pair;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::Interval;

using ostk::physics::Environment;
using ostk::physics::coord::Position;
using ostk::physics::coord::spherical::AER;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::units::Angle;
using ostk::physics::units::Length;
using ostk::physics::env::obj::Celestial;

using ostk::astro::Access;
using ostk::astro::Trajectory;
using ostk::astro::trajectory::State;

#define DEFAULT_STEP Duration::Minutes(1.0)
#define DEFAULT_TOLERANCE Duration::Microseconds(1.0)

class Generator
{
   public:
    friend class GeneratorContext;

    Generator(
        const Environment& anEnvironment,
        const Duration& aStep = DEFAULT_STEP,
        const Duration& aTolerance = DEFAULT_TOLERANCE
    );

    Generator(
        const Environment& anEnvironment,
        const std::function<bool(const AER&)>& anAerFilter,
        const std::function<bool(const Access&)>& anAccessFilter = {},
        const std::function<bool(const State&, const State&)>& aStateFilter = {},
        const Duration& aStep = DEFAULT_STEP,
        const Duration& aTolerance = DEFAULT_TOLERANCE
    );

    bool isDefined() const;

    Duration getStep() const;

    Duration getTolerance() const;

    Array<Access> computeAccesses(
        const physics::time::Interval& anInterval, const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
    ) const;

    void setStep(const Duration& aStep);

    void setTolerance(const Duration& aTolerance);

    void setAerFilter(const std::function<bool(const AER&)>& anAerFilter);

    void setAccessFilter(const std::function<bool(const Access&)>& anAccessFilter);

    void setStateFilter(const std::function<bool(const State&, const State&)>& aStateFilter);

    static Generator Undefined();

    /// @brief              Constructs an access generator with defined AER ranges
    ///
    /// @param              [in] anAzimuthRange An azimuth interval [deg]
    /// @param              [in] anElevationRange An elevation interval [deg]
    /// @param              [in] aRangeRange A range interval [m]
    /// @param              [in] anEnvironment An environment
    /// @return             An access generator

    static Generator AerRanges(
        const Interval<Real>& anAzimuthRange,
        const Interval<Real>& anElevationRange,
        const Interval<Real>& aRangeRange,
        const Environment& anEnvironment
    );

    /// @brief              Constructs an access generator with a defined AER mask
    ///
    /// @param              [in] anAzimuthElevationMask An azimuth-elevation mask [deg]
    /// @param              [in] aRangeRange A range interval [m]
    /// @param              [in] anEnvironment An environment
    /// @return             An access generator

    static Generator AerMask(
        const Map<Real, Real>& anAzimuthElevationMask,
        const Interval<Real>& aRangeRange,
        const Environment& anEnvironment
    );

   private:
    Environment environment_;

    Duration step_;
    Duration tolerance_;

    std::function<bool(const AER&)> aerFilter_;
    std::function<bool(const Access&)> accessFilter_;
    std::function<bool(const State&, const State&)> stateFilter_;
};

class GeneratorContext
{
   public:
    GeneratorContext(
        const physics::time::Interval& anInterval,
        const Trajectory& aFromTrajectory,
        const Trajectory& aToTrajectory,
        const Environment& anEnvironment,
        const Generator& aGenerator
    );

    bool isAccessActiveAt(const Instant& anInstant);

    Access generateAccess(const physics::time::Interval& anAccessInterval) const;

   private:
    physics::time::Interval interval_;
    Trajectory fromTrajectory_;
    Trajectory toTrajectory_;
    Environment environment_;
    const Generator& generator_;

    const Shared<const Celestial> earthSPtr_;

    Pair<State, State> getStatesAt(const Instant& anInstant) const;

    Pair<Position, Position> getPositionsFromStates(const State& aFromState, const State& aToState) const;

    AER calculateAer(const Instant& anInstant, const Position& aFromPosition, const Position& aToPosition) const;

    bool isAccessActive(const Instant& anInstant, const State& aFromState, const State& aToState);

    Instant findTimeOfClosestApproach(const physics::time::Interval& anAccessInterval) const;

    Angle calculateElevationAt(const Instant& anInstant) const;
};

}  // namespace access
}  // namespace astro
}  // namespace ostk

#endif
