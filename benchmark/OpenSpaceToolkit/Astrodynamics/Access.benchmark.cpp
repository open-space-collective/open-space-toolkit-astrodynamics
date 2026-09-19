/// Apache License 2.0

#include "benchmark/benchmark.h"

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Interval.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriterion.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

using ostk::core::container::Array;
using ostk::core::type::Index;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::curvefitting::Interpolator;
using MathInterval = ostk::mathematics::object::Interval<Real>;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::Environment;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::time::Time;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;

using ostk::astrodynamics::Access;
using ostk::astrodynamics::access::AccessTarget;
using ostk::astrodynamics::access::Generator;
using ostk::astrodynamics::access::VisibilityCriterion;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::model::Tabulated;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;

static const int DEFAULT_ITERATIONS = 10;

// The tabulated scenarios cover much longer windows (and far more targets), so each iteration is expensive; keep the
// count low.
static const int TABULATED_ITERATIONS = 3;

static const Environment REFERENCE_ENVIRONMENT = Environment::Default();

static const Instant REFERENCE_START_INSTANT = Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC);

// One-week analysis window (used by the single-target TLE benchmark and the 100-target tabulated benchmark).
static const Instant REFERENCE_END_INSTANT = Instant::DateTime(DateTime(2023, 1, 8, 0, 0, 0), Scale::UTC);

// Two-week analysis window (used by the single-target tabulated benchmarks).
static const Instant REFERENCE_TWO_WEEK_END_INSTANT = REFERENCE_START_INSTANT + Duration::Days(14.0);

static void computeAccesses(
    benchmark::State& state, const AccessTarget& anAccessTarget, const Trajectory& aToTrajectory
)
{
    const Generator generator = {REFERENCE_ENVIRONMENT};
    const Interval interval = Interval::Closed(REFERENCE_START_INSTANT, REFERENCE_END_INSTANT);

    state.ResumeTiming();
    benchmark::DoNotOptimize(generator.computeAccesses(interval, anAccessTarget, aToTrajectory));
}

static void benchmark001(benchmark::State& state)
{
    for (auto _ : state)
    {
        state.PauseTiming();

        // Ground Station
        const LLA groundStationLla = {Angle::Degrees(-45.0), Angle::Degrees(-170.0), Length::Meters(5.0)};
        const Position groundStationPosition = Position::Meters(
            groundStationLla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_), Frame::ITRF()
        );
        const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromLineOfSight(REFERENCE_ENVIRONMENT);
        const AccessTarget groundStationAccessTarget =
            AccessTarget::FromPosition(visibilityCriterion, groundStationPosition);

        // Satellite
        const TLE tle = {
            "1 39419U 13066D   18248.44969859 -.00000394  00000-0 -31796-4 0  9997",
            "2 39419  97.6313 314.6863 0012643 218.7350 141.2966 14.93878994260975"
        };
        const SGP4 sgp4 = {tle};
        const Orbit satelliteOrbit = {sgp4, REFERENCE_ENVIRONMENT.accessCelestialObjectWithName("Earth")};

        computeAccesses(state, groundStationAccessTarget, satelliteOrbit);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Tabulated trajectory model scenarios.
//
// A sun-synchronous satellite (500 km, 10:30 LTAN) is sampled at a fixed cadence over the analysis window and wrapped
// in a tabulated trajectory model. The model's output frame is the lever under test: when it matches the (ITRF) target
// frame the access generator performs zero per-step frame transforms; when it is GCRF the generator must rotate the
// satellite position to ITRF at every step.
// ---------------------------------------------------------------------------------------------------------------------

// Cadence of the tabulated states.
static const Duration TABULATED_STEP = Duration::Seconds(20.0);

// Above-the-horizon elevation bounds. The AER and elevation criteria take their angular bounds in degrees (the
// constructor converts to radians), so these are degrees.
static const MathInterval ABOVE_HORIZON_ELEVATION = MathInterval::Closed(0.0, 90.0);

// SSO satellite states sampled at 20 s over the full (two-week) window plus margin, expressed in GCRF. Built once and
// shared across the tabulated benchmarks; the tabulated model converts them to its output frame at construction.
static const Array<State>& ReferenceSsoStatesGCRF()
{
    static const Array<State> states = []() -> Array<State>
    {
        const Orbit orbit = Orbit::SunSynchronous(
            REFERENCE_START_INSTANT,
            Length::Kilometers(500.0),
            Time::Parse("10:30:00"),
            REFERENCE_ENVIRONMENT.accessCelestialObjectWithName("Earth")
        );

        const Interval samplingInterval = Interval::Closed(
            REFERENCE_START_INSTANT - Duration::Minutes(5.0), REFERENCE_TWO_WEEK_END_INSTANT + Duration::Minutes(5.0)
        );

        return orbit.getStatesAt(samplingInterval.generateGrid(TABULATED_STEP));
    }();

    return states;
}

// A tabulated trajectory whose computed states are expressed in `anOutputFrame`. Linear interpolation is used
// (appropriate for the dense 20 s sampling).
static Trajectory MakeTabulatedTrajectory(const Shared<const Frame>& anOutputFrame)
{
    return Trajectory(Tabulated(ReferenceSsoStatesGCRF(), Interpolator::Type::Linear, anOutputFrame));
}

// A single ground station with a line-of-sight visibility criterion.
static AccessTarget MakeLOSTarget()
{
    const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromLineOfSight(REFERENCE_ENVIRONMENT);

    const LLA groundStationLla = {Angle::Degrees(-45.0), Angle::Degrees(-170.0), Length::Meters(5.0)};
    const Position groundStationPosition = Position::Meters(
        groundStationLla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_), Frame::ITRF()
    );

    return AccessTarget::FromPosition(visibilityCriterion, groundStationPosition);
}

