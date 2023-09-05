# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.geometry.d3.objects import Cuboid
from ostk.mathematics.geometry.d3.objects import Composite
from ostk.mathematics.geometry.d3.objects import Point

from ostk.physics import Environment
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.units import Length
from ostk.physics.units import Mass
from ostk.physics.units import Derived
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.environment.objects.celestial_bodies import Earth
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.environment.atmospheric import Earth as EarthAtmosphericModel
from ostk.physics.environment.magnetic import Earth as EarthMagneticModel

from ostk.astrodynamics import NumericalSolver
from ostk.astrodynamics.event_condition import InstantCondition
from ostk.astrodynamics.event_condition import COECondition
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Sequence
from ostk.astrodynamics.trajectory.state import TrajectorySegment
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.flight.system import Dynamics


@pytest.fixture
def satellite_system() -> SatelliteSystem:
    mass = Mass(90.0, Mass.Unit.Kilogram)
    satellite_geometry = Composite(
        Cuboid(
            Point(0.0, 0.0, 0.0),
            [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],
            [1.0, 0.0, 0.0],
        )
    )
    inertia_tensor = np.identity(3)
    surface_area = 500.0
    drag_coefficient = 2.2

    return SatelliteSystem(
        mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient
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
def state() -> State:
    frame: Frame = Frame.GCRF()
    position: Position = Position.meters(
        [717094.039086306, -6872433.2241124, 46175.9696673281], frame
    )
    velocity: Velocity = Velocity.meters_per_second(
        [-970.650826004612, -45.4598114773158, 7529.82424886455], frame
    )

    instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    return State(instant, position, velocity)


@pytest.fixture
def dynamics(environment: Environment, satellite_system: SatelliteSystem) -> list:
    return Dynamics.from_environment(environment, satellite_system)


@pytest.fixture
def numerical_solver() -> NumericalSolver:
    return NumericalSolver.default_conditional()


@pytest.fixture
def duration() -> Duration:
    return Duration.minutes(5.0)


@pytest.fixture
def instant_condition(state: State, duration: Duration) -> InstantCondition:
    return InstantCondition(
        InstantCondition.Criteria.AnyCrossing, state.get_instant() + duration
    )


@pytest.fixture
def sma() -> Length:
    return Length.kilometers(6904.000)


@pytest.fixture
def gravitational_parameter() -> Derived:
    return EarthGravitationalModel.spherical.gravitational_parameter


@pytest.fixture
def sma_condition(sma: Length, gravitational_parameter: Derived) -> COECondition:
    return COECondition.semi_major_axis(
        COECondition.Criteria.AnyCrossing,
        sma,
        gravitational_parameter,
    )


@pytest.fixture
def coast_duration_segment(instant_condition: InstantCondition):
    return TrajectorySegment.Coast(instant_condition)


@pytest.fixture
def coast_sma_segment(sma_condition: COECondition):
    return TrajectorySegment.Coast(sma_condition)


@pytest.fixture
def segments(
    coast_duration_segment: TrajectorySegment, coast_sma_segment: TrajectorySegment
) -> list[TrajectorySegment]:
    return [coast_sma_segment, coast_duration_segment]


@pytest.fixture
def maximum_propagation_duration() -> Duration:
    return Duration.days(2.0)


@pytest.fixture
def sequence(
    segments: list[TrajectorySegment],
    dynamics: list,
    numerical_solver: NumericalSolver,
    maximum_propagation_duration: Duration,
):
    return Sequence(segments, numerical_solver, dynamics, maximum_propagation_duration)


class TestSequence:
    def test_get_segments(self, sequence: Sequence, segments: list[TrajectorySegment]):
        assert len(sequence.get_segments()) == len(segments)

    def test_get_dynamics(self, sequence: Sequence, dynamics: list):
        assert len(sequence.get_dynamics()) == len(dynamics)

    def test_get_numerical_solver(
        self, sequence: Sequence, numerical_solver: NumericalSolver
    ):
        assert sequence.get_numerical_solver() == numerical_solver

    def test_get_maximum_propagation_duration(
        self, sequence: Sequence, maximum_propagation_duration: Duration
    ):
        assert sequence.get_maximum_propagation_duration() == maximum_propagation_duration

    def test_get_dynamics(self, dynamics: list, sequence: Sequence):
        assert len(sequence.get_dynamics()) == len(dynamics)

    def test_solve(self, state: State, sequence: Sequence, duration: Duration):
        solution = sequence.solve(state)

        assert len(solution.segment_solutions) == 2
        assert solution.segment_solutions[0].initial_state == state
        assert (
            solution.segment_solutions[1].final_state.get_instant() - state.get_instant()
        ) > duration
