# Apache License 2.0

import numpy as np
import pytest
from ostk.astrodynamics.trajectory import Model, State
from ostk.astrodynamics.trajectory.model import Static
from ostk.physics.coordinate import Frame, Position
from ostk.physics.time import Instant


@pytest.fixture
def position() -> Position:
    return Position.meters([7000000.0, 0.0, 0.0], Frame.ITRF())


@pytest.fixture
def static(position: Position) -> Static:
    return Static(position)


class TestStatic:
    def test_constructor(
        self,
        static: Static,
    ):
        assert static is not None
        assert isinstance(static, Static)
        assert isinstance(static, Model)
        assert static.is_defined()

    def test_constructor_with_output_frame(
        self,
        position: Position,
    ):
        static: Static = Static(position, Frame.GCRF())

        assert isinstance(static, Static)
        assert static.is_defined()

    def test_constructor_with_none_output_frame(
        self,
        position: Position,
    ):
        static: Static = Static(position, None)

        assert isinstance(static, Static)
        assert static.is_defined()
        assert static == Static(position)

    def test_get_frame(
        self,
        static: Static,
    ):
        assert isinstance(static.get_frame(), Frame)

    def test_is_defined(
        self,
        static: Static,
    ):
        assert static.is_defined()

    def test_calculate_state_at(
        self,
        static: Static,
        position: Position,
    ):
        instant: Instant = Instant.J2000()

        state: State = static.calculate_state_at(instant)

        assert isinstance(state, State)
        assert state.get_instant() == instant

    def test_equality_operator(
        self,
        static: Static,
        position: Position,
    ):
        assert static == static
        assert static == Static(position)

    def test_inequality_operator(
        self,
        static: Static,
    ):
        assert static != Static(Position.meters([0.0, 0.0, 0.0], Frame.ITRF()))
