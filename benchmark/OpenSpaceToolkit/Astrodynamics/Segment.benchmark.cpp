/// Apache License 2.0
///
/// Segment Performance Benchmark
///
/// Benchmarks four maneuver-segment scenarios under a 20x20 EGM96 gravity model,
/// starting from an SSO satellite at 550 km altitude, and reports:
///   - elapsed wall-clock runtime (per iteration)
///   - number of propagated states
///   - number of maneuvers generated
///   - minimum / maximum step size between consecutive states
///
/// Each scenario runs with a single iteration.

#include <algorithm>
#include <limits>

#include "benchmark/benchmark.h"

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BrouwerLyddaneMeanLongCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::environment::object::Celestial;

namespace CelestialEarth = ostk::physics::environment::object::celestial;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::eventcondition::BrouwerLyddaneMeanLongCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::ConstantThrust;
using ostk::astrodynamics::guidancelaw::QLaw;
using ostk::astrodynamics::trajectory::Segment;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

using BrouwerLyddaneMeanLong = ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using COE = ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using CartesianPosition = ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using CartesianVelocity = ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using NumericalSolver = ostk::astrodynamics::trajectory::state::NumericalSolver;
using flightManeuver = ostk::astrodynamics::flight::Maneuver;

// ---------------------------------------------------------------------------
// Common scenario constants
// ---------------------------------------------------------------------------

static const Instant EPOCH = Instant::DateTime(DateTime(2026, 3, 1, 0, 0, 0), Scale::UTC);

static const double INITIAL_ALTITUDE_M = 550.0e3;
static const double EARTH_EQUATORIAL_RADIUS_M = EarthGravitationalModel::EGM96.equatorialRadius_.inMeters();
static const double INITIAL_SMA_M = EARTH_EQUATORIAL_RADIUS_M + INITIAL_ALTITUDE_M;
static const double TARGET_SMA_580_M = EARTH_EQUATORIAL_RADIUS_M + 580.0e3;

static const double INITIAL_MASS_KG = 200.0;

static const Duration MAX_PROPAGATION_DURATION = Duration::Days(2.0);

static const Duration MIN_MANEUVER_DURATION = Duration::Minutes(5.0);
static const Duration MAX_MANEUVER_DURATION = Duration::Minutes(20.0);
static const Duration MIN_MANEUVER_SEPARATION = Duration::Minutes(30.0);

// ---------------------------------------------------------------------------
// Shared builders
// ---------------------------------------------------------------------------

static State BuildInitialState(const Shared<const Frame>& aFrameSPtr, const Derived& aGravitationalParameter)
{
    const BrouwerLyddaneMeanLong initialCOE = {
        Length::Meters(INITIAL_SMA_M),
        0.001,                 // small eccentricity for QLaw numerical stability
        Angle::Degrees(97.6),  // SSO inclination at 550 km
        Angle::Degrees(0.0),   // RAAN
        Angle::Degrees(0.0),   // AoP
        Angle::Degrees(0.0),   // True anomaly
    };

    const COE::CartesianState cartesianState = initialCOE.getCartesianState(aGravitationalParameter, aFrameSPtr);

    const Shared<CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    }));

    VectorXd coordinates(7);
    coordinates.segment<3>(0) = cartesianState.first.getCoordinates();
    coordinates.segment<3>(3) = cartesianState.second.getCoordinates();
    coordinates(6) = INITIAL_MASS_KG;

    return State(EPOCH, coordinates, aFrameSPtr, brokerSPtr);
}

static Array<Shared<Dynamics>> BuildDynamics()
{
    const Shared<Celestial> earth20x20 = std::make_shared<Celestial>(CelestialEarth::Earth::EGM96(20, 20));
    return {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earth20x20),
    };
}

static NumericalSolver BuildSolver()
{
    return NumericalSolver(
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaDopri5, 5.0, 1.0e-12, 1.0e-12
    );
}

static Segment::ManeuverConstraints BuildQLawConstraints()
{
    return Segment::ManeuverConstraints(
        MIN_MANEUVER_DURATION,
        MAX_MANEUVER_DURATION,
        MIN_MANEUVER_SEPARATION,
        Segment::MaximumManeuverDurationViolationStrategy::Center
    );
}