// A spread of ground stations across the globe, all sharing `aVisibilityCriterion`.
static Array<AccessTarget> MakeTargets(const Index aTargetCount, const VisibilityCriterion& aVisibilityCriterion)
{
    Array<AccessTarget> targets = Array<AccessTarget>::Empty();
    targets.reserve(aTargetCount);

    for (Index i = 0; i < aTargetCount; ++i)
    {
        const double latitude_deg = -70.0 + 140.0 * (static_cast<double>(i) / static_cast<double>(aTargetCount));
        const double longitude_deg =
            -180.0 + 360.0 * (static_cast<double>((i * 37) % aTargetCount) / static_cast<double>(aTargetCount));

        const LLA lla = {Angle::Degrees(latitude_deg), Angle::Degrees(longitude_deg), Length::Meters(0.0)};
        const Position position = Position::Meters(
            lla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_), Frame::ITRF()
        );

        targets.add(AccessTarget::FromPosition(aVisibilityCriterion, position));
    }

    return targets;
}

// A spread of ground stations, each with an above-the-horizon elevation criterion.
static Array<AccessTarget> MakeElevationTargets(const Index aTargetCount)
{
    return MakeTargets(aTargetCount, VisibilityCriterion::FromElevationInterval(ABOVE_HORIZON_ELEVATION));
}

// Scenario 1: tabulated model with ITRF output frame, one target, two-week window. The target is in ITRF, so no
// per-step frame transform is required.
static void benchmarkTabulatedItrf1Target2Weeks(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::ITRF());
    static const AccessTarget target = MakeLOSTarget();

    const Generator generator = {REFERENCE_ENVIRONMENT};
    const Interval interval = Interval::Closed(REFERENCE_START_INSTANT, REFERENCE_TWO_WEEK_END_INSTANT);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(generator.computeAccesses(interval, target, trajectory));
    }
}

// Scenario 2: tabulated model with GCRF output frame, one target, two-week window. The generator must rotate the
// satellite position from GCRF to the ITRF target frame at every step.
static void benchmarkTabulatedGcrf1Target2Weeks(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::GCRF());
    static const AccessTarget target = MakeLOSTarget();

    const Generator generator = {REFERENCE_ENVIRONMENT};
    const Interval interval = Interval::Closed(REFERENCE_START_INSTANT, REFERENCE_TWO_WEEK_END_INSTANT);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(generator.computeAccesses(interval, target, trajectory));
    }
}

