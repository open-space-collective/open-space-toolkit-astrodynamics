/// Apache License 2.0
///
/// Sequence Performance Benchmark
///
/// Benchmarks a duty-cycle constrained, Q-Law driven orbit raise executed through the
/// Sequence / Segment machinery:
///
///   - 540 km -> 550 km sun-synchronous orbit raise, 10:00 LTAN
///   - Q-Law targeting semi-major axis and argument of periapsis (90 deg)
///   - effectivity thresholds of 0.6 (absolute and relative)
///   - 15.1 mN thruster, 1016 s specific impulse
///   - maneuver constraints: 10 min minimum, 40 min maximum, 30 min minimum separation,
///     40 min of thrust per 95 min duty cycle window
///   - 2 week maximum propagation duration
///
/// and reports, in addition to the wall-clock runtime:
///   - number of propagated states
///   - number of maneuvers generated
///   - minimum / maximum step size between consecutive states
///   - propagation duration
///   - convergence status
///
/// The same scenario is registered both through Sequence::solve and through
/// Segment::solve, so that the Sequence-level overhead can be isolated. A Q-Law
/// guidance law micro-benchmark is registered as well, to relate the end-to-end
/// runtime to the cost of a single guidance law evaluation.
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
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
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
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/OrbitalElementCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>
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
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::environment::object::Celestial;

namespace CelestialEarth = ostk::physics::environment::object::celestial;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using SunCelestial = ostk::physics::environment::object::celestial::Sun;

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
using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::eventcondition::OrbitalElementCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::QLaw;
using ostk::astrodynamics::RootSolver;
using ostk::astrodynamics::trajectory::Segment;
using ostk::astrodynamics::trajectory::Sequence;
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
// Scenario constants
// ---------------------------------------------------------------------------

static const Instant EPOCH = Instant::DateTime(DateTime(2026, 3, 1, 0, 0, 0), Scale::UTC);

static const double EARTH_EQUATORIAL_RADIUS_M = EarthGravitationalModel::EGM96.equatorialRadius_.inMeters();
static const double EARTH_MU = static_cast<double>(
    EarthGravitationalModel::EGM96.gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared())
);
static const double EARTH_J2 = static_cast<double>(EarthGravitationalModel::EGM96.J2_);

static const double INITIAL_SMA_M = EARTH_EQUATORIAL_RADIUS_M + 540.0e3;
static const double TARGET_SMA_M = EARTH_EQUATORIAL_RADIUS_M + 550.0e3;

static const double INITIAL_ECCENTRICITY = 0.001;
static const double TARGET_AOP_DEG = 90.0;
static const double LTAN_HOURS = 10.0;

static const double INITIAL_MASS_KG = 200.0;
static const double DRY_MASS_KG = 150.0;

static const double THRUST_N = 15.1e-3;
static const double SPECIFIC_IMPULSE_S = 1016.0;

static const double EFFECTIVITY_THRESHOLD = 0.6;

static const Duration MAX_PROPAGATION_DURATION = Duration::Weeks(2.0);

static const Duration MIN_MANEUVER_DURATION = Duration::Minutes(10.0);
static const Duration MAX_MANEUVER_DURATION = Duration::Minutes(40.0);
static const Duration MIN_MANEUVER_SEPARATION = Duration::Minutes(30.0);
static const Pair<Duration, Duration> MAX_DUTY_CYCLE = {Duration::Minutes(40.0), Duration::Minutes(95.0)};

// ---------------------------------------------------------------------------
// Shared builders
// ---------------------------------------------------------------------------

/// @brief Sun-synchronous inclination for a given semi-major axis and eccentricity.
static double ComputeSunSynchronousInclinationRad(const double& aSemiMajorAxis, const double& anEccentricity)
{
    // Nodal precession rate matching the mean motion of the Sun (rad/s)
    static const double sunNodalPrecessionRate = 2.0 * M_PI / (365.2421897 * 86400.0);

    const double meanMotion = std::sqrt(EARTH_MU / std::pow(aSemiMajorAxis, 3));
    const double oneMinusESquared = 1.0 - anEccentricity * anEccentricity;

    const double cosInclination =
        -(2.0 * sunNodalPrecessionRate * std::pow(aSemiMajorAxis, 2) * oneMinusESquared * oneMinusESquared) /
        (3.0 * meanMotion * std::pow(EARTH_EQUATORIAL_RADIUS_M, 2) * EARTH_J2);

    return std::acos(cosInclination);
}

