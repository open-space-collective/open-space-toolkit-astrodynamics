/// Apache License 2.0
///
/// ROE Targeting Benchmark
///
/// Benchmarks a maneuvering segment steered by the ROETargeting guidance law under a 20x20 EGM96 gravity model, on the
/// 550 km SSO of the Segment benchmark. The spacecraft starts about 5 km behind (T), 2 km below (R) and 1 km away (N)
/// from a reference spacecraft and must converge onto it, without radial thrust, then with radial thrust.
/// Operationally, a maneuver lasts at most 10 minutes, starts at least 26 minutes after the end of the previous one,
/// and at most 2 maneuvers start within any orbital period: the Segment enforces these constraints. The guidance law,
/// without memory, decides when within the orbit to start a burn, and commits to it: the Segment then follows a
/// constant thrust along the planned direction for the planned duration. Reports:
///   - elapsed wall-clock runtime (per iteration)
///   - number of propagated states and maneuvers
///   - longest maneuver, shortest time between the end of a maneuver and the start of the next one, and largest number
///     of maneuvers starting within an orbital period
///   - time to converge, convergence status and delta-v
///   - initial and final RTN offsets, and final ROE, with respect to the reference
///
/// Each scenario runs with a single iteration.

#include <algorithm>
#include <cmath>
#include <limits>

#include "benchmark/benchmark.h"

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ROETargeting.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Vector6d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::environment::object::Celestial;

namespace CelestialEarth = ostk::physics::environment::object::celestial;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::ROETargeting;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::orbit::model::Tabulated;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::Segment;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

using BrouwerLyddaneMeanLong = ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using CartesianPosition = ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using CartesianVelocity = ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using COE = ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using NumericalSolver = ostk::astrodynamics::trajectory::state::NumericalSolver;

// ---------------------------------------------------------------------------
// Common scenario constants
// ---------------------------------------------------------------------------

static const Instant EPOCH = Instant::DateTime(DateTime(2026, 3, 1, 0, 0, 0), Scale::UTC);

static const double INITIAL_ALTITUDE_M = 550.0e3;
static const double EARTH_EQUATORIAL_RADIUS_M = EarthGravitationalModel::EGM96.equatorialRadius_.inMeters();
static const double REFERENCE_SMA_M = EARTH_EQUATORIAL_RADIUS_M + INITIAL_ALTITUDE_M;
static const double REFERENCE_ECCENTRICITY = 0.001;
static const Angle REFERENCE_INCLINATION = Angle::Degrees(97.6);  // SSO inclination at 550 km

static const double INITIAL_MASS_KG = 200.0;
static const double THRUST_N = 14e-2;
static const double SPECIFIC_IMPULSE_S = 3000.0;

// ROE of the spacecraft with respect to the reference at the epoch, [δa, δλ, δex, δey, δix, δiy] scaled by the
// reference semi-major axis. The epoch is at argument of latitude 0, where the RTN offset is
// (δa - δex, δλ - 2 δey, -δiy) = (-2, -5, +1) km. A zero δa keeps the relative orbit drift-free: starting 2 km below on
// a lower orbit instead would drift along-track by about 280 km per day.
static const Vector6d INITIAL_ROE_M = (Vector6d() << 0.0, -5000.0, 2000.0, 0.0, 0.0, -1000.0).finished();

// Convergence is declared once every ROE component is within its tolerance
static const Vector6d ROE_TOLERANCE_M = (Vector6d() << 10.0, 100.0, 10.0, 10.0, 10.0, 10.0).finished();

static const Duration MAX_PROPAGATION_DURATION = Duration::Days(3.0);

// Keplerian period of the reference
static const Duration ORBITAL_PERIOD = Duration::Seconds(
    2.0 * M_PI *
    std::sqrt(
        std::pow(REFERENCE_SMA_M, 3) /
        EarthGravitationalModel::EGM96.gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared())
    )
);

// Operational constraints, enforced by the Segment: a maneuver lasts at most 10 minutes, starts at least 26 minutes
// after the end of the previous one, and at most 2 maneuvers start within any orbital period
static const Duration MAX_MANEUVER_DURATION = Duration::Minutes(10.0);
static const Duration MIN_MANEUVER_SEPARATION = Duration::Minutes(26.0);
static const Size MAX_MANEUVERS_PER_ORBIT = 2;

