/// Apache License 2.0

#include "benchmark/benchmark.h"

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Interval.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriterion.hpp>
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

static const int DEFAULT_ITERATIONS = 5;

static const Environment REFERENCE_ENVIRONMENT = Environment::Default();

static const Instant REFERENCE_START_INSTANT = Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC);

static const Instant REFERENCE_END_INSTANT = Instant::DateTime(DateTime(2023, 1, 8, 0, 0, 0), Scale::UTC);

// Cadence of the tabulated states (the "tabulated trajectory model at 20 second intervals").
static const Duration TABULATED_STEP = Duration::Seconds(20.0);

// Build the satellite states once, sampled at 20 s over the analysis interval (+ margin so the access generator's
// grid and root-finding never query outside the interpolation range). States are returned in GCRF.
static const Array<State>& ReferenceStatesGCRF()
{
    static const Array<State> states = []() -> Array<State>
    {
        const TLE tle = {
            "1 39419U 13066D   18248.44969859 -.00000394  00000-0 -31796-4 0  9997",
            "2 39419  97.6313 314.6863 0012643 218.7350 141.2966 14.93878994260975"
        };
        const SGP4 sgp4 = {tle};
        const Orbit orbit = {sgp4, REFERENCE_ENVIRONMENT.accessCelestialObjectWithName("Earth")};

        const Interval samplingInterval = Interval::Closed(
            REFERENCE_START_INSTANT - Duration::Minutes(5.0), REFERENCE_END_INSTANT + Duration::Minutes(5.0)
        );

        return orbit.getStatesAt(samplingInterval.generateGrid(TABULATED_STEP));
    }();

    return states;
}

// Build a tabulated trajectory whose states are expressed in `anInputFrame` and whose computed states are expressed in
// `anOutputFrame`. Linear interpolation is used (appropriate for dense 20 s data and O(log n) per query).
static Trajectory MakeTabulatedTrajectory(
    const Shared<const Frame>& anInputFrame, const Shared<const Frame>& anOutputFrame
)
{
    Array<State> states = ReferenceStatesGCRF();

    if (anInputFrame != Frame::GCRF())
    {
        for (State& state : states)
        {
            state = state.inFrame(anInputFrame);
        }
    }

    return Trajectory(Tabulated(states, Interpolator::Type::Linear, anOutputFrame));
}

static void RunAccesses(
    benchmark::State& state, const Trajectory& aToTrajectory, const VisibilityCriterion& aVisibilityCriterion
)
{
    const Generator generator = {REFERENCE_ENVIRONMENT};
    const Interval interval = Interval::Closed(REFERENCE_START_INSTANT, REFERENCE_END_INSTANT);

    // Ground Station
    const LLA groundStationLla = {Angle::Degrees(-45.0), Angle::Degrees(-170.0), Length::Meters(5.0)};
    const Position groundStationPosition = Position::Meters(
        groundStationLla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_), Frame::ITRF()
    );
    const AccessTarget groundStationAccessTarget =
        AccessTarget::FromPosition(aVisibilityCriterion, groundStationPosition);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(generator.computeAccesses(interval, groundStationAccessTarget, aToTrajectory));
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// LineOfSight criterion: a realistic, relatively expensive per-step occultation check (Earth intersection). The frame
// transform is one of several per-step costs, so the relative speedup is diluted.
// ---------------------------------------------------------------------------------------------------------------------

// Baseline: states in GCRF, output in GCRF. Per step: one GCRF -> ITRF transform inside the generator.
static void benchmarkLosGcrfInGcrfOut(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::GCRF(), Frame::GCRF());
    RunAccesses(state, trajectory, VisibilityCriterion::FromLineOfSight(REFERENCE_ENVIRONMENT));
}

// New: states in ITRF, output in ITRF. Per step: zero frame transforms (both the model output and the generator
// request are ITRF, so the conversions short-circuit).
static void benchmarkLosItrfInItrfOut(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::ITRF(), Frame::ITRF());
    RunAccesses(state, trajectory, VisibilityCriterion::FromLineOfSight(REFERENCE_ENVIRONMENT));
}

// Legacy behavior with ITRF data: states in ITRF, output forced to GCRF. Per step: two rotation applications (ITRF ->
// GCRF in the model, then GCRF -> ITRF in the generator), but the Earth-orientation transform at each instant is
// constructed once and reused, so this costs about the same as the single-transform baseline.
static void benchmarkLosItrfInGcrfOut(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::ITRF(), Frame::GCRF());
    RunAccesses(state, trajectory, VisibilityCriterion::FromLineOfSight(REFERENCE_ENVIRONMENT));
}