static Segment::ManeuverConstraints BuildConstantThrustConstraints()
{
    return Segment::ManeuverConstraints(
        MIN_MANEUVER_DURATION,
        MAX_MANEUVER_DURATION,
        MIN_MANEUVER_SEPARATION,
        Segment::MaximumManeuverDurationViolationStrategy::Chunk
    );
}

static SatelliteSystem BuildSatelliteSystem()
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0},
        {ostk::mathematics::object::Vector3d {1.0, 0.0, 0.0},
         ostk::mathematics::object::Vector3d {0.0, 1.0, 0.0},
         ostk::mathematics::object::Vector3d {0.0, 0.0, 1.0}},
        {1.0, 0.0, 0.0}
    ));
    return {
        Mass::Kilograms(150.0),
        satelliteGeometry,
        Matrix3d::Identity(),
        2.2325,
        2.5466188571658566,
        PropulsionSystem(14e-2, 3000.0),
    };
}

// ---------------------------------------------------------------------------
// Helper: run segment, record custom counters
// ---------------------------------------------------------------------------

static Segment::Solution SolveAndRecord(benchmark::State& state, const Segment& aSegment, const State& anInitialState)
{
    Segment::Solution solution = aSegment.solve(anInitialState, MAX_PROPAGATION_DURATION);
    benchmark::DoNotOptimize(solution);

    state.PauseTiming();

    state.counters["states"] = static_cast<double>(solution.states.getSize());

    const Array<flightManeuver> maneuvers = solution.extractManeuvers(Frame::GCRF());
    state.counters["maneuvers"] = static_cast<double>(maneuvers.getSize());

    double minStepSec = std::numeric_limits<double>::infinity();
    double maxStepSec = 0.0;

    if (solution.states.getSize() >= 2)
    {
        for (Size i = 1; i < solution.states.getSize(); ++i)
        {
            const double stepSec =
                (solution.states[i].accessInstant() - solution.states[i - 1].accessInstant()).inSeconds();

            minStepSec = std::min(minStepSec, stepSec);
            maxStepSec = std::max(maxStepSec, stepSec);
        }
    }
    else
    {
        minStepSec = 0.0;
    }

    state.counters["min_step_s"] = minStepSec;
    state.counters["max_step_s"] = maxStepSec;

    state.counters["duration_h"] =
        solution.states.isEmpty() ? 0.0 : static_cast<double>(solution.getPropagationDuration().inHours());

    state.counters["converged"] = solution.conditionIsSatisfied ? 1.0 : 0.0;

    state.ResumeTiming();

    return solution;
}

// ---------------------------------------------------------------------------
// Scenario 1: Constant Thrust (intrack) raising SSO 550 -> 580 km
// ---------------------------------------------------------------------------

static void BM_Segment_ConstantThrust_Intrack_550_to_580(benchmark::State& state)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const Derived mu = EarthGravitationalModel::EGM96.gravitationalParameter_;

    const Array<Shared<Dynamics>> dynamics = BuildDynamics();
    const NumericalSolver solver = BuildSolver();
    const SatelliteSystem satelliteSystem = BuildSatelliteSystem();
    const Segment::ManeuverConstraints constraints = BuildConstantThrustConstraints();

    const Shared<RealCondition> condition =
        std::make_shared<RealCondition>(BrouwerLyddaneMeanLongCondition::SemiMajorAxis(
            RealCondition::Criterion::AnyCrossing, gcrfSPtr, Length::Meters(TARGET_SMA_580_M), mu
        ));

    const Shared<const ConstantThrust> constantThrustSPtr = std::make_shared<ConstantThrust>(ConstantThrust::Intrack());

    const Shared<Thruster> thruster = std::make_shared<Thruster>(satelliteSystem, constantThrustSPtr);

    const Segment segment =
        Segment::Maneuver("ConstantThrust Intrack 550->580", condition, thruster, dynamics, solver, constraints);

    const State initialState = BuildInitialState(gcrfSPtr, mu);

    for (auto _ : state)
    {
        const Segment::Solution solution = SolveAndRecord(state, segment, initialState);

        const BrouwerLyddaneMeanLong finalCOE = BrouwerLyddaneMeanLong::Cartesian(
            {solution.states.accessLast().getPosition(), solution.states.accessLast().getVelocity()}, mu
        );

        state.counters["final_sma_difference"] =
            static_cast<double>(finalCOE.getSemiMajorAxis().inMeters() - TARGET_SMA_580_M);
    }
}

