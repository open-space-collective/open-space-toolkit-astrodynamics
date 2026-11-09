# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.geometry.d3.object import Cuboid
from ostk.mathematics.geometry.d3.object import Composite
from ostk.mathematics.geometry.d3.object import Point
from ostk.physics.unit import Mass
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.flight.system import PropulsionSystem


@pytest.fixture
def satellite_system_default_inputs():
    mass = Mass(90.0, Mass.Unit.Kilogram)
    satellite_geometry = Composite(
        Cuboid(
            Point(0.0, 0.0, 0.0),
            [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],
            [1.0, 0.0, 0.0],
        )
    )
    inertia_tensor = np.ndarray(shape=(3, 3))
    surface_area = 0.8
    drag_coefficient = 2.2
    propulsion_system = PropulsionSystem(
        1.0,
        150.0,
    )

    return (
        mass,
        satellite_geometry,
        inertia_tensor,
        surface_area,
        drag_coefficient,
        propulsion_system,
    )


@pytest.fixture
def satellite_system(satellite_system_default_inputs) -> SatelliteSystem:
    return SatelliteSystem(*satellite_system_default_inputs)


class TestSatelliteSystem:
    def test_constructors(
        self,
        satellite_system: SatelliteSystem,
    ):
        assert satellite_system is not None
        assert isinstance(satellite_system, SatelliteSystem)

    def test_comparators(
        self,
        satellite_system: SatelliteSystem,
    ):
        assert (satellite_system == satellite_system) is True
        assert (satellite_system != satellite_system) is False

    def test_getters(
        self,
        satellite_system_default_inputs,
        satellite_system: SatelliteSystem,
    ):
        (
            mass,
            satellite_geometry,
            inertia_tensor,
            surface_area,
            drag_coefficient,
            propulsion_system,
        ) = satellite_system_default_inputs

        assert satellite_system.get_mass() == mass
        assert satellite_system.get_geometry() == satellite_geometry
        assert np.array_equal(satellite_system.get_inertia_tensor(), inertia_tensor)
        assert satellite_system.get_cross_sectional_surface_area() == surface_area
        assert satellite_system.get_drag_coefficient() == drag_coefficient
        assert satellite_system.get_propulsion_system() == propulsion_system

    def test_absent_cross_sectional_surface_area_drag_coefficient_and_propulsion_system(
        self,
        satellite_system_default_inputs,
    ):
        (
            mass,
            satellite_geometry,
            inertia_tensor,
            _,
            _,
            _,
        ) = satellite_system_default_inputs

        satellite_system = SatelliteSystem(
            mass,
            satellite_geometry,
            inertia_tensor,
            None,
            None,
        )

        assert isinstance(satellite_system, SatelliteSystem)

        with pytest.raises(RuntimeError):
            satellite_system.get_cross_sectional_surface_area()

        with pytest.raises(RuntimeError):
            satellite_system.get_drag_coefficient()

        with pytest.raises(RuntimeError):
            satellite_system.get_propulsion_system()

        satellite_system = SatelliteSystem(
            mass,
            satellite_geometry,
            inertia_tensor,
            0.8,
            2.2,
            propulsion_system=None,
        )

        assert satellite_system.get_cross_sectional_surface_area() == 0.8
        assert satellite_system.get_drag_coefficient() == 2.2

        with pytest.raises(RuntimeError):
            satellite_system.get_propulsion_system()