// ---------------------------------------------------------------------------------------------------------------------
// ElevationInterval criterion: a cheap, pure-arithmetic per-step check. With the occultation cost removed, the frame
// transform dominates the per-step cost, so the ITRF -> ITRF speedup is more pronounced.
// ---------------------------------------------------------------------------------------------------------------------

static const VisibilityCriterion ELEVATION_CRITERION =
    VisibilityCriterion::FromElevationInterval(MathInterval::Closed(0.0, 90.0));

static void benchmarkElevationGcrfInGcrfOut(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::GCRF(), Frame::GCRF());
    RunAccesses(state, trajectory, ELEVATION_CRITERION);
}

static void benchmarkElevationItrfInItrfOut(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::ITRF(), Frame::ITRF());
    RunAccesses(state, trajectory, ELEVATION_CRITERION);
}

BENCHMARK(benchmarkLosGcrfInGcrfOut)
    ->Name("Access | Tabulated | LineOfSight | GCRF in -> GCRF out (1 transform/step)")
    ->Iterations(DEFAULT_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkLosItrfInItrfOut)
    ->Name("Access | Tabulated | LineOfSight | ITRF in -> ITRF out (0 transforms/step)")
    ->Iterations(DEFAULT_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkLosItrfInGcrfOut)
    ->Name("Access | Tabulated | LineOfSight | ITRF in -> GCRF out (2 transforms/step)")
    ->Iterations(DEFAULT_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkElevationGcrfInGcrfOut)
    ->Name("Access | Tabulated | Elevation | GCRF in -> GCRF out (1 transform/step)")
    ->Iterations(DEFAULT_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkElevationItrfInItrfOut)
    ->Name("Access | Tabulated | Elevation | ITRF in -> ITRF out (0 transforms/step)")
    ->Iterations(DEFAULT_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

// ---------------------------------------------------------------------------------------------------------------------
// Multi-target AERInterval: this is the only criterion path that exercises computeAer (the per-target SEZ rotation
// vectorized by finding G). ITRF in/out removes the per-step frame transform, and the range bound (0, 1 m) yields zero
// accesses so that no precise-crossing refinement or TCA runs - leaving the per-step AER computation over all targets
// (where G operates) as the dominant cost. Compare these numbers between the current build and HEAD~1 (pre-G) to
// isolate the impact of G at a given target count.
// ---------------------------------------------------------------------------------------------------------------------

static Array<AccessTarget> MakeAerTargets(const Index aTargetCount)
{
    const VisibilityCriterion aerCriterion = VisibilityCriterion::FromAERInterval(
        MathInterval::Closed(0.0, 360.0), MathInterval::Closed(0.0, 90.0), MathInterval::Closed(0.0, 1.0)
    );

    Array<AccessTarget> targets = Array<AccessTarget>::Empty();
    targets.reserve(aTargetCount);

    for (Index i = 0; i < aTargetCount; ++i)
    {
        const double latitude_deg = -80.0 + 160.0 * (static_cast<double>(i) / static_cast<double>(aTargetCount));
        const double longitude_deg =
            -180.0 + 360.0 * (static_cast<double>((i * 7) % aTargetCount) / static_cast<double>(aTargetCount));

        const LLA lla = {Angle::Degrees(latitude_deg), Angle::Degrees(longitude_deg), Length::Meters(0.0)};
        const Position position = Position::Meters(
            lla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_), Frame::ITRF()
        );

        targets.add(AccessTarget::FromPosition(aerCriterion, position));
    }

    return targets;
}

static void RunMultiTargetAccesses(
    benchmark::State& state, const Array<AccessTarget>& someTargets, const Trajectory& aToTrajectory
)
{
    const Generator generator = {REFERENCE_ENVIRONMENT};
    const Interval interval = Interval::Closed(REFERENCE_START_INSTANT, REFERENCE_END_INSTANT);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(generator.computeAccesses(interval, someTargets, aToTrajectory));
    }
}

static void benchmarkAer10Targets(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::ITRF(), Frame::ITRF());
    static const Array<AccessTarget> targets = MakeAerTargets(10);
    RunMultiTargetAccesses(state, targets, trajectory);
}

static void benchmarkAer100Targets(benchmark::State& state)
{
    static const Trajectory trajectory = MakeTabulatedTrajectory(Frame::ITRF(), Frame::ITRF());
    static const Array<AccessTarget> targets = MakeAerTargets(100);
    RunMultiTargetAccesses(state, targets, trajectory);
}

BENCHMARK(benchmarkAer10Targets)
    ->Name("Access | Tabulated | AERInterval | 10 fixed targets (ITRF)")
    ->Iterations(DEFAULT_ITERATIONS)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkAer100Targets)
    ->Name("Access | Tabulated | AERInterval | 100 fixed targets (ITRF)")
    ->Iterations(DEFAULT_ITERATIONS)
    ->Unit(benchmark::kMillisecond);
