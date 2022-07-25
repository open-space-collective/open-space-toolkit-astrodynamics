################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/test_satellite_system.py
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
Point = mathematics.geometry.d3.objects.Point

Mass = physics.units.Mass

SatelliteSystem = astrodynamics.flight.SatelliteSystem

################################################################################################################################################################

@pytest.fixture
def satellitesystem_default_inputs_fix ():

    mass = Mass(90.0, Mass.Unit.Kilogram)
    inertia_tensor = np.ndarray(shape=(3, 3))
    satellite_geometry = Cuboid(Point(0.0, 0.0, 0.0), [ [1.0, 0.0, 0.0 ], [ 0.0, 1.0, 0.0 ], [ 0.0, 0.0, 1.0 ] ], [1.0, 0.0, 0.0 ] )
    surface_area = 0.8
    drag_coefficient = 2.2

    return mass, inertia_tensor, satellite_geometry, surface_area, drag_coefficient

@pytest.fixture
def satellitesystem_fix (satellitesystem_default_inputs_fix) -> SatelliteSystem:
    mass, inertia_tensor, satellite_geometry, surface_area, drag_coefficient = satellitesystem_default_inputs_fix
    satellite_system = SatelliteSystem(mass, inertia_tensor, satellite_geometry, surface_area, drag_coefficient)

    return satellite_system

################################################################################################################################################################

class TestSatelliteSystem:

    def test_constructors (self, satellitesystem_fix: SatelliteSystem):

        assert satellitesystem_fix is not None
        assert isinstance(satellitesystem_fix, SatelliteSystem)
        assert satellitesystem_fix.is_defined()

    def test_comparators (self, satellitesystem_fix: SatelliteSystem):

        assert satellitesystem_fix == satellitesystem_fix
        assert (satellitesystem_fix != satellitesystem_fix) is False

    def test_getters (self, satellitesystem_default_inputs_fix, satellitesystem_fix: SatelliteSystem):
        mass, inertia_tensor, satellite_geometry, surface_area, drag_coefficient = satellitesystem_default_inputs_fix

        assert satellitesystem_fix.get_mass() == mass
        assert np.array_equal(satellitesystem_fix.get_inertia_tensor(), inertia_tensor)
        assert satellitesystem_fix.get_geometry() == satellite_geometry
        assert satellitesystem_fix.get_cross_sectional_surface_area() == surface_area
        assert satellitesystem_fix.get_drag_coefficient() == drag_coefficient

################################################################################################################################################################
