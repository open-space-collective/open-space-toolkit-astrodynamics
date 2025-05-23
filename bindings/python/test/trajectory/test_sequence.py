# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.geometry.d3.object import Composite
from ostk.mathematics.geometry.d3.object import Cuboid
from ostk.mathematics.geometry.d3.object import Point

from ostk.physics import Environment
from ostk.physics.coordinate import Frame
from ostk.physics.environment.atmospheric import Earth as EarthAtmosphericModel
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.environment.magnetic import Earth as EarthMagneticModel
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import DateTime
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.time import Scale
from ostk.physics.unit import Derived
from ostk.physics.unit import Length
from ostk.physics.unit import Mass

from ostk.astrodynamics.event_condition import COECondition
from ostk.astrodynamics.event_condition import InstantCondition
from ostk.astrodynamics.event_condition import RealCondition
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics import EventCondition
from ostk.astrodynamics.dynamics import Thruster
from ostk.astrodynamics.guidance_law import ConstantThrust
from ostk.astrodynamics.flight.system import PropulsionSystem
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.trajectory import LocalOrbitalFrameDirection
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import Segment
from ostk.astrodynamics.trajectory import Sequence
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import CoordinateBroker
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state import NumericalSolver
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity


@pytest.fixture
def propulsion_system() -> PropulsionSystem:
    return PropulsionSystem(
        1.0,
        1500.0,
    )


@pytest.fixture
def dry_mass() -> Mass:
    return Mass.kilograms(100.0)


@pytest.fixture
def wet_mass() -> Mass:
    return Mass.kilograms(10.0)


@pytest.fixture
def cross_sectional_surface_area() -> float:
    return 1.0


@pytest.fixture
def drag_coefficient() -> float:
    return 2.2


@pytest.fixture
def satellite_system(
    dry_mass: Mass,
    cross_sectional_surface_area: float,
    drag_coefficient: float,
    propulsion_system: PropulsionSystem,
) -> SatelliteSystem:
    satellite_geometry = Composite(
        Cuboid(
            Point(0.0, 0.0, 0.0),
            [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],
            [1.0, 0.0, 0.0],
        )
    )
    inertia_tensor = np.ndarray(shape=(3, 3))

    return SatelliteSystem(
        dry_mass,
        satellite_geometry,
        inertia_tensor,
        cross_sectional_surface_area,
        drag_coefficient,
        propulsion_system,
    )


@pytest.fixture
def environment() -> Environment:
    return Environment(
        Instant.J2000(),
        [
            Earth.from_models(
                EarthGravitationalModel(EarthGravitationalModel.Type.Spherical),
                EarthMagneticModel(EarthMagneticModel.Type.Undefined),
                EarthAtmosphericModel(EarthAtmosphericModel.Type.Exponential),
            )
        ],
    )


@pytest.fixture
def coordinate_broker() -> CoordinateBroker:
    return CoordinateBroker(
        [
            CartesianPosition.default(),
            CartesianVelocity.default(),
            CoordinateSubset.mass(),
            CoordinateSubset.surface_area(),
            CoordinateSubset.drag_coefficient(),
        ]
    )


@pytest.fixture
def cartesian_coordinates() -> list[float]:
    return [
        717094.039086306,
        -6872433.2241124,
        46175.9696673281,
        -970.650826004612,
        -45.4598114773158,
        7529.82424886455,
    ]


@pytest.fixture
def state(
    cartesian_coordinates: list[float],
    dry_mass: Mass,
    wet_mass: Mass,
    cross_sectional_surface_area: float,
    drag_coefficient: float,
    coordinate_broker: CoordinateBroker,
) -> State:
    frame: Frame = Frame.GCRF()
    instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    coordinates = [
        *cartesian_coordinates,
        dry_mass.in_kilograms() + wet_mass.in_kilograms(),
        cross_sectional_surface_area,
        drag_coefficient,
    ]

    return State(
        instant,
        coordinates,
        frame,
        coordinate_broker,
    )


@pytest.fixture
def dynamics(environment: Environment) -> list:
    return Dynamics.from_environment(environment)


@pytest.fixture
def numerical_solver() -> NumericalSolver:
    return NumericalSolver.default_conditional()


@pytest.fixture
def duration() -> Duration:
    return Duration.minutes(5.0)


@pytest.fixture
def instant_condition(state: State, duration: Duration) -> InstantCondition:
    return InstantCondition(
        RealCondition.Criterion.AnyCrossing, state.get_instant() + duration
    )


