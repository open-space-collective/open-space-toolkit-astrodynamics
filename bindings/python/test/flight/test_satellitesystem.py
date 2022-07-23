################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/flight/test_satellitesystem.py
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

def test_satellitesystem_constructors ():

    mass = Mass(90.0, Mass.Unit.Kilogram)

    inertia_tensor = np.ndarray(shape=(3, 3))

    satellite_geometry = Cuboid(Point(0.0, 0.0, 0.0), [ [1.0, 0.0, 0.0 ], [ 0.0, 1.0, 0.0 ], [ 0.0, 0.0, 1.0 ] ], [1.0, 0.0, 0.0 ] )
    surface_area = 0.8
    drag_coefficient = 2.2

    satellitesystem = SatelliteSystem(mass, inertia_tensor, satellite_geometry, surface_area, drag_coefficient)

    assert satellitesystem is not None
    assert isinstance(satellitesystem, SatelliteSystem)
    assert satellitesystem.is_defined()

    satellitesystem_default_yam: SatelliteSystem = SatelliteSystem.default_yam()

    assert satellitesystem_default_yam is not None
    assert isinstance(satellitesystem_default_yam, SatelliteSystem)
    assert satellitesystem_default_yam.is_defined()

################################################################################################################################################################

def test_satellitesystem_comparators ():

    satellitesystem: SatelliteSystem = SatelliteSystem.default_yam()

    assert satellitesystem == satellitesystem
    assert (satellitesystem != satellitesystem) is False

################################################################################################################################################################

def test_satellitesystem_getters ():

    mass = Mass(90.0, Mass.Unit.Kilogram)

    inertia_tensor = np.ndarray(shape=(3, 3))

    satellite_geometry = Cuboid(Point(0.0, 0.0, 0.0), [ [1.0, 0.0, 0.0 ], [ 0.0, 1.0, 0.0 ], [ 0.0, 0.0, 1.0 ] ], [1.0, 0.0, 0.0 ] )
    surface_area = 0.8
    drag_coefficient = 2.2

    satellitesystem = SatelliteSystem(mass, inertia_tensor, satellite_geometry, surface_area, drag_coefficient)

    assert satellitesystem.get_mass() == mass
    assert np.array_equal(satellitesystem.get_inertia_tensor(),inertia_tensor)
    assert satellitesystem.get_geometry() == satellite_geometry
    assert satellitesystem.get_cross_sectional_surface_area() == surface_area
    assert satellitesystem.get_drag_coefficient() == drag_coefficient

    ################################################################################################################################################################
