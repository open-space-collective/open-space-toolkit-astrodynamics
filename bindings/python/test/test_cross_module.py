# Apache License 2.0

"""Types bound by ostk.core, ostk.mathematics and ostk.physics crossing into and out of ostk.astrodynamics.

All the extension modules must use the same nanobind version and ABI for these to resolve.
"""

import sys

import numpy as np

from ostk.core.type import String

from ostk.mathematics.geometry.d3.object import Composite
from ostk.mathematics.geometry.d3.object import Cuboid
from ostk.mathematics.geometry.d3.object import Point
from ostk.mathematics.geometry.d3.transformation.rotation import Quaternion
from ostk.mathematics.object import RealInterval

import ostk.physics as physics
from ostk.physics import Environment
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.environment.object import Celestial
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.unit import Mass

import ostk.astrodynamics as astrodynamics
from ostk.astrodynamics.access import VisibilityCriterion
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.orbit.model import Kepler
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE


def test_upstream_modules_are_imported_by_astrodynamics():
    # ostk/astrodynamics/__init__.py imports ostk.physics (which imports ostk.io, ostk.mathematics and
    # ostk.core) first, so their types are registered before ostk.astrodynamics uses them.
    for module in ("ostk.core", "ostk.io", "ostk.mathematics", "ostk.physics"):
        assert module in sys.modules

    assert astrodynamics.time.Instant is Instant
    assert physics.time.Instant is Instant


def test_physics_arguments_and_return_values():
    instant = Instant.J2000()
    position = Position.meters([7.0e6, 0.0, 0.0], Frame.GCRF())
    velocity = Velocity.meters_per_second([0.0, 7.5e3, 0.0], Frame.GCRF())

    state = State(instant, position, velocity)

    assert type(state.get_instant()) is Instant
    assert state.get_instant() == instant
    assert type(state.get_position()) is Position
    assert state.get_position() == position
    assert type(state.get_velocity()) is Velocity
    assert state.get_velocity() == velocity
    assert type(state.get_frame()) is Frame
    assert state.get_frame() == Frame.GCRF()

    in_itrf = state.in_frame(Frame.ITRF())

    assert in_itrf.get_frame() == Frame.ITRF()
    assert type(in_itrf.get_instant() + Duration.seconds(1.0)) is Instant


def test_physics_shared_pointers_and_environment():
    environment = Environment.default()
    earth = environment.get_celestial_object_with_name("Earth")

    assert isinstance(earth, Celestial)

    orbit = Orbit.circular(
        Instant.J2000(),
        physics.unit.Length.kilometers(500.0),
        physics.unit.Angle.degrees(45.0),
        earth,
    )

    assert isinstance(orbit.get_celestial_object(), Celestial)
    assert orbit.get_celestial_object().get_name() == earth.get_name()
    assert isinstance(orbit.get_model(), Kepler)

    states = orbit.get_states_at(
        Interval.closed(
            Instant.J2000(), Instant.J2000() + Duration.minutes(1.0)
        ).generate_grid(Duration.seconds(30.0))
    )

    assert isinstance(states, list)
    assert len(states) == 3
    assert all(isinstance(state, State) for state in states)


def test_core_string_arguments_and_return_values():
    satellite_system = SatelliteSystem.default()

    assert satellite_system is not None

    orbit = Orbit.circular(
        Instant.J2000(),
        physics.unit.Length.kilometers(500.0),
        physics.unit.Angle.degrees(45.0),
        Environment.default().get_celestial_object_with_name(String("Earth")),
    )

    assert orbit is not None


def test_mathematics_arguments_and_return_values():
    criterion = VisibilityCriterion.from_aer_interval(
        RealInterval.closed(0.0, 360.0),
        RealInterval.closed(0.0, 90.0),
        RealInterval.closed(0.0, 1.0e10),
    )

    assert criterion.is_aer_interval()

    aer_interval = criterion.as_aer_interval()

    assert type(aer_interval.azimuth) is RealInterval

    geometry = Composite(Cuboid.cube(Point(0.0, 0.0, 0.0), 1.0))

    satellite_system = SatelliteSystem(
        Mass.kilograms(100.0),
        geometry,
        np.eye(3),
        1.0,
        2.1,
    )

    assert type(satellite_system.get_geometry()) is Composite
    assert satellite_system.get_geometry() == geometry

    state = State(
        Instant.J2000(),
        Position.meters([7.0e6, 0.0, 0.0], Frame.GCRF()),
        Velocity.meters_per_second([0.0, 7.5e3, 0.0], Frame.GCRF()),
        Quaternion.unit(),
        [0.0, 0.0, 0.0],
        Frame.GCRF(),
    )

    assert type(state.get_attitude()) is Quaternion
    assert state.get_attitude() == Quaternion.unit()
