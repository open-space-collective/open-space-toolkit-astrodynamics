# Apache License 2.0

import pytest

import numpy as np

import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

Cuboid = mathematics.geometry.d3.objects.Cuboid
Composite = mathematics.geometry.d3.objects.Composite
Point = mathematics.geometry.d3.objects.Point

Mass = physics.units.Mass

System = astrodynamics.flight.System


@pytest.fixture
def system_default_inputs():
    mass = Mass(90.0, Mass.Unit.Kilogram)
    geometry = Composite(
        Cuboid(
            Point(0.0, 0.0, 0.0),
            [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],
            [1.0, 0.0, 0.0],
        )
    )

    return (mass, geometry)


@pytest.fixture
def system(system_default_inputs) -> System:
    return System(*system_default_inputs)


class TestSatelliteSystem:
    def test_constructors(self, system: System):
        assert system is not None
        assert isinstance(system, System)
        assert system.is_defined()

    def test_comparators(self, system: System):
        assert (system == system) is True
        assert (system != system) is False

    def test_getters(self, system_default_inputs, system: System):
        (mass, geometry) = system_default_inputs

        assert system.get_mass() == mass
        assert system.get_geometry() == geometry