// The predictions place the burns at least half an orbit apart: tangential burns half an orbit apart correct the
// eccentricity vector without changing the semi-major axis
static const Duration BURN_SPACING = ORBITAL_PERIOD / double(MAX_MANEUVERS_PER_ORBIT);

static const Duration MIN_BURN_DURATION = Duration::Seconds(5.0);  // Segment skips maneuvers shorter than 5 s

// The guidance law burns when burning now achieves at least 99% of the improvement over not burning of the best start
// time of the next orbit (searched every 10 degrees of argument of latitude, then refined): as it commits to the whole
// burn, it can wait for the best start time
static const Real EFFECTIVITY_THRESHOLD = 0.99;
static const Size BURN_START_CANDIDATE_COUNT = 36;

// Cubic spline interpolation error at 30 s: 2 cm, 2e-5 m/s
static const Duration REFERENCE_TABULATION_STEP = Duration::Seconds(30.0);

// ---------------------------------------------------------------------------
// Shared builders
// ---------------------------------------------------------------------------

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
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-12, 1.0e-12
    );
}

static SatelliteSystem BuildSatelliteSystem()
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 0.0, 0.0}
    ));
    return {
        Mass::Kilograms(150.0),
        satelliteGeometry,
        Matrix3d::Identity(),
        2.2325,
        2.5466188571658566,
        PropulsionSystem(THRUST_N, SPECIFIC_IMPULSE_S),
    };
}

// Natural (thrust-free) motion of the reference in the same gravity field, tabulated
static Trajectory BuildReferenceTrajectory(
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter
)
{
    const BrouwerLyddaneMeanLong referenceCOE = {
        Length::Meters(REFERENCE_SMA_M),
        REFERENCE_ECCENTRICITY,
        REFERENCE_INCLINATION,
        Angle::Degrees(0.0),  // RAAN
        Angle::Degrees(0.0),  // AoP
        Angle::Degrees(0.0),  // Mean anomaly
    };

    const COE::CartesianState cartesianState = referenceCOE.getCartesianState(aGravitationalParameter, aFrameSPtr);

    const Array<Instant> instants = Interval::Closed(EPOCH, EPOCH + MAX_PROPAGATION_DURATION + Duration::Hours(1.0))
                                        .generateGrid(REFERENCE_TABULATION_STEP);

    const Array<State> states =
        Propagator(aNumericalSolver, aDynamicsArray)
            .calculateStatesAt(State(EPOCH, cartesianState.first, cartesianState.second), instants);

    return Trajectory(Tabulated(states, 1, Interpolator::Type::CubicSpline));
}

