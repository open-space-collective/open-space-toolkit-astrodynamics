################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/system/test_satellite_system.py
# @author         Antoine Paletta <antoine.paletta@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np

import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Cuboid = mathematics.geometry.d3.objects.Cuboid
Composite = mathematics.geometry.d3.objects.Composite
Point = mathematics.geometry.d3.objects.Point

Mass = physics.units.Mass

SatelliteSystem = astrodynamics.flight.system.SatelliteSystem

################################################################################################################################################################

@pytest.fixture
def satellite_system_default_inputs ():

    mass = Mass(90.0, Mass.Unit.Kilogram)
    satellite_geometry = Composite(Cuboid(Point(0.0, 0.0, 0.0), [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]], [1.0, 0.0, 0.0]))
    inertia_tensor = np.ndarray(shape=(3, 3))
    surface_area = 0.8
    drag_coefficient = 2.2

    return (mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient)

@pytest.fixture
def satellite_system (satellite_system_default_inputs) -> SatelliteSystem:

    return SatelliteSystem(*satellite_system_default_inputs)

################################################################################################################################################################

class TestSatelliteSystem:

    def test_constructors (self, satellite_system: SatelliteSystem):

        assert satellite_system is not None
        assert isinstance(satellite_system, SatelliteSystem)
        assert satellite_system.is_defined()

    def test_comparators (self, satellite_system: SatelliteSystem):

        assert (satellite_system == satellite_system) is True
        assert (satellite_system != satellite_system) is False

    def test_getters (self, satellite_system_default_inputs, satellite_system: SatelliteSystem):

        (mass, satellite_geometry, inertia_tensor, surface_area, drag_coefficient) = satellite_system_default_inputs

        assert satellite_system.get_mass() == mass
        assert satellite_system.get_geometry() == satellite_geometry
        assert np.array_equal(satellite_system.get_inertia_tensor(), inertia_tensor)
        assert satellite_system.get_cross_sectional_surface_area() == surface_area
        assert satellite_system.get_drag_coefficient() == drag_coefficient

################################################################################################################################################################
