# Getting started

In this tutorial you will take your first steps with Open Space Toolkit
Astrodynamics. By the end you will have set up a physical environment, described
an orbit with classical orbital elements, propagated it, and read off a
spacecraft state. You do not need to know any C++ — everything here uses the
Python bindings.

This is a lesson, not a task list: follow along and run each snippet as you go.

## What you need

- Python 3.9 or later
- The package installed:

  ```bash
  pip install open-space-toolkit-astrodynamics
  ```

Start an interactive session so you can experiment as you read:

```bash
python
```

## Step 1 — Import the toolkit

Astrodynamics lives under the `ostk.astrodynamics` package, and it builds on
the [physics](https://open-space-collective.github.io/open-space-toolkit-physics)
layer for time, units and reference frames. Import the pieces you will use:

```python
from ostk.physics import Environment
from ostk.physics.time import Instant, Scale, Duration
from ostk.physics.unit import Length, Angle

from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.orbit.model import Kepler
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE
```

If those imports succeed, you are ready to go.

```{note}
The first time you construct an environment, the toolkit downloads the physical
data it needs (ephemerides, gravity models, …) into a local cache. This happens
automatically and only once.
```

## Step 2 — Set up the environment

Almost everything in astrodynamics happens relative to a central body. The
default environment gives you the Earth (plus the Sun and Moon) already
configured:

```python
environment = Environment.default()
earth = environment.access_celestial_object_with_name("Earth")
```

## Step 3 — Describe an orbit

Classical orbital elements (COE) are a compact way to describe an orbit: its
size, shape and orientation. Here is a near-circular, Sun-synchronous-like orbit
at 7000 km:

```python
coe = COE(
    Length.kilometers(7000.0),  # semi-major axis
    0.0,                        # eccentricity
    Angle.degrees(98.0),        # inclination
    Angle.degrees(0.0),         # right ascension of the ascending node
    Angle.degrees(0.0),         # argument of periapsis
    Angle.degrees(0.0),         # true anomaly
)
```

Pin the orbit to an epoch and wrap it in a two-body (Keplerian) model:

```python
import datetime

epoch = Instant.date_time(datetime.datetime(2023, 1, 1, 0, 0, 0), Scale.UTC)

kepler = Kepler(coe, epoch, earth, Kepler.PerturbationType.No)
orbit = Orbit(kepler, earth)
```

## Step 4 — Propagate and read a state

An `Orbit` is a trajectory: ask it for the spacecraft state at any instant and
it propagates for you. Let's look 45 minutes after the epoch — roughly half an
orbit later:

```python
state = orbit.get_state_at(epoch + Duration.minutes(45.0))

position = state.get_position()
velocity = state.get_velocity()

print(position.get_coordinates())              # [x, y, z] in metres
print(position.access_frame().get_name())      # "GCRF"
print(velocity.get_coordinates())              # [vx, vy, vz] in m/s
```

The coordinates are expressed in the Earth-centred inertial GCRF frame. You have
just propagated an orbit.

## What you learned

You have:

- imported `ostk.astrodynamics` and the `ostk.physics` layers it builds on;
- created a physical environment with the Earth as the central body;
- described an orbit with classical orbital elements and pinned it to an epoch;
- propagated the orbit and read off a position and velocity.

## Where to go next

```{seealso}
- Continue with the {doc}`Orbit computation </notebooks/Flight Dynamics/Orbit Computation>`
  notebook, which propagates a real satellite from a Two-Line Element set.
- Solve concrete problems with the [how-to guides](../how-to/index) — for example
  {doc}`computing accesses </notebooks/Flight Dynamics/Access Computation>` between
  a ground station and a satellite.
- Look up every class and method in the [reference](../reference/index).
- Understand *how the pieces fit together* in
  [Concepts](../explanation/concepts).
```
