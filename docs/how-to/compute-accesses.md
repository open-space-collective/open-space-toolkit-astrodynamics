# How to compute accesses to a ground station

An **access** is an interval during which an observer can see a target subject to
a visibility criterion — a ground station watching a satellite rise above a
minimum elevation, for example. This guide computes those intervals and then
shows how to make the computation much faster for long horizons.

For a fuller, runnable walkthrough see the
[Access Computation](/notebooks/Flight Dynamics/Access Computation) notebook.

## Define a visibility criterion

The criterion describes *when* the target is considered visible. Build it from an
azimuth–elevation–range interval for a realistic ground station, or from an
elevation interval alone for a simple mask:

```python
from ostk.mathematics.object import RealInterval
from ostk.astrodynamics.access import VisibilityCriterion

criterion = VisibilityCriterion.from_aer_interval(
    RealInterval.closed(0.0, 360.0),      # azimuth [deg]
    RealInterval.closed(10.0, 90.0),      # elevation [deg]  — 10° horizon mask
    RealInterval.closed(0.0, 10000.0e3),  # range [m]
)
```

Other constructors: `VisibilityCriterion.from_elevation_interval(...)` (elevation
only), `from_aer_mask(...)` (an azimuth-dependent elevation mask), and
`from_line_of_sight(environment)` (occlusion by celestial bodies, for
satellite-to-satellite links).

## Define the target and the observer

Attach the criterion to an `AccessTarget`. A ground station is fixed at a
latitude/longitude/altitude:

```python
from ostk.physics import Environment
from ostk.physics.unit import Length, Angle
from ostk.physics.coordinate.spherical import LLA
from ostk.astrodynamics.access import AccessTarget

environment = Environment.default()
earth = environment.access_celestial_object_with_name("Earth")

ground_station = AccessTarget.from_lla(
    criterion,
    LLA(Angle.degrees(50.0), Angle.degrees(20.0), Length.meters(30.0)),
    earth,
)
```

The moving object you are observing (here a satellite) is an ordinary trajectory:

```python
from ostk.physics.time import Instant, DateTime, Scale, Time
from ostk.astrodynamics.trajectory import Orbit

epoch = Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC)
satellite = Orbit.sun_synchronous(epoch, Length.kilometers(500.0), Time(12, 0, 0), earth)
```

## Compute the accesses

Create a `Generator` from the environment and compute accesses over an interval:

```python
from ostk.physics.time import Duration, Interval
from ostk.astrodynamics.access import Generator

interval = Interval.closed(epoch, epoch + Duration.days(1.0))

generator = Generator(environment=environment)
accesses = generator.compute_accesses(
    interval=interval,
    access_target=ground_station,
    to_trajectory=satellite,
)

for access in accesses:
    print(
        access.get_acquisition_of_signal().get_date_time(Scale.UTC),
        f"{float(access.get_duration().in_seconds()):.0f} s",
        f"{float(access.get_max_elevation().in_degrees()):.1f} deg",
    )
```

Each `Access` exposes `get_acquisition_of_signal()` (AOS),
`get_time_of_closest_approach()` (TCA), `get_loss_of_signal()` (LOS),
`get_interval()`, `get_duration()` and `get_max_elevation()`. Pass a list of
targets to `compute_accesses` to get a list of access lists, one per target.

## Enhance performance with a tabulated trajectory in ITRF

Ground stations are fixed in the Earth-fixed **ITRF** frame, but a satellite
orbit produces its states in the inertial GCRF frame. At every sample instant —
and again during the sub-second refinement of each rise/set crossing — the
generator transforms the satellite position into ITRF, and that transform
recomputes the full Earth-orientation rotation (precession, nutation, Earth
rotation, polar motion). Over days of coverage this dominates the runtime.

If the trajectory you hand the generator **already produces ITRF states**, that
per-step transform collapses to a no-op. So propagate once, re-express the states
in ITRF, and wrap them in a `Tabulated` trajectory model whose output frame is
ITRF:

```python
from ostk.physics.coordinate import Frame
from ostk.mathematics.curve_fitting import Interpolator
from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory.model import Tabulated

# 1. Propagate (or otherwise obtain) the satellite states, then express them in ITRF.
grid = interval.generate_grid(Duration.seconds(30.0))
itrf_states = [state.in_frame(Frame.ITRF()) for state in satellite.get_states_at(grid)]

# 2. Build a tabulated trajectory that interpolates and outputs in ITRF.
tabulated = Tabulated(
    itrf_states,
    Interpolator.Type.CubicSpline,
    output_frame=Frame.ITRF(),
)
fast_satellite = Trajectory(tabulated)

# 3. Feed the ITRF trajectory to the generator — same result, far fewer frame transforms.
accesses = generator.compute_accesses(
    interval=interval,
    access_target=ground_station,
    to_trajectory=fast_satellite,
)
```

```{important}
Interpolate in the frame you output in. Because the states are both stored and
returned in ITRF, and the ground target is already ITRF-fixed, the whole geometry
pipeline stays in ITRF and no Earth-orientation rotation is computed per step.
Feeding GCRF states and merely labelling the output frame ITRF would be
incorrect.
```

This trades one bulk frame transform (building `itrf_states`) for the thousands
the generator would otherwise perform, and pays off whenever you compute accesses
over long intervals or against many ground targets.

```{seealso}
- End-to-end example with plots: [Access Computation](/notebooks/Flight Dynamics/Access Computation).
- Interference geometry: [Find RF Interference Accesses](/notebooks/Flight Dynamics/Find RF Interference Accesses).
- The concepts behind trajectories, frames and accesses: [Concepts](../explanation/concepts).
```
