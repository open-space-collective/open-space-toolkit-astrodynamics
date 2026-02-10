# Apache License 2.0

import numpy as np

import pytest

from ostk.physics.time import Duration
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


@pytest.fixture
def guidance_law_with_buffer() -> GuidanceLaw:
    class MyGuidanceLaw(GuidanceLaw):
        def __init__(
            self,
            name: str,
            weight_transition_buffer_duration: Duration,
        ):
            super().__init__(name, weight_transition_buffer_duration)

        def calculate_thrust_acceleration_at(
            self,
            instant: Instant,
            position_coordinates: np.array,
            velocity_coordinates: np.array,
            thrust_acceleration: float,
            output_frame: Frame,
        ) -> np.array:
            return np.array([0.0, 0.0, 0.0])

    return MyGuidanceLaw("My Guidance Law", Duration.seconds(30.0))


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

    def test_get_weight_transition_buffer_duration_undefined(
        self,
        guidance_law: GuidanceLaw,
    ):
        assert not guidance_law.get_weight_transition_buffer_duration().is_defined()

    def test_get_weight_transition_buffer_duration_defined(
        self,
        guidance_law_with_buffer: GuidanceLaw,
    ):
        assert (
            guidance_law_with_buffer.get_weight_transition_buffer_duration()
            == Duration.seconds(30.0)
        )