@pytest.fixture
def sma() -> Length:
    return Length.kilometers(6907.000)


@pytest.fixture
def gravitational_parameter() -> Derived:
    return EarthGravitationalModel.spherical.gravitational_parameter


@pytest.fixture
def constant_thrust() -> ConstantThrust:
    return ConstantThrust(
        thrust_direction=LocalOrbitalFrameDirection(
            vector=[1.0, 0.0, 0.0],
            local_orbital_frame_factory=LocalOrbitalFrameFactory.VNC(Frame.GCRF()),
        )
    )


@pytest.fixture
def thruster_dynamics(
    satellite_system: SatelliteSystem, constant_thrust: ConstantThrust
) -> ConstantThrust:
    return Thruster(
        satellite_system=satellite_system,
        guidance_law=constant_thrust,
    )


@pytest.fixture
def sma_target(sma: Length) -> EventCondition.Target:
    return EventCondition.Target(sma)


@pytest.fixture
def sma_condition(
    sma_target: EventCondition.Target, gravitational_parameter: Derived
) -> COECondition:
    return COECondition.semi_major_axis(
        criterion=RealCondition.Criterion.AnyCrossing,
        frame=Frame.GCRF(),
        semi_major_axis=sma_target,
        gravitational_parameter=gravitational_parameter,
    )


@pytest.fixture
def coast_duration_segment(
    instant_condition: InstantCondition,
    dynamics: list[Dynamics],
    numerical_solver: NumericalSolver,
):
    return Segment.coast(
        name="duration coast",
        event_condition=instant_condition,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
    )


@pytest.fixture
def coast_sma_segment(
    sma_condition: COECondition,
    dynamics: list[Dynamics],
    numerical_solver: NumericalSolver,
):
    return Segment.coast(
        name="sma coast",
        event_condition=sma_condition,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
    )


@pytest.fixture
def thrust_segment(
    sma_condition: RealCondition,
    thruster_dynamics: Thruster,
    dynamics: list[Dynamics],
    numerical_solver: NumericalSolver,
):
    return Segment.maneuver(
        name="duration thrust",
        event_condition=sma_condition,
        thruster_dynamics=thruster_dynamics,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
    )


@pytest.fixture
def segments(
    coast_duration_segment: Segment,
    thrust_segment: Segment,
) -> list[Segment]:
    return [coast_duration_segment, thrust_segment]


@pytest.fixture
def maximum_propagation_duration() -> Duration:
    return Duration.days(2.0)


@pytest.fixture
def repetition_count() -> int:
    return 1


@pytest.fixture
def sequence(
    segments: list[Segment],
    numerical_solver: NumericalSolver,
    dynamics: list,
    maximum_propagation_duration: Duration,
):
    return Sequence(
        segments=segments,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
        maximum_propagation_duration=maximum_propagation_duration,
    )


@pytest.fixture
def segment_solution(
    dynamics: list,
    state: State,
):
    return Segment.Solution(
        name="A Segment Solution",
        dynamics=dynamics,
        states=[
            state,
        ],
        condition_is_satisfied=True,
        segment_type=Segment.Type.Maneuver,
    )


@pytest.fixture
def instants(state: State) -> list[Instant]:
    return [state.get_instant(), state.get_instant() + Duration.minutes(1.0)]


@pytest.fixture
def sequence_solution(
    segment_solution: Segment.Solution,
):
    return Sequence.Solution(
        segment_solutions=[
            segment_solution,
        ],
        execution_is_complete=True,
    )


@pytest.fixture
def minimum_maneuver_duration():
    return Duration.minutes(1.0)


@pytest.fixture
def sequence_with_minimum_maneuver_duration(
    segments: list[Segment],
    numerical_solver: NumericalSolver,
    dynamics: list,
    maximum_propagation_duration: Duration,
    minimum_maneuver_duration: Duration,
):
    return Sequence(
        segments=segments,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
        maximum_propagation_duration=maximum_propagation_duration,
        minimum_maneuver_duration=minimum_maneuver_duration,
    )