// Spacecraft whose mean elements are offset from the ones of the reference by INITIAL_ROE_M
static State BuildInitialState(const Shared<const Frame>& aFrameSPtr, const Derived& aGravitationalParameter)
{
    const Vector6d roe = INITIAL_ROE_M / REFERENCE_SMA_M;
    const double inclination = REFERENCE_INCLINATION.inRadians();

    const double eccentricityX = REFERENCE_ECCENTRICITY + roe[2];
    const double eccentricityY = roe[3];
    const double aop = std::atan2(eccentricityY, eccentricityX);
    const double raan = roe[5] / std::sin(inclination);
    const double meanArgumentOfLatitude = roe[1] - (raan * std::cos(inclination));

    const BrouwerLyddaneMeanLong initialCOE = {
        Length::Meters(REFERENCE_SMA_M * (1.0 + roe[0])),
        std::hypot(eccentricityX, eccentricityY),
        Angle::Radians(inclination + roe[4]),
        Angle::Radians(raan),
        Angle::Radians(aop),
        Angle::Radians(meanArgumentOfLatitude - aop),
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

// Position of a state in the RTN frame of a reference state, in m
static Vector3d ComputeRTNOffset(const State& aState, const State& aReferenceState)
{
    const Vector3d referencePosition = aReferenceState.getPosition().getCoordinates();
    const Vector3d referenceVelocity = aReferenceState.getVelocity().getCoordinates();

    const Vector3d radial = referencePosition.normalized();
    const Vector3d normal = referencePosition.cross(referenceVelocity).normalized();
    const Vector3d tangential = normal.cross(radial);

    const Vector3d offset = aState.getPosition().getCoordinates() - referencePosition;

    return {offset.dot(radial), offset.dot(tangential), offset.dot(normal)};
}

// ---------------------------------------------------------------------------
// Helper: record custom counters
// ---------------------------------------------------------------------------

static void RecordCounters(
    benchmark::State& state,
    const Segment::Solution& aSolution,
    const ROETargeting& aGuidanceLaw,
    const Trajectory& aReferenceTrajectory
)
{
    state.counters["states"] = static_cast<double>(aSolution.states.getSize());

    double minStepSec = std::numeric_limits<double>::infinity();
    double maxStepSec = 0.0;

    for (Size i = 1; i < aSolution.states.getSize(); ++i)
    {
        const double stepSec =
            (aSolution.states[i].accessInstant() - aSolution.states[i - 1].accessInstant()).inSeconds();

        minStepSec = std::min(minStepSec, stepSec);
        maxStepSec = std::max(maxStepSec, stepSec);
    }

    state.counters["min_step_s"] = (aSolution.states.getSize() >= 2) ? minStepSec : 0.0;
    state.counters["max_step_s"] = maxStepSec;

    // Maneuver constraints: each maneuver lasts at most 10 minutes, starts at least 26 minutes after the end of the
    // previous one, and at most 2 maneuvers start within any orbital period
    const Array<Interval>& maneuverIntervals = aSolution.maneuverIntervals;

    double longestManeuverSec = 0.0;
    double shortestManeuverGapMin = std::numeric_limits<double>::quiet_NaN();
    Size maximumManeuversPerOrbit = 0;

    for (Size i = 0; i < maneuverIntervals.getSize(); ++i)
    {
        longestManeuverSec = std::max(longestManeuverSec, double(maneuverIntervals[i].getDuration().inSeconds()));

        if (i > 0)
        {
            const double gapMin = (maneuverIntervals[i].getStart() - maneuverIntervals[i - 1].getEnd()).inMinutes();

            shortestManeuverGapMin = (i == 1) ? gapMin : std::min(shortestManeuverGapMin, gapMin);
        }

        // Maneuvers starting within the orbital period starting with this one (1 ms tolerance: a maneuver can start
        // exactly one orbital period after the start of the one before the previous one)
        Size maneuversPerOrbit = 0;

        for (Size j = i; j < maneuverIntervals.getSize(); ++j)
        {
            if ((maneuverIntervals[j].getStart() - maneuverIntervals[i].getStart()) <
                (ORBITAL_PERIOD - Duration::Milliseconds(1.0)))
            {
                ++maneuversPerOrbit;
            }
        }

        maximumManeuversPerOrbit = std::max(maximumManeuversPerOrbit, maneuversPerOrbit);
    }

    state.counters["maneuvers"] = static_cast<double>(maneuverIntervals.getSize());
    state.counters["longest_maneuver_s"] = longestManeuverSec;
    state.counters["shortest_maneuver_gap_min"] = shortestManeuverGapMin;
    state.counters["max_maneuvers_per_orbit"] = static_cast<double>(maximumManeuversPerOrbit);

    state.counters["duration_d"] = static_cast<double>(aSolution.getPropagationDuration().inDays());
    state.counters["converged"] = aSolution.conditionIsSatisfied ? 1.0 : 0.0;
    state.counters["delta_v_mps"] = static_cast<double>(aSolution.computeDeltaV(SPECIFIC_IMPULSE_S));

    const State& initialState = aSolution.states.accessFirst();
    const State& finalState = aSolution.states.accessLast();

    const Vector3d initialRTNOffset =
        ComputeRTNOffset(initialState, aReferenceTrajectory.getStateAt(initialState.accessInstant()));
    const Vector3d finalRTNOffset =
        ComputeRTNOffset(finalState, aReferenceTrajectory.getStateAt(finalState.accessInstant()));

    state.counters["initial_R_m"] = initialRTNOffset[0];
    state.counters["initial_T_m"] = initialRTNOffset[1];
    state.counters["initial_N_m"] = initialRTNOffset[2];
    state.counters["final_R_m"] = finalRTNOffset[0];
    state.counters["final_T_m"] = finalRTNOffset[1];
    state.counters["final_N_m"] = finalRTNOffset[2];

    const Vector6d finalROE = aGuidanceLaw.computeRelativeOrbitalElements(finalState);

    state.counters["final_da_m"] = finalROE[0];
    state.counters["final_dl_m"] = finalROE[1];
    state.counters["final_dex_m"] = finalROE[2];
    state.counters["final_dey_m"] = finalROE[3];
    state.counters["final_dix_m"] = finalROE[4];
    state.counters["final_diy_m"] = finalROE[5];
}

// ---------------------------------------------------------------------------
// Scenario: ROE targeting rendezvous from (R, T, N) = (-2, -5, +1) km, at most two maneuvers of at most 10 minutes
// per orbit
// ---------------------------------------------------------------------------

static void RunRendezvous(benchmark::State& state, const bool& allowRadialThrust)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();
    const Derived mu = EarthGravitationalModel::EGM96.gravitationalParameter_;

    const Array<Shared<Dynamics>> dynamics = BuildDynamics();
    const NumericalSolver solver = BuildSolver();
    const SatelliteSystem satelliteSystem = BuildSatelliteSystem();

    const Trajectory referenceTrajectory = BuildReferenceTrajectory(dynamics, solver, gcrfSPtr, mu);

    const ROETargeting::Parameters parameters = {
        BURN_SPACING,
        MAX_MANEUVER_DURATION,
        THRUST_N / INITIAL_MASS_KG,
        5,
        ROE_TOLERANCE_M,
        1.0,
        MIN_BURN_DURATION,
        0.0,
        EFFECTIVITY_THRESHOLD,
        BURN_START_CANDIDATE_COUNT,
        allowRadialThrust,
    };

    const Shared<const ROETargeting> roeTargetingSPtr = std::make_shared<ROETargeting>(referenceTrajectory, parameters);

    // Largest ROE error relative to its tolerance, below 1 once converged
    const Shared<RealCondition> condition = std::make_shared<RealCondition>(
        "ROE within tolerance",
        RealCondition::Criterion::StrictlyNegative,
        [roeTargetingSPtr](const State& aState) -> Real
        {
            return (roeTargetingSPtr->computeRelativeOrbitalElements(aState).cwiseAbs().array() /
                    ROE_TOLERANCE_M.array())
                .maxCoeff();
        },
        1.0
    );

    const Shared<Thruster> thruster = std::make_shared<Thruster>(satelliteSystem, roeTargetingSPtr);

    const Segment::ManeuverConstraints constraints = {
        Duration::Undefined(),
        MAX_MANEUVER_DURATION,
        MIN_MANEUVER_SEPARATION,
        Segment::MaximumManeuverDurationViolationStrategy::TruncateEnd,
        Pair<Duration, Duration>(Duration::Undefined(), Duration::Undefined()),
        Pair<Size, Duration>(MAX_MANEUVERS_PER_ORBIT, ORBITAL_PERIOD),
    };

    const Segment segment =
        Segment::Maneuver("ROE Targeting rendezvous", condition, thruster, dynamics, solver, constraints);

    const State initialState = BuildInitialState(gcrfSPtr, mu);

    for (auto _ : state)
    {
        Segment::Solution solution = segment.solve(initialState, MAX_PROPAGATION_DURATION);
        benchmark::DoNotOptimize(solution);

        state.PauseTiming();

        RecordCounters(state, solution, *roeTargetingSPtr, referenceTrajectory);

        state.ResumeTiming();
    }
}

// Burns in the tangential-normal plane only
static void BM_Segment_ROETargeting_Rendezvous(benchmark::State& state)
{
    RunRendezvous(state, false);
}

// Burns that also thrust radially
static void BM_Segment_ROETargeting_Rendezvous_RadialThrust(benchmark::State& state)
{
    RunRendezvous(state, true);
}

// ---------------------------------------------------------------------------
// Benchmark registration — each scenario runs a single iteration
// ---------------------------------------------------------------------------

BENCHMARK(BM_Segment_ROETargeting_Rendezvous)->Iterations(1)->Unit(benchmark::kSecond);
BENCHMARK(BM_Segment_ROETargeting_Rendezvous_RadialThrust)->Iterations(1)->Unit(benchmark::kSecond);
