# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import DateTime
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.time import Scale
from ostk.physics.unit import Angle
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import LocalOrbitalFrameDirection

from ostk.astrodynamics import GuidanceLaw
from ostk.astrodynamics.flight import Maneuver
from ostk.astrodynamics.guidance_law import ConstantThrust
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianAcceleration


@pytest.fixture
def local_orbital_frame_direction() -> LocalOrbitalFrameDirection:
    return LocalOrbitalFrameDirection(
        vector=[1.0, 0.0, 0.0],
        local_orbital_frame_factory=LocalOrbitalFrameFactory.VNC(Frame.GCRF()),
    )


@pytest.fixture
def guidance_law(
    local_orbital_frame_direction: LocalOrbitalFrameDirection,
) -> ConstantThrust:
    return ConstantThrust(thrust_direction=local_orbital_frame_direction)


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2021, 3, 20, 12, 0, 0), Scale.UTC)


@pytest.fixture
def position_coordinates() -> list[float]:
    return [7000000.0, 0.0, 0.0]


@pytest.fixture
def velocity_coordinates() -> list[float]:
    return [0.0, 7546.05329, 0.0]


@pytest.fixture
def thrust_acceleration() -> float:
    return 1.0 / 105.0


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


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
def maneuver(
    instants: list[Instant],
    acceleration_profile: list[np.ndarray],
    mass_flow_rate_profile: list[float],
    frame: Frame,
    coordinate_subsets: list[CoordinateSubset],
) -> Maneuver:
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
    return Maneuver(states)


class TestConstantThrust:
    def test_constructors(self, guidance_law: ConstantThrust):
        assert guidance_law is not None
        assert isinstance(guidance_law, ConstantThrust)
        assert isinstance(guidance_law, GuidanceLaw)

    def test_getters(self, guidance_law: ConstantThrust):
        assert guidance_law.get_local_thrust_direction() is not None

    def test_static_constructors(self):
        assert ConstantThrust.intrack() is not None

        assert ConstantThrust.intrack(velocity_direction=False) is not None

        assert ConstantThrust.intrack(velocity_direction=True) is not None

    def test_compute_acceleration_success(
        self,
        guidance_law: ConstantThrust,
        instant: Instant,
        position_coordinates: list[float],
        velocity_coordinates: list[float],
        thrust_acceleration: float,
        frame: Frame,
    ):
        contribution = guidance_law.calculate_thrust_acceleration_at(
            instant=instant,
            position_coordinates=position_coordinates,
            velocity_coordinates=velocity_coordinates,
            thrust_acceleration=thrust_acceleration,
            output_frame=frame,
        )

        assert len(contribution) == 3
        assert contribution == pytest.approx([0.0, 0.009523809523809525, 0.0], abs=5e-11)

    def test_from_maneuver(self, maneuver: Maneuver):
        constant_thrust = ConstantThrust.from_maneuver(
            maneuver=maneuver,
            local_orbital_frame_factory=LocalOrbitalFrameFactory.TNW(Frame.GCRF()),
        )
        assert isinstance(constant_thrust, ConstantThrust)

        constant_thrust_with_maximum_allowed_angular_offset = (
            ConstantThrust.from_maneuver(
                maneuver=maneuver,
                local_orbital_frame_factory=LocalOrbitalFrameFactory.TNW(Frame.GCRF()),
                maximum_allowed_angular_offset=Angle.degrees(180.0),
            )
        )
        assert isinstance(
            constant_thrust_with_maximum_allowed_angular_offset, ConstantThrust
        )
