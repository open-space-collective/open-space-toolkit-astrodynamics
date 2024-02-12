/// Apache License 2.0

#include "benchmark/benchmark.h"

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::Propagator;
using ostk::astro::Dynamics;
using ostk::astro::dynamics::PositionDerivative;
using ostk::astro::dynamics::CentralBodyGravity;

static const int DEFAULT_ITERATIONS = 10;

static const Instant REFERENCE_START_INSTANT = Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC);

static const Instant REFERENCE_END_INSTANT = Instant::DateTime(DateTime(2023, 1, 2, 0, 0, 0), Scale::UTC);

// Circular orbit, 550km, i=98deg, at X axis
static const State REFERENCE_INITIAL_STATE = {
    REFERENCE_START_INSTANT,
    Position::Meters({6928030.022926601, -35311.5927995581, -15342.216614716504}, Frame::GCRF()),
    Velocity::MetersPerSecond({11.25440758409726, -1055.4321962342744, 7511.291781873726}, Frame::GCRF()),
};

static const NumericalSolver REFERENCE_SOLVER = {
    NumericalSolver::LogType::NoLog,
    NumericalSolver::StepperType::RungeKutta4,
    5.0,
    1.0e-9,
    1.0e-9,
};

static void calculateStateAt(benchmark::State &state, const Array<Shared<Dynamics>> &dynamics)
{
    const Propagator propagator = {
        REFERENCE_SOLVER,
        dynamics,
    };

    state.ResumeTiming();
    benchmark::DoNotOptimize(propagator.calculateStateAt(REFERENCE_INITIAL_STATE, REFERENCE_END_INSTANT));
}

static void benchmark001(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::Spherical());
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(), std::make_shared<CentralBodyGravity>(earth)
        };

        calculateStateAt(state, dynamics);
    }
}

static void benchmark002(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM84(100, 100));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(), std::make_shared<CentralBodyGravity>(earth)
        };

        calculateStateAt(state, dynamics);
    }
}

static void benchmark003(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM96(100, 100));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(), std::make_shared<CentralBodyGravity>(earth)
        };

        calculateStateAt(state, dynamics);
    }
}

static void benchmark004(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM2008(100, 100));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(), std::make_shared<CentralBodyGravity>(earth)
        };

        calculateStateAt(state, dynamics);
    }
}

// Register the functions as a benchmark
BENCHMARK(benchmark001)->Name("Propagation | Numerical | Spherical")->Iterations(DEFAULT_ITERATIONS);
BENCHMARK(benchmark002)->Name("Propagation | Numerical | EGM1984 {100, 100}")->Iterations(DEFAULT_ITERATIONS);
BENCHMARK(benchmark003)->Name("Propagation | Numerical | EGM1996 {100, 100}")->Iterations(DEFAULT_ITERATIONS);
BENCHMARK(benchmark004)->Name("Propagation | Numerical | EGM2008 {100, 100}")->Iterations(DEFAULT_ITERATIONS);
