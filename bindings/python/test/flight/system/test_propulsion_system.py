# Apache License 2.0

import pytest

import numpy as np

import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

Mass = physics.unit.Mass

PropulsionSystem = astrodynamics.flight.system.PropulsionSystem


@pytest.fixture
def propulsion_system() -> PropulsionSystem:
    return PropulsionSystem(
        1.0,
        150.0,
    )


@pytest.fixture
def mass() -> Mass:
    return Mass(90.0, Mass.Unit.Kilogram)


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

    def test_getters(
        self,
        propulsion_system: PropulsionSystem,
        mass: Mass,
    ):
        assert propulsion_system.get_thrust() == 1.0
        assert propulsion_system.get_specific_impulse() == 150.0
        assert propulsion_system.get_mass_flow_rate() is not None
        assert propulsion_system.get_acceleration(mass) is not None

    def test_static_methods(
        self,
        mass: Mass,
    ):
        assert PropulsionSystem.undefined().is_defined() is False

        assert PropulsionSystem.default().is_defined() is True
        assert PropulsionSystem.default().get_thrust() is not None
        assert PropulsionSystem.default().get_specific_impulse() is not None
        assert PropulsionSystem.default().get_mass_flow_rate() is not None
        assert PropulsionSystem.default().get_acceleration(mass) is not None
