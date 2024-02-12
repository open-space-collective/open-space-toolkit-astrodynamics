# Apache License 2.0

import pytest

import numpy as np

import ostk.mathematics as mathematics

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

Cuboid = mathematics.geometry.d3.object.Cuboid
Composite = mathematics.geometry.d3.object.Composite
Point = mathematics.geometry.d3.object.Point

Mass = physics.unit.Mass

SatelliteSystem = astrodynamics.flight.system.SatelliteSystem
SatelliteSystemBuilder = astrodynamics.flight.system.SatelliteSystemBuilder
PropulsionSystem = astrodynamics.flight.system.PropulsionSystem


class TestSatelliteSystemBuilder:
    def test_constructors(self):
        builder: SatelliteSystemBuilder = SatelliteSystemBuilder()
        assert builder is not None
        assert isinstance(builder, SatelliteSystemBuilder)

        default_builder: SatelliteSystemBuilder = SatelliteSystemBuilder.default()
        assert default_builder is not None
        assert isinstance(default_builder, SatelliteSystemBuilder)

    def test_build(self):
        dry_mass = Mass(90.0, Mass.Unit.Kilogram)
        geometry = Composite(
            Cuboid(
                Point(0.0, 0.0, 0.0),
                [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],
                [1.0, 0.0, 0.0],
            )
        )
        inertia_tensor = np.ndarray(shape=(3, 3))
        cross_sectional_surface_area = 0.8
        drag_coefficient = 2.2
        propulsion_system = PropulsionSystem(
            1.0,
            150.0,
        )

        builder: SatelliteSystemBuilder = (
            SatelliteSystemBuilder()
            .with_dry_mass(dry_mass)
            .with_geometry(geometry)
            .with_inertia_tensor(inertia_tensor)
            .with_cross_sectional_surface_area(cross_sectional_surface_area)
            .with_drag_coefficient(drag_coefficient)
            .with_propulsion_system(propulsion_system)
        )

        satellite_system: SatelliteSystem = builder.build()

        assert satellite_system.get_mass() == dry_mass
        assert satellite_system.get_geometry() == geometry
        assert np.array_equal(satellite_system.get_inertia_tensor(), inertia_tensor)
        assert (
            satellite_system.get_cross_sectional_surface_area()
            == cross_sectional_surface_area
        )
        assert satellite_system.get_drag_coefficient() == drag_coefficient
        assert satellite_system.get_propulsion_system() == propulsion_system
