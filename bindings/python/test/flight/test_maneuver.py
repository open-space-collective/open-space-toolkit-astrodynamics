# Apache License 2.0

import pytest

import numpy as np


from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Duration
from ostk.physics.coordinate import Frame
from ostk.physics.unit import Mass

from ostk.astrodynamics.flight import Maneuver
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.dynamics import Tabulated as TabulatedDynamics
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianAcceleration


@pytest.fixture
def instants() -> list[Instant]:
    return [
        Instant.J2000(),
        Instant.J2000() + Duration.seconds(30.0),
        Instant.J2000() + Duration.seconds(35.0),
        Instant.J2000() + Duration.seconds(37.0),
    ]


@pytest.fixture
def acceleration_profile() -> list[np.ndarray]:
    return [
        np.array([1.0e-3, 0.0e-3, 0.0e-3]),
        np.array([0.0e-3, 1.0e-3, 0.0e-3]),
        np.array([0.0e-3, 0.0e-3, 1.0e-3]),
        np.array([1.0e-3, 1.0e-3, 1.0e-3]),
    ]


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def mass_flow_rate_profile() -> list[float]:
    return [-1.0e-5, -1.1e-5, -0.9e-5, -1.0e-5]


@pytest.fixture
def coordinate_subsets() -> list[CoordinateSubset]:
    return [
        CartesianPosition.default(),
        CartesianVelocity.default(),
        CartesianAcceleration.thrust_acceleration(),
        CoordinateSubset.mass_flow_rate(),
    ]


@pytest.fixture
def states(
    instants: list[Instant],
    acceleration_profile: list[np.ndarray],
    mass_flow_rate_profile: list[float],
    frame: Frame,
    coordinate_subsets: list[CoordinateSubset],
) -> list[State]:
    states = []
    for instant, acceleration, mass_flow_rate in zip(
        instants, acceleration_profile, mass_flow_rate_profile
    ):
        states.append(
            State(
                instant,
                [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, *acceleration, mass_flow_rate],
                frame,
                coordinate_subsets,
            )
        )
    return states


@pytest.fixture
def maneuver(
    states: list[State],
) -> Maneuver:
    return Maneuver(states)


@pytest.fixture
def tabulated_dynamics(
    tabulated_instants: list[Instant],
    tabulated_contribution_profile: np.ndarray,
    tabulated_coordinate_subsets: list[CoordinateSubset],
    frame: Frame,
) -> TabulatedDynamics:
    return TabulatedDynamics(
        tabulated_instants,
        tabulated_contribution_profile,
        tabulated_coordinate_subsets,
        frame,
    )


class TestManeuver:
    def test_constructor(
        self,
        maneuver: Maneuver,
    ):
        assert maneuver is not None
        assert isinstance(maneuver, Maneuver)
        assert maneuver.is_defined()

    def test_comparators(
        self,
        maneuver: Maneuver,
    ):
        assert (maneuver == maneuver) is True
        assert (maneuver != maneuver) is False

    def test_getters(
        self,
        maneuver: Maneuver,
        states: list[State],
    ):
        assert maneuver.is_defined()

        assert maneuver.get_states() == states

        assert maneuver.get_interval() == Interval.closed(
            states[0].get_instant(), states[-1].get_instant()
        )

    def test_calculators(
        self,
        maneuver: Maneuver,
    ):
        assert maneuver.calculate_delta_v() is not None
        assert maneuver.calculate_delta_mass().in_kilograms() is not None
        assert (
            maneuver.calculate_average_thrust(
                initial_spacecraft_mass=Mass(100.0, Mass.Unit.Kilogram)
            )
            is not None
        )
        assert (
            maneuver.calculate_average_specific_impulse(
                initial_spacecraft_mass=Mass(100.0, Mass.Unit.Kilogram)
            )
            is not None
        )

    def test_to_tabulated_dynamics(
        self,
        maneuver: Maneuver,
        instants: list[Instant],
        acceleration_profile: list[np.ndarray],
        frame: Frame,
        mass_flow_rate_profile: list[float],
    ):
        tabulated_dynamics: TabulatedDynamics = maneuver.to_tabulated_dynamics(
            frame=frame
        )

        assert tabulated_dynamics.is_defined()
        assert tabulated_dynamics.access_instants() == [
            state.get_instant() for state in maneuver.get_states()
        ]

        contribution_profile: np.ndarray = (
            tabulated_dynamics.access_contribution_profile()
        )

        for i in range(len(instants)):
            assert contribution_profile[i][0:3] == pytest.approx(
                acceleration_profile[i], rel=1e-15
            )
            assert contribution_profile[i][3] == pytest.approx(
                mass_flow_rate_profile[i], rel=1e-15
            )

        assert tabulated_dynamics.access_frame() == frame

    def test_from_constant_mass_flow_rate(
        self,
        states: list[State],
    ):
        mass_flow_rate: float = -1.0e-5
        maneuver = Maneuver.constant_mass_flow_rate_profile(
            states=states,
            mass_flow_rate=mass_flow_rate,
        )

        assert maneuver.is_defined()
