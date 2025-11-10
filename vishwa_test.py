#! /usr/bin/env python3.11

from ostk.core.filesystem import Directory
from ostk.physics.time import Instant, Scale, Duration, Interval
from ostk.physics.coordinate import Frame
from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    CartesianPosition,
    CartesianVelocity,
)
from ostk.astrodynamics.trajectory.state import CoordinateSubset, NumericalSolver
from ostk.astrodynamics.trajectory import State, Propagator, Orbit
from ostk.astrodynamics.trajectory.orbit.model import Propagated
from ostk.physics import Environment
from ostk.physics.environment.object.celestial import Earth, Moon, Sun
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.environment.atmospheric import Earth as EarthAtmosphericModel
from ostk.physics.environment.magnetic import Earth as EarthMagneticModel


instant = Instant.parse("2024-06-09 14:44:30.734.999", Scale.UTC)
frame = Frame.GCRF()

coordinates = [
    3786681.30288918,
    -4897593.40751009,
    -2997836.66403268,
    1124.5962634611,
    -3265.39989654837,
    6779.45043611605,
    100.0,
    1.0,
    1.36738720713048,
]

coordinate_subsets = [
    CartesianPosition.default(),
    CartesianVelocity.default(),
    CoordinateSubset.mass(),
    CoordinateSubset.surface_area(),
    CoordinateSubset.drag_coefficient(),
]

state = State(instant, coordinates, frame, coordinate_subsets)

earth = Earth.from_models(
    EarthGravitationalModel(
        EarthGravitationalModel.Type.EGM96, Directory.undefined(), 70, 70
    ),
    EarthMagneticModel(EarthMagneticModel.Type.Undefined),
    EarthAtmosphericModel(EarthAtmosphericModel.Type.NRLMSISE00),
)
moon = Moon.default()
sun = Sun.default()

environment = Environment(Instant.J2000(), [earth, moon, sun])

numerical_solver = NumericalSolver(
    NumericalSolver.LogType.NoLog,
    NumericalSolver.StepperType.RungeKuttaFehlberg78,
    5.0,
    1.0e-12,
    1.0e-12,
)
propagator = Propagator.from_environment(numerical_solver, environment)

propagated = Propagated(
    propagator,
    [state],
)

physical_orbit = Orbit(
    model=propagated,
    celestial_object=environment.access_celestial_object_with_name("Earth"),
)

start_instant = Instant.parse("2024-06-10 00:00:00.000.000", Scale.UTC)
end_instant = Instant.parse("2024-06-10 01:00:00.000.000", Scale.UTC)

instants = Interval.closed(start_instant, end_instant).generate_grid(
    Duration.seconds(20.0)
)

states = physical_orbit.get_states_at(instants)
