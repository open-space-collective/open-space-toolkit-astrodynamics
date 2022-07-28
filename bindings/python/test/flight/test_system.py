################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/test_system.py
# @author         Antoine Paletta <antoine.paletta@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

import ostk.physics as physics
import ostk.mathematics as mathematics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Cuboid = mathematics.geometry.d3.objects.Cuboid
Composite = mathematics.geometry.d3.objects.Composite
Point = mathematics.geometry.d3.objects.Point

Mass = physics.units.Mass

System = astrodynamics.flight.System

################################################################################################################################################################

@pytest.fixture
def system_default_inputs_fix ():

    mass = Mass(90.0, Mass.Unit.Kilogram)
    geometry = Composite(Cuboid(Point(0.0, 0.0, 0.0), [ [1.0, 0.0, 0.0 ], [ 0.0, 1.0, 0.0 ], [ 0.0, 0.0, 1.0 ] ], [1.0, 0.0, 0.0 ] ))

    return mass, geometry

@pytest.fixture
def system_fix (system_default_inputs_fix) -> System:
    mass, geometry = system_default_inputs_fix
    satellite_system = System(mass, geometry)

    return satellite_system

################################################################################################################################################################

class TestSatelliteSystem:

    def test_constructors (self, system_fix: System):

        assert system_fix is not None
        assert isinstance(system_fix, System)
        assert system_fix.is_defined()

    def test_comparators (self, system_fix: System):

        assert system_fix == system_fix
        assert (system_fix != system_fix) is False

    def test_getters (self, system_default_inputs_fix, system_fix: System):
        mass, geometry = system_default_inputs_fix

        assert system_fix.get_mass() == mass
        assert system_fix.get_geometry() == geometry

################################################################################################################################################################