/// @brief RAAN placing the ascending node at the requested local time of ascending node.
static double ComputeRaanForLTANRad(const Instant& anInstant, const double& anLTANInHours)
{
    const Celestial sun = SunCelestial::Default();
    const Vector3d sunPosition = sun.getPositionIn(Frame::GCRF(), anInstant).getCoordinates();

    const double sunRightAscension = std::atan2(sunPosition.y(), sunPosition.x());

    return sunRightAscension + (anLTANInHours - 12.0) * (M_PI / 12.0);
}

static State BuildInitialState(const Shared<const Frame>& aFrameSPtr, const Derived& aGravitationalParameter)
{
    const BrouwerLyddaneMeanLong initialCOE = {
        Length::Meters(INITIAL_SMA_M),
        INITIAL_ECCENTRICITY,
        Angle::Radians(ComputeSunSynchronousInclinationRad(INITIAL_SMA_M, INITIAL_ECCENTRICITY)),
        Angle::Radians(ComputeRaanForLTANRad(EPOCH, LTAN_HOURS)),
        Angle::Degrees(0.0),  // AoP
        Angle::Degrees(0.0),  // True anomaly
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
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-12,
        1.0e-12,
        RootSolver::Default()
    );
}

static Segment::ManeuverConstraints BuildConstraints()
{
    return Segment::ManeuverConstraints(
        MIN_MANEUVER_DURATION,
        MAX_MANEUVER_DURATION,
        MIN_MANEUVER_SEPARATION,
        Segment::MaximumManeuverDurationViolationStrategy::Chunk,
        MAX_DUTY_CYCLE
    );
}

static SatelliteSystem BuildSatelliteSystem()
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 0.0, 0.0}
    ));

    return {
        Mass::Kilograms(DRY_MASS_KG),
        satelliteGeometry,
        Matrix3d::Identity(),
        2.2325,
        2.5466188571658566,
        PropulsionSystem(THRUST_N, SPECIFIC_IMPULSE_S),
    };
}

static Shared<const QLaw> BuildQLaw(const Derived& aGravitationalParameter)
{
    const COE targetCOE = {
        Length::Meters(TARGET_SMA_M),
        INITIAL_ECCENTRICITY,
        Angle::Radians(ComputeSunSynchronousInclinationRad(TARGET_SMA_M, INITIAL_ECCENTRICITY)),
        Angle::Radians(ComputeRaanForLTANRad(EPOCH, LTAN_HOURS)),
        Angle::Degrees(TARGET_AOP_DEG),
        Angle::Degrees(0.0),
    };

    const QLaw::Parameters parameters = {
        {
            {COE::Element::SemiMajorAxis, {1.0, 100.0}},
            {COE::Element::Aop, {1.0, 1.0e-3}},
        },
        3,
        4,
        2,
        0.01,
        100,
        0.0,
        Length::Kilometers(6578.0),
        EFFECTIVITY_THRESHOLD,  // absolute effectivity threshold
        EFFECTIVITY_THRESHOLD,  // relative effectivity threshold
    };

    return std::make_shared<QLaw>(
        targetCOE,
        aGravitationalParameter,
        parameters,
        QLaw::COEDomain::BrouwerLyddaneMeanLong,
        QLaw::GradientStrategy::Analytical
    );
}

static Shared<RealCondition> BuildTargetCondition(
    const Shared<const Frame>& aFrameSPtr, const Derived& aGravitationalParameter
)
{
    return std::make_shared<RealCondition>(OrbitalElementCondition::SemiMajorAxis(
        OrbitalElementCondition::Theory::BrouwerLyddaneMeanLong,
        RealCondition::Criterion::AnyCrossing,
        aFrameSPtr,
        Length::Meters(TARGET_SMA_M),
        aGravitationalParameter
    ));
}

