# Apache License 2.0

import numpy as np

import pytest

from ostk.physics.coordinate import Frame
from ostk.physics.time import Instant
from ostk.physics.time import Duration
from ostk.physics.time import Interval
from ostk.astrodynamics import GuidanceLaw
from ostk.astrodynamics.guidance_law import HeterogeneousGuidanceLaw


@pytest.fixture
def interval_1() -> Interval:
    return Interval.closed(
        start_instant=Instant.J2000(),
        end_instant=Instant.J2000() + Duration.seconds(100.0),
    )


@pytest.fixture
def guidance_law_1() -> GuidanceLaw:
    class GuidanceLaw1(GuidanceLaw):

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
            return np.array([1.0, 2.0, 3.0])

    return GuidanceLaw1("My Guidance Law 1")


@pytest.fixture
def interval_2() -> Interval:
    return Interval.closed(
        start_instant=Instant.J2000() + Duration.seconds(200.0),
        end_instant=Instant.J2000() + Duration.seconds(300.0),
    )


@pytest.fixture
def guidance_law_2() -> GuidanceLaw:
    class GuidanceLaw2(GuidanceLaw):

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
            return np.array([4.0, 5.0, 6.0])

    return GuidanceLaw2("My Guidance Law 2")


@pytest.fixture
def heterogeneous_guidance_law(
    interval_1: Interval,
    guidance_law_1: GuidanceLaw,
    interval_2: Interval,
    guidance_law_2: GuidanceLaw,
) -> HeterogeneousGuidanceLaw:
    return HeterogeneousGuidanceLaw(
        guidance_laws_with_intervals=[
            (guidance_law_1, interval_1),
            (guidance_law_2, interval_2),
        ],
    )


class TestHeterogeneousGuidanceLaw:
    def test_constructor_and_getters(
        self,
        interval_1: Interval,
        guidance_law_1: GuidanceLaw,
        interval_2: Interval,
        guidance_law_2: GuidanceLaw,
    ):
        heterogeneous_guidance_law = HeterogeneousGuidanceLaw(
            guidance_laws_with_intervals=[
                (guidance_law_1, interval_1),
                (guidance_law_2, interval_2),
            ],
        )
        assert heterogeneous_guidance_law is not None
        assert isinstance(heterogeneous_guidance_law, HeterogeneousGuidanceLaw)
        assert heterogeneous_guidance_law.get_guidance_laws_with_intervals() == [
            (guidance_law_1, interval_1),
            (guidance_law_2, interval_2),
        ]

    def test_add_guidance_law(
        self,
        interval_1: Interval,
        guidance_law_1: GuidanceLaw,
        interval_2: Interval,
        guidance_law_2: GuidanceLaw,
    ):
        heterogeneous_guidance_law = HeterogeneousGuidanceLaw()

        assert isinstance(heterogeneous_guidance_law, HeterogeneousGuidanceLaw)
        assert heterogeneous_guidance_law.get_guidance_laws_with_intervals() == []

        heterogeneous_guidance_law.add_guidance_law(guidance_law_1, interval_1)
        assert heterogeneous_guidance_law.get_guidance_laws_with_intervals() == [
            (guidance_law_1, interval_1),
        ]

        heterogeneous_guidance_law.add_guidance_law(guidance_law_2, interval_2)
        assert heterogeneous_guidance_law.get_guidance_laws_with_intervals() == [
            (guidance_law_1, interval_1),
            (guidance_law_2, interval_2),
        ]

    @pytest.mark.parametrize(
        (
            "instant",
            "expected_thrust_acceleration",
        ),
        [
            (Instant.J2000() - Duration.seconds(50.0), np.array([0.0, 0.0, 0.0])),
            (Instant.J2000() + Duration.seconds(50.0), np.array([1.0, 2.0, 3.0])),
            (Instant.J2000() + Duration.seconds(150.0), np.array([0.0, 0.0, 0.0])),
            (Instant.J2000() + Duration.seconds(250.0), np.array([4.0, 5.0, 6.0])),
            (Instant.J2000() + Duration.seconds(350.0), np.array([0.0, 0.0, 0.0])),
        ],
    )
    def test_calculate_thrust_acceleration_at(
        self,
        heterogeneous_guidance_law: HeterogeneousGuidanceLaw,
        instant: Instant,
        expected_thrust_acceleration: np.array,
    ):
        assert np.array_equal(
            heterogeneous_guidance_law.calculate_thrust_acceleration_at(
                instant=instant,
                position_coordinates=np.array([0.0, 0.0, 0.0]),
                velocity_coordinates=np.array([0.0, 0.0, 0.0]),
                thrust_acceleration=1.0,
                output_frame=Frame.GCRF(),
            ),
            expected_thrust_acceleration,
        )
