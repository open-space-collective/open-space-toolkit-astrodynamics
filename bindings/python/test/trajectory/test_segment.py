# Apache License 2.0

import pytest
import numpy as np

from ostk.physics import Environment
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.time import Interval
from ostk.physics.coordinate import Frame
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.unit import Angle

from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics import CentralBodyGravity
from ostk.astrodynamics.dynamics import PositionDerivative
from ostk.astrodynamics.dynamics import Thruster
from ostk.astrodynamics.guidance_law import ConstantThrust
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Segment
from ostk.astrodynamics.event_condition import InstantCondition
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state import CoordinateBroker
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics.trajectory.state import NumericalSolver


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def state() -> State:
    instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    coordinates: list[float] = [
        7500000.0,
        0.0,
        0.0,
        0.0,
        5335.865450622126,
        5335.865450622126,
        300.0,
    ]
    frame: Frame = Frame.GCRF()
    coordinate_broker: CoordinateBroker = CoordinateBroker(
        [
            CartesianPosition.default(),
            CartesianVelocity.default(),
            CoordinateSubset.mass(),
        ]
    )

    return State(instant, coordinates, frame, coordinate_broker)


@pytest.fixture
def state_2() -> State:
    instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 1, 0, 0), Scale.UTC)
    coordinates: list[float] = [7500000.0, 0.0, 0.0, 0.0, 5335.865450622126, 0.0, 300.0]
    frame: Frame = Frame.GCRF()
    coordinate_broker: CoordinateBroker = CoordinateBroker(
        [
            CartesianPosition.default(),
            CartesianVelocity.default(),
            CoordinateSubset.mass(),
        ]
    )

    return State(instant, coordinates, frame, coordinate_broker)


@pytest.fixture
def states(state: State, state_2: State) -> list[State]:
    return [state, state_2]


@pytest.fixture
def central_body_gravity() -> CentralBodyGravity:
    return CentralBodyGravity(Earth.spherical())


@pytest.fixture
def position_derivative() -> PositionDerivative:
    return PositionDerivative()


@pytest.fixture
def dynamics(
    position_derivative: PositionDerivative,
    central_body_gravity: CentralBodyGravity,
) -> list:
    return [
        position_derivative,
        central_body_gravity,
    ]


@pytest.fixture
def local_orbital_frame_factory() -> LocalOrbitalFrameFactory:
    return LocalOrbitalFrameFactory.VNC(Frame.GCRF())


@pytest.fixture
def numerical_solver() -> NumericalSolver:
    return NumericalSolver.default_conditional()


@pytest.fixture
def end_instant(state: State) -> Instant:
    return state.get_instant() + Duration.minutes(15.0)


@pytest.fixture
def interval(state: State) -> Interval:
    return Interval.closed(
        state.get_instant(), state.get_instant() + Duration.minutes(10.0)
    )


@pytest.fixture
def instant_condition(end_instant: Instant) -> InstantCondition:
    return InstantCondition(InstantCondition.Criterion.AnyCrossing, end_instant)


@pytest.fixture
def name() -> str:
    return "A Segment"


@pytest.fixture
def coast_duration_segment(
    name: str,
    instant_condition: InstantCondition,
    dynamics: list,
    numerical_solver: NumericalSolver,
) -> Segment:
    return Segment.coast(name, instant_condition, dynamics, numerical_solver)


@pytest.fixture
def maneuver_constraints() -> Segment.ManeuverConstraints:
    return Segment.ManeuverConstraints(
        minimum_duration=Duration.minutes(1.0),
        maximum_duration=Duration.minutes(10.0),
        minimum_separation=Duration.minutes(5.0),
        maximum_duration_strategy=Segment.MaximumManeuverDurationViolationStrategy.Skip,
    )


