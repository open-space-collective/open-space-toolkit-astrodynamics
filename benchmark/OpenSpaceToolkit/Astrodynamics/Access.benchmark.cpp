/// Apache License 2.0

#include "benchmark/benchmark.h"

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Interval.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;

using ostk::physics::Environment;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::spherical::LLA;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::time::Interval;
using ostk::physics::units::Angle;
using ostk::physics::units::Length;

using ostk::astro::Access;
using ostk::astro::Trajectory;
using ostk::astro::access::Generator;
using ostk::astro::trajectory::Orbit;
using ostk::astro::trajectory::orbit::models::SGP4;
using ostk::astro::trajectory::orbit::models::sgp4::TLE;

static const int DEFAULT_ITERATIONS = 10;

static const Environment REFERENCE_ENVIRONMENT = Environment::Default();

static const Instant REFERENCE_START_INSTANT = Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC);

static const Instant REFERENCE_END_INSTANT = Instant::DateTime(DateTime(2023, 1, 8, 0, 0, 0), Scale::UTC);

static void computeAccesses(benchmark::State& state, const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory)
{
    const Generator generator = {REFERENCE_ENVIRONMENT};
    const Interval interval = Interval::Closed(REFERENCE_START_INSTANT, REFERENCE_END_INSTANT);

    state.ResumeTiming();
    benchmark::DoNotOptimize(generator.computeAccesses(interval, aFromTrajectory, aToTrajectory));
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
        const Trajectory groundStationTrajectory = Trajectory::Position(groundStationPosition);

        // Satellite
        const TLE tle = {
            "1 39419U 13066D   18248.44969859 -.00000394  00000-0 -31796-4 0  9997",
            "2 39419  97.6313 314.6863 0012643 218.7350 141.2966 14.93878994260975"
        };
        const SGP4 sgp4 = {tle};
        const Orbit satelliteOrbit = {sgp4, REFERENCE_ENVIRONMENT.accessCelestialObjectWithName("Earth")};

        computeAccesses(state, groundStationTrajectory, satelliteOrbit);
    }
}

// Register the functions as a benchmark
BENCHMARK(benchmark001)->Name("Access | Ground Station <> TLE")->Iterations(DEFAULT_ITERATIONS);
