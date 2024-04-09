# Apache License 2.0

import pytest

import numpy as np


from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Duration
from ostk.physics.coordinate import Frame
from ostk.physics.unit import Mass

from ostk.astrodynamics.flight import Maneuver
from ostk.astrodynamics.dynamics import Tabulated as TabulatedDynamics
from ostk.astrodynamics.trajectory.state import CoordinateSubset


from ..dynamics.test_tabulated import (
    instants as tabulated_instants,
    contribution_profile as tabulated_contribution_profile,
    coordinate_subsets as tabulated_coordinate_subsets,
)


@pytest.fixture
def instants() -> list[Instant]:
    return [
        Instant.J2000(),
        Instant.J2000() + Duration.seconds(1.0),
        Instant.J2000() + Duration.seconds(5.0),
        Instant.J2000() + Duration.seconds(7.0),
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
def maneuver(
    instants: list[Instant],
    acceleration_profile: list[np.ndarray],
    frame: Frame,
    mass_flow_rate_profile: list[float],
) -> Maneuver:
    return Maneuver(instants, acceleration_profile, frame, mass_flow_rate_profile)


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
        instants: list[Instant],
        acceleration_profile: list[np.ndarray],
        mass_flow_rate_profile: list[float],
    ):
        assert maneuver.is_defined()

        assert maneuver.get_instants() == instants

        computed_acceleration_profile_default_frame = maneuver.get_acceleration_profile()
        for i in range(len(instants)):
            assert np.array_equal(
                computed_acceleration_profile_default_frame[i], acceleration_profile[i]
            )

        computed_acceleration_profile_non_default_frame = (
            maneuver.get_acceleration_profile(frame=Frame.ITRF())
        )
        for i in range(len(instants)):
            assert (
                np.array_equal(
                    computed_acceleration_profile_non_default_frame[i],
                    acceleration_profile[i],
                )
                == False
            )

        assert maneuver.get_mass_flow_rate_profile() == mass_flow_rate_profile

        assert maneuver.get_interval() == Interval.closed(instants[0], instants[-1])

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
        assert tabulated_dynamics.access_instants() == maneuver.get_instants()

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

    def test_from_tabulated_dynamics(
        self,
        tabulated_dynamics: TabulatedDynamics,
    ):
        maneuver = Maneuver.tabulated_dynamics(tabulated_dynamics)

        assert maneuver.is_defined()
        assert maneuver.get_instants() == tabulated_dynamics.access_instants()
        assert (
            len(maneuver.get_acceleration_profile())
            == tabulated_dynamics.access_contribution_profile().shape[0]
        )
        assert (
            len(maneuver.get_mass_flow_rate_profile())
            == tabulated_dynamics.access_contribution_profile().shape[0]
        )

    def test_from_constant_mass_flow_rate(
        self,
        instants: list[Instant],
        acceleration_profile: list[np.ndarray],
        frame: Frame,
    ):
        mass_flow_rate: float = -1.0e-5
        maneuver = Maneuver.constant_mass_flow_rate_profile(
            instants=instants,
            acceleration_profile=acceleration_profile,
            frame=frame,
            mass_flow_rate=mass_flow_rate,
        )

        assert maneuver.is_defined()
        assert maneuver.get_mass_flow_rate_profile() == [
            mass_flow_rate for _ in range(len(instants))
        ]
