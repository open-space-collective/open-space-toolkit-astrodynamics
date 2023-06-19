# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.geometry.d3.objects import Cuboid
from ostk.mathematics.geometry.d3.objects import Composite
from ostk.mathematics.geometry.d3.objects import Point

from ostk.physics.units import Mass
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.environment.objects.celestial_bodies import Earth

from ostk.astrodynamics import NumericalSolver
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.flight.system.dynamics import CentralBodyGravity
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Propagator


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
    surface_area = 0.8
    drag_coefficient = 0.0

    return SatelliteSystem(
        mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient
    )


@pytest.fixture
def propagator(propagator_default_inputs) -> Propagator:
    return Propagator(*propagator_default_inputs[:2])


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
def gravitational_dynamics() -> CentralBodyGravity:
    return CentralBodyGravity(Earth.WGS84(20, 0))


@pytest.fixture
def dynamics(gravitational_dynamics: CentralBodyGravity) -> list:
    return [gravitational_dynamics]


@pytest.fixture
def numerical_solver() -> NumericalSolver:
    return NumericalSolver(
        NumericalSolver.LogType.NoLog,
        NumericalSolver.StepperType.RungeKuttaFehlberg78,
        5.0,
        1.0e-15,
        1.0e-15,
    )


@pytest.fixture
def propagator(dynamics: list, numerical_solver: NumericalSolver) -> Propagator:
    return Propagator(dynamics, numerical_solver)


class TestPropagator:
    def test_constructors(self, propagator: Propagator):
        assert propagator is not None
        assert isinstance(propagator, Propagator)
        assert propagator.is_defined()

    def test_get_dynamics(self, propagator: Propagator, dynamics: list):
        assert propagator.get_dynamics() == dynamics

    def test_set_dynamics(self, propagator: Propagator, dynamics: list):
        assert len(propagator.get_dynamics()) == 1

        propagator.set_dynamics(dynamics + dynamics)

        assert len(propagator.get_dynamics()) == 2

    def test_add_dynamics(
        self, propagator: Propagator, gravitational_dynamics: CentralBodyGravity
    ):
        assert len(propagator.get_dynamics()) == 1

        propagator.add_dynamics(gravitational_dynamics)
        propagator.add_dynamics(gravitational_dynamics)

        assert len(propagator.get_dynamics()) == 3

    def test_clear_dynamics(self, propagator: Propagator):
        assert len(propagator.get_dynamics()) == 1

        propagator.clear_dynamics()

        assert len(propagator.get_dynamics()) == 0

    def test_calculate_state(self, propagator: Propagator, state: State):
        instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC)

        propagator_state = propagator.calculate_state_at(state, instant)

        propagator_state_position_ref = np.array(
            [6265892.25765909, 3024770.94961259, 3024359.72137468]
        )
        propagator_state_velocity_ref = np.array(
            [-3974.49168221, 4468.16996776, 4466.19232746]
        )

        propagator_state_position = propagator_state.get_position().get_coordinates()
        propagator_state_velocity = propagator_state.get_velocity().get_coordinates()

        assert all(
            [
                round(propagator_state_position[i], 8)
                == round(propagator_state_position_ref[i], 8)
                for i in range(0, len(propagator_state_position_ref))
            ]
        )
        assert all(
            [
                round(propagator_state_velocity[i], 8)
                == round(propagator_state_velocity_ref[i], 8)
                for i in range(0, len(propagator_state_velocity_ref))
            ]
        )
        assert propagator_state.get_instant() == instant

    def test_calculate_states(self, propagator: Propagator, state: State):
        instant_array = [
            Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 20, 0), Scale.UTC),
        ]

        _ = propagator.calculate_states_at(state, instant_array)

        with pytest.raises(RuntimeError):
            instant_array.reverse()
            propagator.calculate_states_at(state, instant_array)
