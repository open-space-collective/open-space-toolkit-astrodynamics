# Apache License 2.0

import pytest

import numpy as np

import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

Mass = physics.units.Mass

PropulsionSystem = astrodynamics.flight.system.PropulsionSystem


@pytest.fixture
def propulsion_system() -> PropulsionSystem:
    return PropulsionSystem(
        1.0,
        150.0,
    )


class TestPropulsionSystem:
    def test_constructors(
        self,
        propulsion_system: PropulsionSystem,
    ):
        assert propulsion_system is not None
        assert isinstance(propulsion_system, PropulsionSystem)
        assert propulsion_system.is_defined()

    def test_comparators(
        self,
        propulsion_system: PropulsionSystem,
    ):
        assert (propulsion_system == propulsion_system) is True
        assert (propulsion_system != propulsion_system) is False

    def test_is_defined(
        self,
        propulsion_system: PropulsionSystem,
    ):
        assert propulsion_system.is_defined()
