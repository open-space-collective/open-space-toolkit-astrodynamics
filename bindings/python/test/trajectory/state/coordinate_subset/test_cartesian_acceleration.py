# Apache License 2.0

import pytest

from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory.state import CoordinateBroker, CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianAcceleration
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity


@pytest.fixture
def name() -> str:
    return "Cartesian Acceleration"


@pytest.fixture
def cartesian_position() -> CartesianPosition:
    return CartesianPosition.default()


@pytest.fixture
def cartesian_velocity() -> CartesianVelocity:
    return CartesianVelocity.default()


@pytest.fixture
def cartesian_acceleration() -> CartesianAcceleration:
    return CartesianAcceleration.default()


@pytest.fixture
def coordinate_subsets(
    cartesian_acceleration: CartesianAcceleration,
) -> list[CoordinateSubset]:
    return [cartesian_acceleration]


@pytest.fixture
def instant() -> Instant:
    return Instant.J2000()


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def coordinate_broker(
    coordinate_subsets: list[CoordinateSubset],
) -> CoordinateBroker:
    return CoordinateBroker(coordinate_subsets)


@pytest.fixture
def coordinates() -> list[float]:
    return [9.81, 0.0, 0.0]


@pytest.fixture
def another_coordinates() -> list[float]:
    return [0.0, 9.81, 0.0]


class TestCartesianAcceleration:
    def test_constructor(
        self,
        cartesian_position: CartesianPosition,
        cartesian_velocity: CartesianVelocity,
        name: str,
    ):
        assert (
            CartesianAcceleration(cartesian_position, cartesian_velocity, name)
            is not None
        )

    def test_add(
        self,
        cartesian_acceleration: CartesianAcceleration,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        another_coordinates: list[float],
        coordinate_broker: CoordinateBroker,
    ):
        assert all(
            cartesian_acceleration.add(
                instant, coordinates, another_coordinates, frame, coordinate_broker
            )
            == [9.81, 9.81, 0.0]
        )

    def test_subtract(
        self,
        cartesian_acceleration: CartesianAcceleration,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        another_coordinates: list[float],
        coordinate_broker: CoordinateBroker,
    ):
        assert all(
            cartesian_acceleration.subtract(
                instant, coordinates, another_coordinates, frame, coordinate_broker
            )
            == [9.81, -9.81, 0.0]
        )

    def test_in_frame(
        self,
        cartesian_acceleration: CartesianAcceleration,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        coordinate_broker: CoordinateBroker,
    ):
        assert (
            cartesian_acceleration.in_frame(
                instant, coordinates, frame, frame, coordinate_broker
            )
            is not None
        )

        with pytest.raises(RuntimeError):
            cartesian_acceleration.in_frame(
                instant, coordinates, frame, Frame.ITRF(), coordinate_broker
            )

    def test_default(self):
        assert CartesianAcceleration.default() is not None

    def test_thrust_acceleration(self):
        assert CartesianAcceleration.thrust_acceleration() is not None