@pytest.fixture
def maneuver_segment(
    name: str,
    instant_condition: InstantCondition,
    dynamics: list,
    numerical_solver: NumericalSolver,
    thruster_dynamics: Thruster,
) -> Segment:
    return Segment.maneuver(
        name=name,
        event_condition=instant_condition,
        thruster_dynamics=thruster_dynamics,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
    )


@pytest.fixture
def maximum_allowed_angular_offset() -> Angle:
    return Angle.degrees(180.0)


@pytest.fixture
def constant_local_orbital_frame_direction_maneuver_segment(
    name: str,
    instant_condition: InstantCondition,
    thruster_dynamics: Thruster,
    dynamics: list,
    numerical_solver: NumericalSolver,
    local_orbital_frame_factory: LocalOrbitalFrameFactory,
) -> Segment:
    return Segment.constant_local_orbital_frame_direction_maneuver(
        name=name,
        event_condition=instant_condition,
        thruster_dynamics=thruster_dynamics,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
        local_orbital_frame_factory=local_orbital_frame_factory,
    )


@pytest.fixture
def constant_local_orbital_frame_direction_maneuver_segment_with_maximum_allowed_angular_offset(
    name: str,
    instant_condition: InstantCondition,
    thruster_dynamics: Thruster,
    dynamics: list,
    numerical_solver: NumericalSolver,
    local_orbital_frame_factory: LocalOrbitalFrameFactory,
    maximum_allowed_angular_offset: Angle,
) -> Segment:
    return Segment.constant_local_orbital_frame_direction_maneuver(
        name=name,
        event_condition=instant_condition,
        thruster_dynamics=thruster_dynamics,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
        local_orbital_frame_factory=local_orbital_frame_factory,
        maximum_allowed_angular_offset=maximum_allowed_angular_offset,
    )


@pytest.fixture
def instants(state: State) -> list[Instant]:
    return [state.get_instant(), state.get_instant() + Duration.minutes(1.0)]


@pytest.fixture
def thruster_dynamics() -> Thruster:
    return Thruster(
        satellite_system=SatelliteSystem.default(),
        guidance_law=ConstantThrust.intrack(),
    )


@pytest.fixture
def segment_solution(
    dynamics: list[Dynamics],
    state: State,
) -> Segment.Solution:
    return Segment.Solution(
        name="A Segment Solution",
        dynamics=dynamics,
        states=[
            state,
        ],
        condition_is_satisfied=True,
        segment_type=Segment.Type.Coast,
    )


@pytest.fixture
def maneuver_segment_solution(
    dynamics: list[Dynamics],
    thruster_dynamics: Thruster,
    states: list[State],
) -> Segment.Solution:
    return Segment.Solution(
        name="A Maneuver Segment Solution",
        dynamics=dynamics + [thruster_dynamics],
        states=states,
        condition_is_satisfied=True,
        segment_type=Segment.Type.Maneuver,
    )


