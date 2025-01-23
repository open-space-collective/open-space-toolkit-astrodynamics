# Apache License 2.0

import pytest

from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory.state import CoordinateBroker, CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import NewtonianAcceleration
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition


@pytest.fixture
def name() -> str:
    return "Newtonian Acceleration"


@pytest.fixture
def cartesian_position() -> CartesianPosition:
    return CartesianPosition.default()


@pytest.fixture
def newtonian_acceleration() -> NewtonianAcceleration:
    return NewtonianAcceleration.default()


@pytest.fixture
def coordinate_subsets(
    newtonian_acceleration: NewtonianAcceleration,
) -> list[CoordinateSubset]:
    return [newtonian_acceleration]


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


class TestNewtonianAcceleration:
    def test_constructor(
        self,
        cartesian_position: CartesianPosition,
        name: str,
    ):
        assert NewtonianAcceleration(cartesian_position, name) is not None

    def test_add(
        self,
        newtonian_acceleration: NewtonianAcceleration,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        another_coordinates: list[float],
        coordinate_broker: CoordinateBroker,
    ):
        assert all(
            newtonian_acceleration.add(
                instant, coordinates, another_coordinates, frame, coordinate_broker
            )
            == [9.81, 9.81, 0.0]
        )

    def test_subtract(
        self,
        newtonian_acceleration: NewtonianAcceleration,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        another_coordinates: list[float],
        coordinate_broker: CoordinateBroker,
    ):
        assert all(
            newtonian_acceleration.subtract(
                instant, coordinates, another_coordinates, frame, coordinate_broker
            )
            == [9.81, -9.81, 0.0]
        )

    def test_in_frame(
        self,
        newtonian_acceleration: NewtonianAcceleration,
        instant: Instant,
        frame: Frame,
        coordinates: list[float],
        coordinate_broker: CoordinateBroker,
    ):
        assert (
            newtonian_acceleration.in_frame(
                instant, coordinates, frame, Frame.ITRF(), coordinate_broker
            )
            is not None
        )
