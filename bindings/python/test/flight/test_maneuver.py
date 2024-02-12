# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.curve_fitting import Interpolator

from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import DateTime
from ostk.physics.time import Duration
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame
from ostk.physics.unit import Mass

from ostk.astrodynamics.flight import Maneuver
from ostk.astrodynamics.dynamics import Tabulated as TabulatedDynamics


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
    return [1.0e-5, 1.1e-5, 0.9e-5, 1.0e-5]


@pytest.fixture
def maneuver(
    instants: list[Instant],
    acceleration_profile: list[np.ndarray],
    frame: Frame,
    mass_flow_rate_profile: list[float],
) -> Maneuver:
    return Maneuver(instants, acceleration_profile, frame, mass_flow_rate_profile)


class TestManeuver:
    def test_constructor(
        self,
        maneuver: Maneuver,
    ):
        assert maneuver is not None
        assert isinstance(maneuver, Maneuver)
        assert maneuver.is_defined()

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
        pass
        # assert maneuver.calculate_delta_v().get_value() == pytest.approx(
        #     0.001394181, rel=1e-4
        # )

        # assert maneuver.calculate_delta_mass().in_kilograms() == pytest.approx(
        #     1.26e-4, rel=1e-4
        # )

        # assert maneuver.calculate_average_thrust(
        #     initial_spacecraft_mass=Mass(100.0, Mass.Unit.Kilogram)
        # ).get_value() == pytest.approx(0.1393676608, rel=1e-4)

        # assert maneuver.calculate_average_specific_impulse(
        #     initial_spacecraft_mass=Mass(100.0, Mass.Unit.Kilogram)
        # ).get_value() == pytest.approx(789.816, rel=1e-4)

    def test_calculate_acceleration_at(
        self,
        maneuver: Maneuver,
        instants: list[Instant],
        acceleration_profile: list[np.ndarray],
    ):
        for i in range(len(instants)):
            interpolated_acceleration = maneuver.calculate_acceleration_at(
                instant=instants[i],
                frame=Frame.GCRF(),
                interpolation_type=Interpolator.Type.Linear,
            )
            assert interpolated_acceleration == pytest.approx(
                acceleration_profile[i], rel=1e-15
            )

    def test_calculate_accelerations_at(
        self,
        maneuver: Maneuver,
        instants: list[Instant],
        acceleration_profile: list[np.ndarray],
    ):
        interpolated_accelerations = maneuver.calculate_accelerations_at(
            instants=instants,
            frame=Frame.GCRF(),
            interpolation_type=Interpolator.Type.Linear,
        )
        for i in range(len(instants)):
            assert interpolated_accelerations[i] == pytest.approx(
                acceleration_profile[i], rel=1e-15
            )

    def test_calculate_mass_flow_rate_at(
        self,
        maneuver: Maneuver,
        instants: list[Instant],
        mass_flow_rate_profile: list[float],
    ):
        for i in range(len(instants)):
            interpolated_mass_flow_rate = maneuver.calculate_mass_flow_rate_at(
                instant=instants[i], interpolation_type=Interpolator.Type.Linear
            )
            assert interpolated_mass_flow_rate == pytest.approx(
                mass_flow_rate_profile[i], rel=1e-15
            )

    def test_calculate_mass_flow_rates_at(
        self,
        maneuver: Maneuver,
        instants: list[Instant],
        mass_flow_rate_profile: list[float],
    ):
        interpolated_mass_flow_rates = maneuver.calculate_mass_flow_rates_at(
            instants=instants, interpolation_type=Interpolator.Type.Linear
        )
        for i in range(len(instants)):
            assert interpolated_mass_flow_rates[i] == pytest.approx(
                mass_flow_rate_profile[i], rel=1e-15
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
        assert tabulated_dynamics.get_instants() == maneuver.get_instants()

        contribution_profile: np.ndarray = tabulated_dynamics.get_contribution_profile()

        for i in range(len(instants)):
            assert contribution_profile[i][0:3] == pytest.approx(
                acceleration_profile[i], rel=1e-15
            )
            assert contribution_profile[i][3] == pytest.approx(
                mass_flow_rate_profile[i], rel=1e-15
            )

        assert tabulated_dynamics.get_frame() == frame

    def test_from_constant_mass_flow_rate(
        self,
        instants: list[Instant],
        acceleration_profile: list[np.ndarray],
        frame: Frame,
    ):
        mass_flow_rate: float = 1.0e-5
        maneuver = Maneuver.from_constant_mass_flow_rate_profile(
            instants=instants,
            acceleration_profile=acceleration_profile,
            frame=frame,
            mass_flow_rate=mass_flow_rate,
        )

        assert maneuver.is_defined()
        assert maneuver.get_mass_flow_rate_profile() == [
            mass_flow_rate for _ in range(len(instants))
        ]