// ---------------------------------------------------------------------------
// Scenario 2: QLaw (analytical), SMA targeting, 550 -> 580 km
// ---------------------------------------------------------------------------

static void BM_Segment_QLaw_Analytical_SMA_550_to_580(benchmark::State& state)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const Derived mu = EarthGravitationalModel::EGM96.gravitationalParameter_;

    const Array<Shared<Dynamics>> dynamics = BuildDynamics();
    const NumericalSolver solver = BuildSolver();
    const SatelliteSystem satelliteSystem = BuildSatelliteSystem();
    const Segment::ManeuverConstraints constraints = BuildQLawConstraints();

    const Shared<RealCondition> condition =
        std::make_shared<RealCondition>(BrouwerLyddaneMeanLongCondition::SemiMajorAxis(
            RealCondition::Criterion::AnyCrossing, gcrfSPtr, Length::Meters(TARGET_SMA_580_M), mu
        ));

    const COE targetCOE = {
        Length::Meters(TARGET_SMA_580_M),
        0.001,
        Angle::Degrees(97.6),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const QLaw::Parameters qlawParameters = {
        {
            {COE::Element::SemiMajorAxis, {1.0, 100.0}},
        },
        3,
        4,
        2,
        0.01,
        100,
        0.0,
        Length::Kilometers(6578.0),
        0.7,
        0.7,
    };

    const Shared<const QLaw> qlawSPtr = std::make_shared<QLaw>(
        targetCOE, mu, qlawParameters, QLaw::COEDomain::BrouwerLyddaneMeanLong, QLaw::GradientStrategy::Analytical
    );

    const Shared<Thruster> thruster = std::make_shared<Thruster>(satelliteSystem, qlawSPtr);

    const Segment segment =
        Segment::Maneuver("QLaw (analytical) SMA 550->580", condition, thruster, dynamics, solver, constraints);

    const State initialState = BuildInitialState(gcrfSPtr, mu);

    for (auto _ : state)
    {
        const Segment::Solution solution = SolveAndRecord(state, segment, initialState);

        const BrouwerLyddaneMeanLong finalCOE = BrouwerLyddaneMeanLong::Cartesian(
            {solution.states.accessLast().getPosition(), solution.states.accessLast().getVelocity()}, mu
        );

        state.counters["final_sma_difference"] =
            static_cast<double>(finalCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters());
    }
}

// ---------------------------------------------------------------------------
// Scenario 3: QLaw (analytical), SMA + Eccentricity + AoP (frozen orbit target)
// ---------------------------------------------------------------------------

static void BM_Segment_QLaw_Analytical_Frozen_550_to_580(benchmark::State& state)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const Derived mu = EarthGravitationalModel::EGM96.gravitationalParameter_;

    const Array<Shared<Dynamics>> dynamics = BuildDynamics();
    const NumericalSolver solver = BuildSolver();
    const SatelliteSystem satelliteSystem = BuildSatelliteSystem();
    const Segment::ManeuverConstraints constraints = BuildQLawConstraints();

    const Shared<RealCondition> condition =
        std::make_shared<RealCondition>(BrouwerLyddaneMeanLongCondition::SemiMajorAxis(
            RealCondition::Criterion::AnyCrossing, gcrfSPtr, Length::Meters(TARGET_SMA_580_M), mu
        ));

    // Frozen SSO target: a = 6958 km, e ~ 0.0011, AoP = 90 deg
    const COE frozenTargetCOE = {
        Length::Meters(TARGET_SMA_580_M),
        0.00011,
        Angle::Degrees(97.81),
        Angle::Degrees(0.0),
        Angle::Degrees(90.0),
        Angle::Degrees(0.0),
    };

    const QLaw::Parameters qlawParameters = {
        {
            {COE::Element::SemiMajorAxis, {1.0, 100.0}},
            {COE::Element::Eccentricity, {1.0, 1.0e-4}},
            {COE::Element::Aop, {1.0, 1.0e-3}},
            {COE::Element::Inclination, {1.0, 1.0e-3}},
        },
        3,
        4,
        2,
        0.01,
        100,
        0.0,
        Length::Kilometers(6578.0),
        0.7,
        0.7,
    };

    const Shared<const QLaw> qlawSPtr = std::make_shared<QLaw>(
        frozenTargetCOE, mu, qlawParameters, QLaw::COEDomain::BrouwerLyddaneMeanLong, QLaw::GradientStrategy::Analytical
    );

    const Shared<Thruster> thruster = std::make_shared<Thruster>(satelliteSystem, qlawSPtr);

    const Segment segment = Segment::Maneuver(
        "QLaw (analytical) SMA+e+AoP frozen 550->580", condition, thruster, dynamics, solver, constraints
    );

    const State initialState = BuildInitialState(gcrfSPtr, mu);

    for (auto _ : state)
    {
        const Segment::Solution solution = SolveAndRecord(state, segment, initialState);

        const BrouwerLyddaneMeanLong finalCOE = BrouwerLyddaneMeanLong::Cartesian(
            {solution.states.accessLast().getPosition(), solution.states.accessLast().getVelocity()}, mu
        );

        state.counters["final_sma_difference"] =
            static_cast<double>(finalCOE.getSemiMajorAxis().inMeters() - frozenTargetCOE.getSemiMajorAxis().inMeters());
        state.counters["final_eccentricity"] =
            static_cast<double>(finalCOE.getEccentricity() - frozenTargetCOE.getEccentricity());
        state.counters["final_aop"] =
            static_cast<double>(finalCOE.getAop().inDegrees() - frozenTargetCOE.getAop().inDegrees());
        state.counters["final_inclination"] =
            static_cast<double>(finalCOE.getInclination().inDegrees() - frozenTargetCOE.getInclination().inDegrees());
    }
}

