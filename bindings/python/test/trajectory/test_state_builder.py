# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import (
    State,
    StateBuilder,
)
from ostk.astrodynamics.trajectory.state import (
    CoordinatesBroker,
    CoordinatesSubset,
)
from ostk.astrodynamics.trajectory.state.coordinates_subset import (
    CartesianPosition,
    CartesianVelocity,
)


@pytest.fixture()
def instant() -> Instant:
    return Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def coordinates_subsets() -> list[CoordinatesSubset]:
    return [CartesianPosition.default(), CartesianVelocity.default()]


@pytest.fixture
def coordinates_broker(coordinates_subsets: list[CoordinatesSubset]) -> CoordinatesBroker:
    return CoordinatesBroker(coordinates_subsets)


@pytest.fixture
def state_builder(frame: Frame, coordinates_broker: CoordinatesBroker) -> State:
    return StateBuilder(frame, coordinates_broker)


@pytest.fixture
def coordinates_subsets_with_mass() -> list[CoordinatesSubset]:
    return [
        CartesianPosition.default(),
        CartesianVelocity.default(),
        CoordinatesSubset.mass(),
    ]


@pytest.fixture
def state_builder_with_mass(
    frame: Frame, coordinates_subsets_with_mass: list[CoordinatesSubset]
) -> State:
    return StateBuilder(frame, CoordinatesBroker(coordinates_subsets_with_mass))


class TestStateBuilder:
    def test_broker_constructor(
        self,
        frame: Frame,
        coordinates_broker: CoordinatesBroker,
    ):
        builder = StateBuilder(frame, coordinates_broker)
        assert builder is not None
        assert isinstance(builder, StateBuilder)
        assert builder.is_defined()

    def test_subsets_constructor(
        self,
        frame: Frame,
        coordinates_subsets: list[CoordinatesSubset],
    ):
        builder = StateBuilder(frame, coordinates_subsets)
        assert builder is not None
        assert isinstance(builder, StateBuilder)
        assert builder.is_defined()

    def test_comparators(self, state_builder: StateBuilder):
        assert (state_builder == state_builder) is True
        assert (state_builder != state_builder) is False

    def test_build_state(
        self,
        instant: Instant,
        state_builder: StateBuilder,
    ):
        coordinates = [1, 2, 3, 1, 2, 3]
        state: State = state_builder.build_state(instant, coordinates)

        assert state is not None
        assert isinstance(state, State)
        assert state.is_defined()
        assert state.get_instant() == instant
        assert (state.get_coordinates() == coordinates).all()
        assert state.get_frame() == state_builder.get_frame()
        assert state.get_coordinates_subsets() == state_builder.get_coordinates_subsets()

    def test_getters(
        self,
        state_builder: StateBuilder,
        frame: Frame,
        coordinates_broker: CoordinatesBroker,
    ):
        assert state_builder.get_frame() == frame
        assert state_builder.get_coordinates_subsets() == coordinates_broker.get_subsets()

    def test_expand(
        self,
        state_builder: StateBuilder,
    ):
        expaned_state_builder: StateBuilder = state_builder.expand(
            CoordinatesSubset.mass()
        )

        assert (state_builder == expaned_state_builder) is False
        assert (state_builder != expaned_state_builder) is True

        assert 2 == len(state_builder.get_coordinates_subsets())
        assert CartesianPosition.default() == state_builder.get_coordinates_subsets()[0]
        assert CartesianVelocity.default() == state_builder.get_coordinates_subsets()[1]

        assert 3 == len(expaned_state_builder.get_coordinates_subsets())
        assert (
            CartesianPosition.default()
            == expaned_state_builder.get_coordinates_subsets()[0]
        )
        assert (
            CartesianVelocity.default()
            == expaned_state_builder.get_coordinates_subsets()[1]
        )
        assert (
            CoordinatesSubset.mass() == expaned_state_builder.get_coordinates_subsets()[2]
        )

    def test_contract(
        self,
        state_builder_with_mass: StateBuilder,
    ):
        contracted_state_builder: StateBuilder = state_builder_with_mass.contract(
            CartesianVelocity.default()
        )

        assert (state_builder_with_mass == contracted_state_builder) is False
        assert (state_builder_with_mass != contracted_state_builder) is True

        assert 3 == len(state_builder_with_mass.get_coordinates_subsets())
        assert (
            CartesianPosition.default()
            == state_builder_with_mass.get_coordinates_subsets()[0]
        )
        assert (
            CartesianVelocity.default()
            == state_builder_with_mass.get_coordinates_subsets()[1]
        )
        assert (
            CoordinatesSubset.mass()
            == state_builder_with_mass.get_coordinates_subsets()[2]
        )

        assert 2 == len(contracted_state_builder.get_coordinates_subsets())
        assert (
            CartesianPosition.default()
            == contracted_state_builder.get_coordinates_subsets()[0]
        )
        assert (
            CoordinatesSubset.mass()
            == contracted_state_builder.get_coordinates_subsets()[1]
        )
