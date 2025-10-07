# Apache License 2.0

import numpy as np

import pytest

from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame
from ostk.astrodynamics import GuidanceLaw


@pytest.fixture
def guidance_law() -> GuidanceLaw:
    class MyGuidanceLaw(GuidanceLaw):

        def __init__(
            self,
            name: str,
        ):
            super().__init__(name)

        def calculate_thrust_acceleration_at(
            self,
            instant: Instant,
            position_coordinates: np.array,
            velocity_coordinates: np.array,
            thrust_acceleration: float,
            output_frame: Frame,
        ) -> np.array:
            return np.array([0.0, 0.0, 0.0])

    return MyGuidanceLaw("My Guidance Law")


class TestGuidanceLaw:
    def test_subclass(
        self,
        guidance_law: GuidanceLaw,
    ):
        assert isinstance(guidance_law, GuidanceLaw)

    def test_get_name(
        self,
        guidance_law: GuidanceLaw,
    ):
        assert guidance_law.get_name() == "My Guidance Law"

    def test_calculate_thrust_acceleration_at(
        self,
        guidance_law: GuidanceLaw,
    ):
        thrust_acceleration: np.array = guidance_law.calculate_thrust_acceleration_at(
            instant=Instant.J2000(),
            position_coordinates=np.array([0.0, 0.0, 0.0]),
            velocity_coordinates=np.array([0.0, 0.0, 0.0]),
            thrust_acceleration=1.0,
            output_frame=Frame.GCRF(),
        )

        assert np.array_equal(thrust_acceleration, np.array([0.0, 0.0, 0.0]))