static Segment BuildSegment(const Shared<const Frame>& aFrameSPtr, const Derived& aGravitationalParameter)
{
    return Segment::Maneuver(
        "QLaw SSO 540->550 km, SMA + AoP",
        BuildTargetCondition(aFrameSPtr, aGravitationalParameter),
        std::make_shared<Thruster>(BuildSatelliteSystem(), BuildQLaw(aGravitationalParameter)),
        BuildDynamics(),
        BuildSolver(),
        BuildConstraints()
    );
}

// ---------------------------------------------------------------------------
// Helper: record custom counters for a set of states
// ---------------------------------------------------------------------------

static void RecordCounters(
    benchmark::State& state,
    const Array<State>& aStateArray,
    const Size& aManeuverCount,
    const bool& isComplete,
    const Derived& aGravitationalParameter
)
{
    state.counters["states"] = static_cast<double>(aStateArray.getSize());
    state.counters["maneuvers"] = static_cast<double>(aManeuverCount);

    double minStepSec = std::numeric_limits<double>::infinity();
    double maxStepSec = 0.0;

    for (Size i = 1; i < aStateArray.getSize(); ++i)
    {
        const double stepSec =
            static_cast<double>((aStateArray[i].accessInstant() - aStateArray[i - 1].accessInstant()).inSeconds());

        minStepSec = std::min(minStepSec, stepSec);
        maxStepSec = std::max(maxStepSec, stepSec);
    }

    state.counters["min_step_s"] = (aStateArray.getSize() >= 2) ? minStepSec : 0.0;
    state.counters["max_step_s"] = maxStepSec;

    state.counters["duration_h"] =
        aStateArray.isEmpty()
            ? 0.0
            : static_cast<double>(
                  (aStateArray.accessLast().accessInstant() - aStateArray.accessFirst().accessInstant()).inHours()
              );

    state.counters["converged"] = isComplete ? 1.0 : 0.0;

    if (!aStateArray.isEmpty())
    {
        const State& finalState = aStateArray.accessLast();

        const BrouwerLyddaneMeanLong finalCOE = BrouwerLyddaneMeanLong::Cartesian(
            {finalState.getPosition(), finalState.getVelocity()}, aGravitationalParameter
        );

        state.counters["final_sma_difference_m"] =
            static_cast<double>(finalCOE.getSemiMajorAxis().inMeters() - TARGET_SMA_M);
        state.counters["final_aop_difference_deg"] =
            static_cast<double>(finalCOE.getAop().inDegrees() - TARGET_AOP_DEG);
        state.counters["propellant_used_kg"] =
            INITIAL_MASS_KG - static_cast<double>(finalState.extractCoordinate(CoordinateSubset::Mass())(0));
    }
}

// ---------------------------------------------------------------------------
// Scenario 1: Sequence::solve — Q-Law SSO 540 -> 550 km, SMA + AoP, duty cycled
// ---------------------------------------------------------------------------

static void BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP(benchmark::State& state)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const Derived mu = EarthGravitationalModel::EGM96.gravitationalParameter_;

    const Sequence sequence = {
        {BuildSegment(gcrfSPtr, mu)},
        BuildSolver(),
        BuildDynamics(),
        MAX_PROPAGATION_DURATION,
    };

    const State initialState = BuildInitialState(gcrfSPtr, mu);

    for (auto _ : state)
    {
        const Sequence::Solution solution = sequence.solve(initialState);
        benchmark::DoNotOptimize(solution);

        state.PauseTiming();
        RecordCounters(
            state, solution.getStates(), solution.extractManeuvers(gcrfSPtr).getSize(), solution.executionIsComplete, mu
        );
        state.ResumeTiming();
    }
}

// ---------------------------------------------------------------------------
// Scenario 2: Segment::solve — identical scenario, without the Sequence layer
// ---------------------------------------------------------------------------

static void BM_Segment_QLaw_SSO_540_to_550_SMA_AoP(benchmark::State& state)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const Derived mu = EarthGravitationalModel::EGM96.gravitationalParameter_;

    const Segment segment = BuildSegment(gcrfSPtr, mu);

    const State initialState = BuildInitialState(gcrfSPtr, mu);

    for (auto _ : state)
    {
        const Segment::Solution solution = segment.solve(initialState, MAX_PROPAGATION_DURATION);
        benchmark::DoNotOptimize(solution);

        state.PauseTiming();
        RecordCounters(state, solution.states, solution.maneuverIntervals.getSize(), solution.conditionIsSatisfied, mu);
        state.ResumeTiming();
    }
}

