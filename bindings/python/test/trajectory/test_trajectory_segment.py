# Apache License 2.0

import pytest

from ostk.physics import Environment
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.environment.objects.celestial_bodies import Earth

from ostk.astrodynamics import NumericalSolver
from ostk.astrodynamics.flight.system.dynamics import CentralBodyGravity
from ostk.astrodynamics.flight.system.dynamics import PositionDerivative
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import TrajectorySegment
from ostk.astrodynamics.event_condition import DurationCondition


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters([7500000.0, 0.0, 0.0], frame)
    velocity: Velocity = Velocity.meters_per_second(
        [0.0, 5335.865450622126, 5335.865450622126], frame
    )

    instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    return State(instant, position, velocity)


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
def duration() -> Duration:
    return Duration.minutes(15.0)


@pytest.fixture
def duration_condition(duration: Duration) -> DurationCondition:
    return DurationCondition(DurationCondition.Criteria.AnyCrossing, duration)


@pytest.fixture
def coast_duration_segment(
    duration_condition: DurationCondition,
    dynamics: list,
    numerical_solver: NumericalSolver,
):
    return TrajectorySegment.coast(duration_condition, dynamics, numerical_solver)


class TestTrajectorySegment:
    def test_get_name(self, coast_duration_segment: TrajectorySegment):
        assert coast_duration_segment.get_name() == "Coast"

    def test_get_event_condition(
        self,
        duration_condition: DurationCondition,
        coast_duration_segment: TrajectorySegment,
    ):
        assert coast_duration_segment.get_event_condition() == duration_condition

    def test_get_dynamics(
        self, dynamics: list, coast_duration_segment: TrajectorySegment
    ):
        assert len(coast_duration_segment.get_dynamics()) == len(dynamics)

    def test_get_numerical_solver(
        self, numerical_solver: NumericalSolver, coast_duration_segment: TrajectorySegment
    ):
        assert coast_duration_segment.get_numerical_solver() == numerical_solver

    def test_solve(
        self, state: State, duration: Duration, coast_duration_segment: TrajectorySegment
    ):
        solution = coast_duration_segment.solve(state)

        assert pytest.approx(
            float((solution.states[-1].get_instant() - state.get_instant()).in_seconds()),
            1e-3,
        ) == float(duration.in_seconds())
        assert len(solution.states) > 0