// Scenario 3: tabulated model with ITRF output frame, 100 targets, one-week window, elevation-interval criterion. The
// satellite state is computed once per step and shared across all targets (vectorized).
static void benchmarkTabulatedItrf100Targets1Week(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::ITRF());
    static const Array<AccessTarget> targets = MakeElevationTargets(100);

    const Generator generator = {REFERENCE_ENVIRONMENT};
    const Interval interval = Interval::Closed(REFERENCE_START_INSTANT, REFERENCE_END_INSTANT);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(generator.computeAccesses(interval, targets, trajectory));
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Bottleneck-isolating scenarios.
//
// The scenarios above measure end-to-end cost. The ones below vary one lever at a time, so that a regression can be
// attributed to a phase of the algorithm rather than just to "access generation got slower". They share a one-day
// window, which keeps the sweeps to seconds rather than minutes per iteration.
//
// The levers, and what each one exposes:
//
//   * Visibility criterion. LineOfSight calls Environment::setInstant() and Environment::intersects() once per
//     (instant, target) pair, which re-poses every celestial body's geometry; the elevation and AER criteria are
//     closed-form vector maths on ITRF coordinates.
//   * Target count. The coarse scan computes the satellite state once per step and shares it across targets, but
//     crossing refinement and time-of-closest-approach run per target, so the spread between these two shows how
//     much of the total is actually shared.
//   * Coarse vs precise. `coarse = true` skips crossing refinement, so the delta is the cost of the root solve.
//   * Target kind. Fixed targets take the vectorized path; trajectory targets take a scalar path that builds an NED
//     frame per sample in Generator::CalculateAer. Compare against the 1-target elevation scenario above.
// ---------------------------------------------------------------------------------------------------------------------

static const Instant REFERENCE_ONE_DAY_END_INSTANT = REFERENCE_START_INSTANT + Duration::Days(1.0);

// The LineOfSight sweep is an order of magnitude more expensive than the rest of the suite; keep it to one iteration.
static const int HEAVY_ITERATIONS = 1;

// Runs `someAccessTargets` against the shared tabulated (ITRF out) trajectory over the one-day window.
static void benchmarkOneDay(benchmark::State& state, const Array<AccessTarget>& someAccessTargets)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::ITRF());

    const Generator generator = {REFERENCE_ENVIRONMENT};
    const Interval interval = Interval::Closed(REFERENCE_START_INSTANT, REFERENCE_ONE_DAY_END_INSTANT);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(generator.computeAccesses(interval, someAccessTargets, trajectory));
    }
}

// Scenario 4: visibility criterion cost, at a fixed target count and window.
static void benchmarkCriterionLineOfSight(benchmark::State& state)
{
    static const Array<AccessTarget> targets =
        MakeTargets(10, VisibilityCriterion::FromLineOfSight(REFERENCE_ENVIRONMENT));

    benchmarkOneDay(state, targets);
}

static void benchmarkCriterionElevation(benchmark::State& state)
{
    static const Array<AccessTarget> targets = MakeElevationTargets(10);

    benchmarkOneDay(state, targets);
}

static void benchmarkCriterionAER(benchmark::State& state)
{
    static const Array<AccessTarget> targets = MakeTargets(
        10,
        VisibilityCriterion::FromAERInterval(
            MathInterval::Closed(0.0, 360.0), ABOVE_HORIZON_ELEVATION, MathInterval::Closed(0.0, 1.0e9)
        )
    );

    benchmarkOneDay(state, targets);
}

// Scenario 5: target-count sweep, with the elevation criterion.
static void benchmarkElevation1Target(benchmark::State& state)
{
    static const Array<AccessTarget> targets = MakeElevationTargets(1);

    benchmarkOneDay(state, targets);
}

static void benchmarkElevation100Targets(benchmark::State& state)
{
    static const Array<AccessTarget> targets = MakeElevationTargets(100);

    benchmarkOneDay(state, targets);
}

