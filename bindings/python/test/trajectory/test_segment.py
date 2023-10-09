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

from ostk.astrodynamics.trajectory.state import NumericalSolver
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.dynamics import CentralBodyGravity
from ostk.astrodynamics.dynamics import PositionDerivative
from ostk.astrodynamics.dynamics.thruster import ConstantThrust
from ostk.astrodynamics.trajectory import LocalOrbitalFrameDirection
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Segment
from ostk.astrodynamics.event_condition import InstantCondition


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
):
    return Segment.coast(name, instant_condition, dynamics, numerical_solver)


@pytest.fixture
def thruster_dynamics() -> ConstantThrust:
    return ConstantThrust(
        satellite_system=SatelliteSystem.default(),
        thrust_direction=LocalOrbitalFrameDirection(
            vector=[1.0, 0.0, 0.0],
            local_orbital_frame_factory=LocalOrbitalFrameFactory.VNC(Frame.GCRF()),
        ),
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
                name, instant_condition, thruster_dynamics, dynamics, numerical_solver
            )
            is not None
        )

    def test_solve(
        self,
        state: State,
        end_instant: Instant,
        coast_duration_segment: Segment,
    ):
        solution = coast_duration_segment.solve(state)

        assert (
            pytest.approx(
                float((solution.states[-1].get_instant() - end_instant).in_seconds()),
                abs=1e-7,
            )
            == 0.0
        )
        assert len(solution.states) > 0
        assert solution.condition_is_satisfied
