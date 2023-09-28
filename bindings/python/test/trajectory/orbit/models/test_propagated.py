# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.environment.objects.celestial_bodies import Earth

from ostk.astrodynamics.trajectory.state import NumericalSolver
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import Propagator
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics import CentralBodyGravity
from ostk.astrodynamics.dynamics import PositionDerivative
from ostk.astrodynamics.trajectory.orbit.models import Propagated


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
def central_body_dynamics() -> CentralBodyGravity:
    return CentralBodyGravity(Earth.WGS84(20, 0))


@pytest.fixture
def position_derivative() -> PositionDerivative:
    return PositionDerivative()


@pytest.fixture
def dynamics(
    position_derivative: PositionDerivative, central_body_dynamics: CentralBodyGravity
) -> list:
    return [position_derivative, central_body_dynamics]


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
def propagator(numerical_solver: NumericalSolver, dynamics: list[Dynamics]) -> Propagator:
    return Propagator(numerical_solver, dynamics)


@pytest.fixture
def propagated(propagator: Propagator, state: State) -> Propagated:
    return Propagated(propagator, state)


@pytest.fixture
def earth() -> Earth:
    return Earth.spherical()


@pytest.fixture
def orbit(propagated: Propagated, earth: Earth) -> Orbit:
    return Orbit(propagated, earth)


class TestPropagated:
    def test_constructors(
        self,
        propagated: Propagated,
        earth: Earth,
        dynamics: list,
        numerical_solver: NumericalSolver,
        state: State,
    ):
        assert propagated is not None
        assert isinstance(propagated, Propagated)
        assert propagated.is_defined()

        orbit = Orbit(propagated, earth)

        assert orbit is not None
        assert isinstance(orbit, Orbit)
        assert orbit.is_defined()

        state_array = [state, state]
        propagated_with_state_array = Propagated(
            Propagator(numerical_solver, dynamics), state_array
        )

        assert propagated_with_state_array is not None
        assert isinstance(propagated_with_state_array, Propagated)
        assert propagated_with_state_array.is_defined()

    def test_comparators(self, propagated: Propagated):
        assert (propagated == propagated) is True
        assert (propagated != propagated) is False

    def test_getters(self, propagated: Propagated, state: State):
        assert propagated.get_epoch() == state.get_instant()

        assert propagated.get_revolution_number_at_epoch() == 1

        assert propagated.access_cached_state_array() == [state]

    def test_calculate_state_at(self, propagated: Propagated, orbit: Orbit):
        instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC)

        propagated_state = propagated.calculate_state_at(instant)
        propagated_state_orbit = orbit.get_state_at(instant)

        assert propagated_state == propagated_state_orbit

        propagated_state_position_ref = np.array(
            [6265892.25765909, 3024770.94961259, 3024359.72137468]
        )
        propagated_state_velocity_ref = np.array(
            [-3974.49168221, 4468.16996776, 4466.19232746]
        )

        propagated_state_position = propagated_state.get_position().get_coordinates()
        propagated_state_velocity = propagated_state.get_velocity().get_coordinates()

        assert all(
            [
                round(propagated_state_position[i], 8)
                == round(propagated_state_position_ref[i], 8)
                for i in range(0, len(propagated_state_position_ref))
            ]
        )
        assert all(
            [
                round(propagated_state_velocity[i], 8)
                == round(propagated_state_velocity_ref[i], 8)
                for i in range(0, len(propagated_state_velocity_ref))
            ]
        )
        assert propagated_state.get_instant() == instant

    def test_calculate_states_at(self, propagated: Propagated, orbit: Orbit):
        instant_array = [
            Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 20, 0), Scale.UTC),
        ]

        propagated_state_array = propagated.calculate_states_at(instant_array)
        propagated_state_array_orbit = orbit.get_states_at(instant_array)

        assert propagated_state_array_orbit == propagated_state_array

        assert propagated_state_array_orbit[0].get_instant() == instant_array[0]
        assert propagated_state_array_orbit[1].get_instant() == instant_array[1]

    def test_calculate_rev_number_at(self, propagated: Propagated, orbit: Orbit):
        instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 40, 0), Scale.UTC)

        assert propagated.calculate_revolution_number_at(instant) == 1
        assert orbit.get_revolution_number_at(instant) == 1

    def test_access_cached_state_array(self, propagated: Propagated, state: State):
        assert len(propagated.access_cached_state_array()) == 1
        assert propagated.access_cached_state_array()[0] == state

    def test_access_propagator(self, propagated: Propagated):
        assert propagated.access_propagator() is not None
        assert isinstance(propagated.access_propagator(), Propagator)

    def test_set_cached_state_array(self, propagated: Propagated, state: State):
        assert len(propagated.access_cached_state_array()) == 1

        propagated.set_cached_state_array([state, state, state])

        assert len(propagated.access_cached_state_array()) == 1
        assert propagated.access_cached_state_array()[0] == state

        with pytest.raises(Exception) as e:
            propagated.set_cached_state_array([])