class TestSegmentSolution:
    def test_constructors(
        self,
        segment_solution: Segment.Solution,
    ):
        assert segment_solution is not None
        assert segment_solution.name is not None
        assert segment_solution.dynamics is not None
        assert segment_solution.states is not None
        assert segment_solution.condition_is_satisfied is not None
        assert segment_solution.segment_type is not None

    def test_getters_and_accessors(
        self,
        segment_solution: Segment.Solution,
    ):
        assert segment_solution.access_start_instant() is not None
        assert segment_solution.access_end_instant() is not None
        assert segment_solution.get_interval() is not None
        assert segment_solution.get_initial_mass() is not None
        assert segment_solution.get_final_mass() is not None
        assert segment_solution.get_propagation_duration() is not None

    def test_compute_delta_v(
        self,
        segment_solution: Segment.Solution,
    ):
        assert segment_solution.compute_delta_v(1500.0) is not None

    def test_compute_delta_mass(
        self,
        segment_solution: Segment.Solution,
    ):
        assert segment_solution.compute_delta_mass() is not None

    def test_get_thruster_dynamics(
        self,
        maneuver_segment_solution: Segment.Solution,
        thruster_dynamics: Thruster,
    ):
        assert maneuver_segment_solution.get_thruster_dynamics() == thruster_dynamics

    def test_extract_maneuvers(
        self,
        maneuver_segment_solution: Segment.Solution,
    ):
        assert maneuver_segment_solution.extract_maneuvers(Frame.GCRF()) is not None

    def test_calculate_states_at(
        self,
        segment_solution: Segment.Solution,
    ):
        assert (
            segment_solution.calculate_states_at(
                [
                    segment_solution.states[0].get_instant(),
                ],
                NumericalSolver.default_conditional(),
            )
            is not None
        )

    def test_get_dynamics_contribution(
        self,
        segment_solution: Segment.Solution,
    ):
        assert (
            segment_solution.get_dynamics_contribution(
                dynamics=segment_solution.dynamics[1],
                frame=Frame.GCRF(),
                coordinate_subsets=[CartesianVelocity.default()],
            )
            is not None
        )

        assert (
            segment_solution.get_dynamics_contribution(
                dynamics=segment_solution.dynamics[1],
                frame=Frame.GCRF(),
            )
            is not None
        )

    def test_get_dynamics_acceleration_contribution(
        self,
        segment_solution: Segment.Solution,
    ):
        assert (
            segment_solution.get_dynamics_acceleration_contribution(
                dynamics=segment_solution.dynamics[1], frame=Frame.GCRF()
            )
            is not None
        )

    def test_get_all_dynamics_contributions(
        self,
        segment_solution: Segment.Solution,
    ):
        assert segment_solution.get_all_dynamics_contributions(Frame.GCRF()) is not None


