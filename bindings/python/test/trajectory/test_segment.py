# Apache License 2.0

import pytest
import numpy as np

from ostk.physics import Environment
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.coordinate import Frame
from ostk.physics.environment.object.celestial import Earth

from ostk.astrodynamics.flight import Maneuver
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.dynamics import CentralBodyGravity
from ostk.astrodynamics.dynamics import PositionDerivative
from ostk.astrodynamics.dynamics import Thruster
from ostk.astrodynamics.guidance_law import ConstantThrust
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
def central_body_gravity() -> CentralBodyGravity:
    return CentralBodyGravity(Earth.spherical())


@pytest.fixture
def position_derivative() -> PositionDerivative:
    return PositionDerivative()


@pytest.fixture
def dynamics(
    position_derivative: PositionDerivative, central_body_gravity: CentralBodyGravity
) -> list:
    return [
        position_derivative,
        central_body_gravity,
    ]


@pytest.fixture
def numerical_solver() -> NumericalSolver:
    return NumericalSolver.default_conditional()


@pytest.fixture
def end_instant(state: State) -> Instant:
    return state.get_instant() + Duration.minutes(15.0)


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
def instants(state: State) -> list[Instant]:
    return [state.get_instant(), state.get_instant() + Duration.minutes(1.0)]


@pytest.fixture
def thruster_dynamics() -> Thruster:
    return Thruster(
        satellite_system=SatelliteSystem.default(),
        guidance_law=ConstantThrust.intrack(),
    )


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

    def test_get_numerical_solver(
        self,
        numerical_solver: NumericalSolver,
        coast_duration_segment: Segment,
    ):
        assert coast_duration_segment.get_numerical_solver() == numerical_solver

    def test_get_type(
        self,
        coast_duration_segment: Segment,
    ):
        assert coast_duration_segment.get_type() == Segment.Type.Coast

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
        thruster_dynamics: ConstantThrust,
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

    def test_solve(
        self,
        state: State,
        end_instant: Instant,
        maneuver_segment: Segment,
        instants: list[Instant],
        numerical_solver: NumericalSolver,
    ):
        solution = maneuver_segment.solve(state)

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