class TestSequenceSolution:
    def test_properties(
        self,
        sequence_solution: Sequence.Solution,
    ):
        assert sequence_solution is not None
        assert len(sequence_solution.segment_solutions) == 1
        assert sequence_solution.execution_is_complete

    def test_getters_and_accessors(
        self,
        sequence_solution: Sequence.Solution,
    ):
        assert sequence_solution.access_start_instant() is not None
        assert sequence_solution.access_end_instant() is not None

        assert sequence_solution.get_states() is not None
        assert sequence_solution.get_initial_mass() is not None
        assert sequence_solution.get_final_mass() is not None
        assert sequence_solution.get_propagation_duration() is not None

        assert sequence_solution.compute_delta_mass() is not None
        assert sequence_solution.compute_delta_v(1500.0) is not None

    def test_calculate_states_at(
        self,
        sequence_solution: Sequence.Solution,
        numerical_solver: NumericalSolver,
    ):
        instants: list[Instant] = sequence_solution.get_interval().generate_grid(
            Duration.seconds(10.0)
        )
        states: list[State] = sequence_solution.calculate_states_at(
            instants,
            numerical_solver,
        )

        assert states is not None
        assert len(states) == len(instants)


class TestSequence:
    def test_get_segments(
        self,
        sequence: Sequence,
        segments: list[Segment],
    ):
        assert len(sequence.get_segments()) == len(segments)

    def test_get_numerical_solver(
        self,
        sequence: Sequence,
        numerical_solver: NumericalSolver,
    ):
        assert sequence.get_numerical_solver() == numerical_solver

    def test_get_dynamics(
        self,
        sequence: Sequence,
        dynamics: list,
    ):
        assert len(sequence.get_dynamics()) == len(dynamics)

    def test_get_maximum_propagation_duration(
        self,
        sequence: Sequence,
        maximum_propagation_duration: Duration,
    ):
        assert sequence.get_maximum_propagation_duration() == maximum_propagation_duration

    def test_get_minimum_maneuver_duration(
        self,
        sequence_with_minimum_maneuver_duration: Sequence,
        minimum_maneuver_duration: Duration,
    ):
        assert (
            sequence_with_minimum_maneuver_duration.get_minimum_maneuver_duration()
            == minimum_maneuver_duration
        )

    def test_add_segment(
        self,
        sequence: Sequence,
        coast_duration_segment: Segment,
    ):
        segments_count: int = len(sequence.get_segments())

        sequence.add_segment(coast_duration_segment)

        assert len(sequence.get_segments()) == segments_count + 1

        segments_count = len(sequence.get_segments())

        sequence.add_segments([coast_duration_segment, coast_duration_segment])

        assert len(sequence.get_segments()) == segments_count + 2

    def test_add_coast_segment(
        self,
        sequence: Sequence,
        instant_condition: InstantCondition,
    ):
        segments_count: int = len(sequence.get_segments())

        sequence.add_coast_segment(instant_condition)

        assert len(sequence.get_segments()) == segments_count + 1

    def test_add_maneuver_segment(
        self,
        sequence: Sequence,
        instant_condition: InstantCondition,
        thruster_dynamics: Thruster,
    ):
        segments_count: int = len(sequence.get_segments())

        sequence.add_maneuver_segment(instant_condition, thruster_dynamics)

        assert len(sequence.get_segments()) == segments_count + 1

    def test_create_sequence_solution(
        self,
        segment_solution: Segment.Solution,
    ):
        solution: Sequence.Solution = Sequence.Solution(
            segment_solutions=[
                segment_solution,
            ],
            execution_is_complete=True,
        )

        assert solution is not None
        assert len(solution.segment_solutions) == 1
        assert solution.execution_is_complete

    def test_solve(
        self,
        state: State,
        repetition_count: int,
        sequence: Sequence,
        segments: list[Segment],
        instants: list[Instant],
        numerical_solver: NumericalSolver,
    ):
        solution = sequence.solve(
            state=state,
            repetition_count=repetition_count,
        )

        assert len(solution.segment_solutions) == len(segments)

        assert solution.execution_is_complete

        assert solution.access_start_instant() is not None
        assert solution.access_end_instant() is not None

        assert solution.get_states() is not None
        assert solution.get_initial_mass() is not None
        assert solution.get_final_mass() is not None
        assert solution.get_propagation_duration() is not None

        assert solution.compute_delta_mass() is not None
        assert solution.compute_delta_v(1500.0) is not None

        propagated_states = solution.calculate_states_at(
            instants,
            numerical_solver,
        )

        assert propagated_states is not None
        assert len(propagated_states) == len(instants)

    def test_solve_to_condition(
        self,
        state: State,
        sequence: Sequence,
        instant_condition: InstantCondition,
    ):
        assert sequence.solve_to_condition(state, instant_condition) is not None
        assert (
            sequence.solve_to_condition(
                state=state,
                event_condition=instant_condition,
                maximum_propagation_duration_limit=Duration.hours(1.0),
            )
            is not None
        )