// Scenario 6: coarse vs precise, isolating the crossing refinement (root solve).
//
// A 10 s step is used rather than the 1 minute default: in coarse mode an access captured by a single sample yields a
// zero-length interval, and the time-of-closest-approach search cannot converge on one. A 10 s step keeps every pass
// in this scenario at two or more samples.
static void benchmarkRefinement(benchmark::State& state, const bool aCoarseFlag)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::ITRF());
    static const Array<AccessTarget> targets = MakeElevationTargets(10);

    const Generator generator = {REFERENCE_ENVIRONMENT, Duration::Seconds(10.0)};
    const Interval interval = Interval::Closed(REFERENCE_START_INSTANT, REFERENCE_ONE_DAY_END_INSTANT);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(generator.computeAccesses(interval, targets, trajectory, aCoarseFlag));
    }
}

static void benchmarkCoarse(benchmark::State& state)
{
    benchmarkRefinement(state, true);
}

static void benchmarkPrecise(benchmark::State& state)
{
    benchmarkRefinement(state, false);
}

// The same ground station as the 1-target elevation scenario, but declared as a (stationary) trajectory target, which
// sends it down the scalar code path.
static AccessTarget MakeElevationTrajectoryTarget()
{
    const LLA lla = {Angle::Degrees(-70.0), Angle::Degrees(-180.0), Length::Meters(0.0)};
    const Position position =
        Position::Meters(lla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_), Frame::ITRF());

    return AccessTarget::FromTrajectory(
        VisibilityCriterion::FromElevationInterval(ABOVE_HORIZON_ELEVATION), Trajectory::Position(position)
    );
}

// Scenario 7: trajectory target (scalar path) for the same geometry as the 1-target elevation scenario above.
static void benchmarkTrajectoryTarget(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::ITRF());
    static const AccessTarget target = MakeElevationTrajectoryTarget();

    const Generator generator = {REFERENCE_ENVIRONMENT};
    const Interval interval = Interval::Closed(REFERENCE_START_INSTANT, REFERENCE_ONE_DAY_END_INSTANT);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(generator.computeAccesses(interval, target, trajectory));
    }
}

// Register the functions as a benchmark
BENCHMARK(benchmark001)->Name("Access | Ground Station <> TLE")->Iterations(DEFAULT_ITERATIONS);

BENCHMARK(benchmarkTabulatedItrf1Target2Weeks)
    ->Name("Access | Tabulated (ITRF out) | 1 target | 2 weeks")
    ->Iterations(TABULATED_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkTabulatedGcrf1Target2Weeks)
    ->Name("Access | Tabulated (GCRF out) | 1 target | 2 weeks")
    ->Iterations(TABULATED_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkTabulatedItrf100Targets1Week)
    ->Name("Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation")
    ->Iterations(TABULATED_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkCriterionLineOfSight)
    ->Name("Access | Criterion | 10 targets | 1 day | LineOfSight")
    ->Iterations(HEAVY_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkCriterionElevation)
    ->Name("Access | Criterion | 10 targets | 1 day | Elevation")
    ->Iterations(TABULATED_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkCriterionAER)
    ->Name("Access | Criterion | 10 targets | 1 day | AER")
    ->Iterations(TABULATED_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkElevation1Target)
    ->Name("Access | Targets | 1 target | 1 day | Elevation")
    ->Iterations(TABULATED_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkElevation100Targets)
    ->Name("Access | Targets | 100 targets | 1 day | Elevation")
    ->Iterations(TABULATED_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkCoarse)
    ->Name("Access | Refinement | 10 targets | 1 day | 10 s step | coarse")
    ->Iterations(TABULATED_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkPrecise)
    ->Name("Access | Refinement | 10 targets | 1 day | 10 s step | precise")
    ->Iterations(TABULATED_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkTrajectoryTarget)
    ->Name("Access | Target kind | trajectory (scalar) | 1 day | Elevation")
    ->Iterations(TABULATED_ITERATIONS)
    ->Unit(benchmark::kMillisecond);