// ---------------------------------------------------------------------------
// Scenario 2b: Segment::solve over the full 2 week horizon
//
// Identical scenario, but terminated on the end of the 2 week window rather than on
// the target semi-major axis, so that the cost of the full planning horizon is measured
// (the semi-major axis target is reached well before the window closes).
// ---------------------------------------------------------------------------

static void BM_Segment_QLaw_SSO_540_to_550_FullHorizon(benchmark::State& state)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const Derived mu = EarthGravitationalModel::EGM96.gravitationalParameter_;

    const Segment segment = Segment::Maneuver(
        "QLaw SSO 540->550 km, SMA + AoP, full 2 week horizon",
        std::make_shared<InstantCondition>(
            InstantCondition::Criterion::StrictlyPositive, EPOCH + MAX_PROPAGATION_DURATION
        ),
        std::make_shared<Thruster>(BuildSatelliteSystem(), BuildQLaw(mu)),
        BuildDynamics(),
        BuildSolver(),
        BuildConstraints()
    );

    const State initialState = BuildInitialState(gcrfSPtr, mu);

    for (auto _ : state)
    {
        const Segment::Solution solution = segment.solve(initialState, MAX_PROPAGATION_DURATION);
        benchmark::DoNotOptimize(solution);

        state.PauseTiming();
        RecordCounters(state, solution.states, solution.maneuverIntervals.getSize(), solution.conditionIsSatisfied, mu);
        state.ResumeTiming();
    }
}

// ---------------------------------------------------------------------------
// Scenario 3: Segment::Solution::extractManeuvers — post-processing only
// ---------------------------------------------------------------------------

static void BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers(benchmark::State& state)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const Derived mu = EarthGravitationalModel::EGM96.gravitationalParameter_;

    const Segment segment = BuildSegment(gcrfSPtr, mu);

    const Segment::Solution solution = segment.solve(BuildInitialState(gcrfSPtr, mu), MAX_PROPAGATION_DURATION);

    state.counters["states"] = static_cast<double>(solution.states.getSize());
    state.counters["maneuver_intervals"] = static_cast<double>(solution.maneuverIntervals.getSize());

    for (auto _ : state)
    {
        const Array<flightManeuver> maneuvers = solution.extractManeuvers(gcrfSPtr);
        benchmark::DoNotOptimize(maneuvers);
    }
}

// ---------------------------------------------------------------------------
// Scenario 4: Q-Law guidance law evaluation — single call cost
// ---------------------------------------------------------------------------

static void BM_QLaw_CalculateThrustAccelerationAt(benchmark::State& state)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const Derived mu = EarthGravitationalModel::EGM96.gravitationalParameter_;

    const Shared<const QLaw> qlawSPtr = BuildQLaw(mu);

    const State initialState = BuildInitialState(gcrfSPtr, mu);

    const Vector3d positionCoordinates = initialState.getPosition().inMeters().getCoordinates();
    const Vector3d velocityCoordinates =
        initialState.getVelocity().inUnit(ostk::physics::coordinate::Velocity::Unit::MeterPerSecond).getCoordinates();

    const double thrustAcceleration = THRUST_N / INITIAL_MASS_KG;

    for (auto _ : state)
    {
        const Vector3d acceleration = qlawSPtr->calculateThrustAccelerationAt(
            EPOCH, positionCoordinates, velocityCoordinates, thrustAcceleration, gcrfSPtr
        );
        benchmark::DoNotOptimize(acceleration);
    }
}

// ---------------------------------------------------------------------------
// Benchmark registration
// ---------------------------------------------------------------------------

BENCHMARK(BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP)->Iterations(1)->Unit(benchmark::kSecond);
BENCHMARK(BM_Segment_QLaw_SSO_540_to_550_SMA_AoP)->Iterations(1)->Unit(benchmark::kSecond);
BENCHMARK(BM_Segment_QLaw_SSO_540_to_550_FullHorizon)->Iterations(1)->Unit(benchmark::kSecond);
BENCHMARK(BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers)->Iterations(1)->Unit(benchmark::kSecond);
BENCHMARK(BM_QLaw_CalculateThrustAccelerationAt)->Unit(benchmark::kMicrosecond);
