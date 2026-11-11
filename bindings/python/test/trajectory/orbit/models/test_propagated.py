# Apache License 2.0

import numpy as np
import pytest
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics import CentralBodyGravity, PositionDerivative
from ostk.astrodynamics.flight import Maneuver
from ostk.astrodynamics.trajectory import Orbit, Propagator, State
from ostk.astrodynamics.trajectory.orbit.model import Propagated, Tabulated
from ostk.astrodynamics.trajectory.state import CoordinateSubset, NumericalSolver
from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    CartesianAcceleration,
    CartesianPosition,
    CartesianVelocity,
)
from ostk.mathematics.curve_fitting import Interpolator
from ostk.physics.coordinate import Frame, Position, Velocity
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import DateTime, Duration, Instant, Scale


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
def revolution_number() -> int:
    return 5


@pytest.fixture
def propagated(
    propagator: Propagator, state: State, revolution_number: int
) -> Propagated:
    return Propagated(propagator, state, revolution_number)


@pytest.fixture
def state_with_mass() -> State:
    return State(
        Instant.J2000() - Duration.seconds(60.0),
        [7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 100.0],
        Frame.GCRF(),
        [
            CartesianPosition.default(),
            CartesianVelocity.default(),
            CoordinateSubset.mass(),
        ],
    )


def generate_maneuver(start_instant: Instant, duration_in_seconds: float) -> Maneuver:
    return Maneuver(
        [
            State(
                instant,
                [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, *acceleration, mass_flow_rate],
                Frame.GCRF(),
                [
                    CartesianPosition.default(),
                    CartesianVelocity.default(),
                    CartesianAcceleration.thrust_acceleration(),
                    CoordinateSubset.mass_flow_rate(),
                ],
            )
            for instant, acceleration, mass_flow_rate in [
                (start_instant, [1e-3, 0.0, 0.0], -1.1e-5),
                (
                    start_instant + Duration.seconds(duration_in_seconds),
                    [0.0, 1e-3, 0.0],
                    -0.9e-5,
                ),
            ]
        ],
        Interpolator.Type.Linear,
    )


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

        orbit = Orbit(propagated, earth)

        assert orbit is not None
        assert isinstance(orbit, Orbit)

        state_array = [state, state]
        propagated_with_state_array = Propagated(
            Propagator(numerical_solver, dynamics), state_array
        )

        assert propagated_with_state_array is not None
        assert isinstance(propagated_with_state_array, Propagated)

    def test_comparators(
        self,
        propagated: Propagated,
    ):
        assert (propagated == propagated) is True
        assert (propagated != propagated) is False

    def test_getters(
        self,
        propagated: Propagated,
        state: State,
        revolution_number: int,
    ):
        assert propagated.get_epoch() == state.get_instant()

        assert propagated.get_revolution_number_at_epoch() == revolution_number

        assert propagated.get_cached_state_array() == [state]

    def test_calculate_state_at(
        self,
        propagated: Propagated,
        orbit: Orbit,
    ):
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

    def test_calculate_states_at(
        self,
        propagated: Propagated,
        orbit: Orbit,
    ):
        instant_array = [
            Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 20, 0), Scale.UTC),
        ]

        propagated_state_array = propagated.calculate_states_at(instant_array)
        propagated_state_array_orbit = orbit.get_states_at(instant_array)

        assert propagated_state_array_orbit == propagated_state_array

        assert propagated_state_array_orbit[0].get_instant() == instant_array[0]
        assert propagated_state_array_orbit[1].get_instant() == instant_array[1]

    def test_calculate_revolution_number_at(
        self,
        propagated: Propagated,
        orbit: Orbit,
    ):
        instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 40, 0), Scale.UTC)

        revolution_number = propagated.calculate_revolution_number_at(instant)
        assert revolution_number is not None
        assert isinstance(revolution_number, int)

        revolution_number_orbit = orbit.get_revolution_number_at(instant)
        assert revolution_number_orbit is not None
        assert isinstance(revolution_number_orbit, int)

    def test_get_cached_state_array(
        self,
        propagated: Propagated,
        state: State,
    ):
        assert len(propagated.get_cached_state_array()) == 1
        assert propagated.get_cached_state_array()[0] == state

    def test_get_propagator(
        self,
        propagated: Propagated,
    ):
        assert propagated.get_propagator() is not None
        assert isinstance(propagated.get_propagator(), Propagator)

    def test_set_cached_state_array(
        self,
        propagated: Propagated,
        state: State,
    ):
        assert len(propagated.get_cached_state_array()) == 1

        propagated.set_cached_state_array([state, state, state])

        assert len(propagated.get_cached_state_array()) == 1
        assert propagated.get_cached_state_array()[0] == state

        with pytest.raises(Exception) as e:
            propagated.set_cached_state_array([])

    def test_to_tabulated(
        self,
        propagated: Propagated,
        state: State,
    ):
        instant_array = [
            state.get_instant() + Duration.minutes(float(offset)) for offset in range(4)
        ]

        tabulated = propagated.to_tabulated(instant_array)

        assert tabulated is not None
        assert isinstance(tabulated, Tabulated)

    def test_maneuvers(
        self,
        propagator: Propagator,
        state: State,
        state_with_mass: State,
    ):
        maneuver = generate_maneuver(Instant.J2000(), 60.0)

        propagated_without_maneuvers = Propagated(propagator, state_with_mass)
        propagated_with_maneuvers = Propagated(
            propagator, state_with_mass, 1, maneuvers=[maneuver]
        )

        assert propagated_without_maneuvers.get_maneuvers() == []
        assert propagated_with_maneuvers.get_maneuvers() == [maneuver]
        assert propagated_with_maneuvers != propagated_without_maneuvers
        assert propagated_with_maneuvers == Propagated(
            propagator, state_with_mass, 1, [maneuver]
        )

        # Same with a state array
        assert Propagated(
            propagator, [state_with_mass], 1, [maneuver]
        ).get_maneuvers() == [maneuver]

        # The maneuvers are provided to the propagator when calculating states
        instants = [
            Instant.J2000() + Duration.seconds(offset) for offset in (30.0, 200.0)
        ]

        states_with_maneuvers = propagated_with_maneuvers.calculate_states_at(instants)
        states_without_maneuvers = propagated_without_maneuvers.calculate_states_at(
            instants
        )
        expected_states = propagator.calculate_states_at(
            state_with_mass, instants, [maneuver]
        )

        assert states_with_maneuvers == expected_states
        # A single instant takes a different integration path than the batch
        single_state = propagated_with_maneuvers.calculate_state_at(instants[1])
        np.testing.assert_allclose(
            single_state.get_position().get_coordinates(),
            expected_states[1].get_position().get_coordinates(),
            atol=1e-4,
        )

        position_difference = np.linalg.norm(
            np.array(states_with_maneuvers[1].get_position().get_coordinates())
            - np.array(states_without_maneuvers[1].get_position().get_coordinates())
        )
        assert position_difference > 1.0

        mass = CoordinateSubset.mass()
        assert (
            states_with_maneuvers[1].extract_coordinate(mass)[0]
            < states_without_maneuvers[1].extract_coordinate(mass)[0]
        )

        # Maneuvers must not overlap, and need the mass in the states
        with pytest.raises(RuntimeError):
            Propagated(propagator, state_with_mass, 1, [maneuver, maneuver])

        with pytest.raises(RuntimeError):
            Propagated(propagator, state, 1, [maneuver])