// ---------------------------------------------------------------------------
// Scenario 4: Constant Thrust (intrack) with 40 min/day duty cycle, 550 -> 580 km
// ---------------------------------------------------------------------------

static void BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580(benchmark::State& state)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const Derived mu = EarthGravitationalModel::EGM96.gravitationalParameter_;

    const Array<Shared<Dynamics>> dynamics = BuildDynamics();
    const NumericalSolver solver = BuildSolver();
    const SatelliteSystem satelliteSystem = BuildSatelliteSystem();
    const Segment::ManeuverConstraints constraints = Segment::ManeuverConstraints(
        MIN_MANEUVER_DURATION,
        MAX_MANEUVER_DURATION,
        MIN_MANEUVER_SEPARATION,
        Segment::MaximumManeuverDurationViolationStrategy::Chunk,
        Pair<Duration, Duration>(Duration::Minutes(40.0), Duration::Days(1.0))
    );

    const Shared<RealCondition> condition =
        std::make_shared<RealCondition>(BrouwerLyddaneMeanLongCondition::SemiMajorAxis(
            RealCondition::Criterion::AnyCrossing, gcrfSPtr, Length::Meters(TARGET_SMA_580_M), mu
        ));

    const Shared<const ConstantThrust> constantThrustSPtr = std::make_shared<ConstantThrust>(ConstantThrust::Intrack());

    const Shared<Thruster> thruster = std::make_shared<Thruster>(satelliteSystem, constantThrustSPtr);

    const Segment segment = Segment::Maneuver(
        "ConstantThrust Intrack 40min/day 550->560", condition, thruster, dynamics, solver, constraints
    );

    const State initialState = BuildInitialState(gcrfSPtr, mu);

    for (auto _ : state)
    {
        const Segment::Solution solution = SolveAndRecord(state, segment, initialState);

        const BrouwerLyddaneMeanLong finalCOE = BrouwerLyddaneMeanLong::Cartesian(
            {solution.states.accessLast().getPosition(), solution.states.accessLast().getVelocity()}, mu
        );

        state.counters["final_sma_difference"] =
            static_cast<double>(finalCOE.getSemiMajorAxis().inMeters() - TARGET_SMA_580_M);
    }
}

// ---------------------------------------------------------------------------
// Benchmark registration — each scenario runs a single iteration
// ---------------------------------------------------------------------------

BENCHMARK(BM_Segment_ConstantThrust_Intrack_550_to_580)->Iterations(1)->Unit(benchmark::kSecond);
BENCHMARK(BM_Segment_QLaw_Analytical_SMA_550_to_580)->Iterations(1)->Unit(benchmark::kSecond);
BENCHMARK(BM_Segment_QLaw_Analytical_Frozen_550_to_580)->Iterations(1)->Unit(benchmark::kSecond);
BENCHMARK(BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580)->Iterations(1)->Unit(benchmark::kSecond);