class TestSegment:
    def test_get_name(self, coast_duration_segment: Segment, name: str):
        assert coast_duration_segment.get_name() == name

    def test_get_event_condition(
        self,
        instant_condition: InstantCondition,
        coast_duration_segment: Segment,
    ):
        assert coast_duration_segment.get_event_condition() == instant_condition

    def test_get_dynamics(
        self,
        dynamics: list,
        coast_duration_segment: Segment,
    ):
        assert len(coast_duration_segment.get_dynamics()) == len(dynamics)

    def test_get_free_dynamics(
        self,
        dynamics: list,
        coast_duration_segment: Segment,
        maneuver_segment: Segment,
    ):
        assert len(coast_duration_segment.get_free_dynamics()) == len(dynamics)
        assert len(maneuver_segment.get_free_dynamics()) == len(dynamics)

    def test_get_numerical_solver(
        self,
        numerical_solver: NumericalSolver,
        coast_duration_segment: Segment,
    ):
        assert coast_duration_segment.get_numerical_solver() == numerical_solver

    def test_get_thruster_dynamics(
        self,
        thruster_dynamics: Thruster,
        maneuver_segment: Segment,
    ):
        assert maneuver_segment.get_thruster_dynamics() == thruster_dynamics

    def test_get_type(
        self,
        coast_duration_segment: Segment,
    ):
        assert coast_duration_segment.get_type() == Segment.Type.Coast

    def test_get_maneuver_constraints(
        self,
        maneuver_segment: Segment,
    ):
        constraints = maneuver_segment.get_maneuver_constraints()
        assert constraints is not None
        assert isinstance(constraints, Segment.ManeuverConstraints)

    def test_coast(
        self,
        name: str,
        instant_condition: InstantCondition,
        dynamics: list,
        numerical_solver: NumericalSolver,
    ):
        assert (
            Segment.coast(name, instant_condition, dynamics, numerical_solver) is not None
        )

    def test_maneuver(
        self,
        name: str,
        instant_condition: InstantCondition,
        thruster_dynamics: Thruster,
        dynamics: list,
        numerical_solver: NumericalSolver,
    ):
        assert (
            Segment.maneuver(
                name=name,
                event_condition=instant_condition,
                thruster_dynamics=thruster_dynamics,
                dynamics=dynamics,
                numerical_solver=numerical_solver,
            )
            is not None
        )

        # Test with maneuver constraints
        constraints = Segment.ManeuverConstraints(
            maximum_duration=Duration.minutes(10.0),
            minimum_separation=Duration.minutes(5.0),
        )
        assert (
            Segment.maneuver(
                name=name,
                event_condition=instant_condition,
                thruster_dynamics=thruster_dynamics,
                dynamics=dynamics,
                numerical_solver=numerical_solver,
                maneuver_constraints=constraints,
            )
            is not None
        )

    def test_constant_local_orbital_frame_direction_maneuver(
        self,
        name: str,
        instant_condition: InstantCondition,
        thruster_dynamics: Thruster,
        dynamics: list,
        numerical_solver: NumericalSolver,
        local_orbital_frame_factory: LocalOrbitalFrameFactory,
        maximum_allowed_angular_offset: Angle,
    ):
        assert (
            Segment.constant_local_orbital_frame_direction_maneuver(
                name=name,
                event_condition=instant_condition,
                thruster_dynamics=thruster_dynamics,
                dynamics=dynamics,
                numerical_solver=numerical_solver,
                local_orbital_frame_factory=local_orbital_frame_factory,
            )
            is not None
        )

        assert (
            Segment.constant_local_orbital_frame_direction_maneuver(
                name=name,
                event_condition=instant_condition,
                thruster_dynamics=thruster_dynamics,
                dynamics=dynamics,
                numerical_solver=numerical_solver,
                local_orbital_frame_factory=local_orbital_frame_factory,
                maximum_allowed_angular_offset=maximum_allowed_angular_offset,
            )
            is not None
        )

        # Test with maneuver constraints
        constraints = Segment.ManeuverConstraints(
            maximum_duration=Duration.minutes(10.0),
            minimum_separation=Duration.minutes(5.0),
        )
        assert (
            Segment.constant_local_orbital_frame_direction_maneuver(
                name=name,
                event_condition=instant_condition,
                thruster_dynamics=thruster_dynamics,
                dynamics=dynamics,
                numerical_solver=numerical_solver,
                local_orbital_frame_factory=local_orbital_frame_factory,
                maximum_allowed_angular_offset=maximum_allowed_angular_offset,
                maneuver_constraints=constraints,
            )
            is not None
        )

    def test_create_solution(
        self,
        dynamics: list,
        state: State,
    ):
        segment_solution: Segment.Solution = Segment.Solution(
            name="A Segment",
            dynamics=dynamics,
            states=[
                state,
            ],
            condition_is_satisfied=True,
            segment_type=Segment.Type.Coast,
        )

        assert segment_solution is not None

    def test_solve_maneuver_segment(
        self,
        state: State,
        end_instant: Instant,
        maneuver_segment: Segment,
        instants: list[Instant],
        numerical_solver: NumericalSolver,
    ):
        solution: Segment.Solution = maneuver_segment.solve(state)

        assert (
            pytest.approx(
                float((solution.states[-1].get_instant() - end_instant).in_seconds()),
                abs=1e-7,
            )
            == 0.0
        )

        assert solution.name is not None
        assert solution.dynamics is not None
        assert len(solution.states) > 0
        assert solution.condition_is_satisfied is True
        assert solution.segment_type is not None

        assert solution.access_start_instant() is not None
        assert solution.access_end_instant() is not None

        assert solution.get_initial_mass() is not None
        assert solution.get_final_mass() is not None
        assert solution.get_propagation_duration() is not None

        assert solution.compute_delta_v(1500.0) is not None
        assert solution.compute_delta_mass() is not None

        assert solution.extract_maneuvers(state.get_frame()) is not None
        assert len(solution.extract_maneuvers(state.get_frame())) == 1

        propagated_states = solution.calculate_states_at(
            instants,
            numerical_solver,
        )

        assert propagated_states is not None
        assert len(propagated_states) == len(instants)

        first_dynamics_contribution = solution.get_dynamics_contribution(
            solution.dynamics[0], state.get_frame()
        )

        second_dynamics_contribution = solution.get_dynamics_contribution(
            solution.dynamics[1], state.get_frame()
        )

        third_dynamics_contribution = solution.get_dynamics_contribution(
            solution.dynamics[2], state.get_frame()
        )

        assert first_dynamics_contribution is not None
        assert second_dynamics_contribution is not None
        assert third_dynamics_contribution is not None
        assert first_dynamics_contribution.shape == (len(solution.states), 3)
        assert second_dynamics_contribution.shape == (len(solution.states), 3)
        assert third_dynamics_contribution.shape == (len(solution.states), 4)

        all_contributions = solution.get_all_dynamics_contributions(state.get_frame())
        assert len(all_contributions) == len(solution.dynamics)

        assert all_contributions is not None
        assert isinstance(all_contributions, dict)
        assert np.array_equal(
            all_contributions[solution.dynamics[0]], first_dynamics_contribution
        )
        assert np.array_equal(
            all_contributions[solution.dynamics[1]], second_dynamics_contribution
        )
        assert np.array_equal(
            all_contributions[solution.dynamics[2]], third_dynamics_contribution
        )

        acceleration_contribution = solution.get_dynamics_acceleration_contribution(
            solution.dynamics[1], state.get_frame()
        )
        assert acceleration_contribution is not None
        assert acceleration_contribution.shape == (len(solution.states), 3)
        assert isinstance(acceleration_contribution, np.ndarray)

        with pytest.raises(Exception):
            solution.get_dynamics_acceleration_contribution(
                solution.dynamics[0], state.get_frame()
            )

    def test_solve_with_previous_maneuver_interval(
        self,
        state: State,
        maneuver_segment: Segment,
        instants: list[Instant],
    ):
        solution: Segment.Solution = maneuver_segment.solve(
            state=state,
            maximum_propagation_duration=Duration.minutes(15.0),
            previous_maneuver_interval=Interval.closed(instants[0], instants[1]),
        )

        assert solution is not None

    def test_solve_constant_local_orbital_frame_direction_maneuver_segment(
        self,
        state: State,
        constant_local_orbital_frame_direction_maneuver_segment: Segment,
        constant_local_orbital_frame_direction_maneuver_segment_with_maximum_allowed_angular_offset: Segment,
    ):
        solution: Segment.Solution = (
            constant_local_orbital_frame_direction_maneuver_segment.solve(state)
        )
        assert solution is not None

        solution_with_maximum_allowed_angular_offset: Segment.Solution = (
            constant_local_orbital_frame_direction_maneuver_segment_with_maximum_allowed_angular_offset.solve(
                state
            )
        )
        assert solution_with_maximum_allowed_angular_offset is not None


class TestManeuverConstraints:
    def test_constructors(
        self,
        maneuver_constraints: Segment.ManeuverConstraints,
    ):
        # Default constructor
        assert maneuver_constraints is not None
        assert maneuver_constraints.minimum_duration is not None
        assert maneuver_constraints.maximum_duration is not None
        assert maneuver_constraints.minimum_separation is not None
        assert maneuver_constraints.maximum_duration_strategy is not None

    def test_is_defined(
        self,
        maneuver_constraints: Segment.ManeuverConstraints,
    ):
        assert maneuver_constraints.is_defined()

    def test_interval_has_valid_minimum_duration(
        self,
        maneuver_constraints: Segment.ManeuverConstraints,
        interval: Interval,
    ):
        assert (
            maneuver_constraints.interval_has_valid_minimum_duration(interval) is not None
        )

    def test_interval_has_valid_maximum_duration(
        self,
        maneuver_constraints: Segment.ManeuverConstraints,
        interval: Interval,
    ):
        assert (
            maneuver_constraints.interval_has_valid_maximum_duration(interval) is not None
        )
