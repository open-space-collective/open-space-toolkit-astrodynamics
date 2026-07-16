# Propagating an orbit with realistic forces

In [Getting started](getting-started) you built a Keplerian orbit — pure
two-body motion. Real spacecraft feel more than a point-mass Earth: the planet
is oblate, the Sun and Moon tug on them, and in low orbit the atmosphere drags
them down. In this tutorial you will assemble those forces into a **numerical
propagator** and watch a satellite's altitude decay under drag.

By the end you will understand how a state, a set of dynamics and a numerical
solver combine into a `Propagator` — the machinery that every high-fidelity
analysis in the toolkit is built on.

## What you need

- The package installed (`pip install open-space-toolkit-astrodynamics`).
- Ten minutes and a Python REPL.
- Familiarity with [Getting started](getting-started).

## Step 1 — Build an environment with an atmosphere

Two-body motion needs nothing but a gravitational parameter. A realistic force
model needs *models*: a detailed gravity field and an atmosphere. Build an Earth
that carries both, and put it in an environment:

```python
from ostk.core.filesystem import Directory
from ostk.physics import Environment
from ostk.physics.time import Instant, DateTime, Scale
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.environment.magnetic import Earth as EarthMagneticModel
from ostk.physics.environment.atmospheric import Earth as EarthAtmosphericModel

epoch = Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC)

gravity = EarthGravitationalModel(
    EarthGravitationalModel.Type.EGM2008, Directory.undefined(), 20, 20
)  # EGM2008 truncated to degree/order 20
magnetic = EarthMagneticModel(EarthMagneticModel.Type.Undefined)
atmosphere = EarthAtmosphericModel(EarthAtmosphericModel.Type.NRLMSISE00)

earth = Earth.from_models(gravity, magnetic, atmosphere)
environment = Environment(epoch, [earth])
```

## Step 2 — Describe the spacecraft's state

Drag depends on the spacecraft, not just the orbit: its **mass**, **cross-sectional
area** and **drag coefficient**. So the state must carry those extra quantities
alongside position and velocity.

Start from a Sun-synchronous orbit for the position and velocity, then use a
`StateBuilder` to append the ballistic properties:

```python
import numpy as np

from ostk.physics.unit import Length
from ostk.physics.time import Time
from ostk.physics.coordinate import Frame
from ostk.astrodynamics.trajectory import Orbit, StateBuilder
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    CartesianPosition,
    CartesianVelocity,
)

orbit = Orbit.sun_synchronous(epoch, Length.kilometers(450.0), Time(22, 30, 0), earth)
initial = orbit.get_state_at(epoch)

builder = StateBuilder(
    Frame.GCRF(),
    [
        CartesianPosition.default(),
        CartesianVelocity.default(),
        CoordinateSubset.mass(),             # kg
        CoordinateSubset.surface_area(),     # m^2
        CoordinateSubset.drag_coefficient(),
    ],
)
state = builder.build(
    epoch,
    np.append(initial.get_coordinates(), [150.0, 1.2, 2.2]),  # 150 kg, 1.2 m^2, Cd = 2.2
)
```

## Step 3 — Assemble the dynamics

The forces acting on the spacecraft are a *list* of dynamics contributions. You
could build them by hand, but the environment already knows what it contains, so
let it assemble the standard set for you:

```python
from ostk.astrodynamics import Dynamics

dynamics = Dynamics.from_environment(environment)
print([d.get_name() for d in dynamics])
# ['Central Body Gravity [Earth]', 'Atmospheric Drag [Earth]', 'Position Derivative']
```

You get central-body gravity, atmospheric drag (because the Earth has an
atmospheric model), and the position derivative that links velocity to position.

## Step 4 — Choose a numerical solver and propagate

A numerical solver integrates the equations of motion. The default is a good
adaptive Runge–Kutta scheme:

```python
from ostk.physics.time import Duration, Interval
from ostk.astrodynamics.trajectory import Propagator
from ostk.astrodynamics.trajectory.state import NumericalSolver

propagator = Propagator(NumericalSolver.default(), dynamics)

grid = Interval.closed(epoch, epoch + Duration.days(10.0)).generate_grid(
    Duration.hours(6.0)
)
states = propagator.calculate_states_at(state, grid)
```

## Step 5 — Watch the orbit decay

Instantaneous (osculating) altitude wobbles with every orbit because of the
Earth's oblateness. To see the *secular* trend — the slow decay from drag —
convert each state to **mean** elements and read the mean semi-major axis:

```python
from ostk.astrodynamics.trajectory.orbit.model.brouwerLyddaneMean import (
    BrouwerLyddaneMeanLong,
)

mu = earth.get_gravitational_parameter()
equatorial_radius_km = float(earth.get_equatorial_radius().in_kilometers())


def mean_altitude_km(state):
    mean = BrouwerLyddaneMeanLong.cartesian(
        (state.get_position(), state.get_velocity()), mu
    )
    return float(mean.get_semi_major_axis().in_kilometers()) - equatorial_radius_km


print(f"day  0: {mean_altitude_km(states[0]):.3f} km")
print(f"day 10: {mean_altitude_km(states[-1]):.3f} km")
```

At 450 km the atmosphere is thin but not negligible: over ten days the mean
altitude drops by more than a kilometre. Lower the starting altitude, or extend
the interval, and the decay accelerates — exactly the behaviour you would expect.

```{note}
Mean altitude sits a few kilometres *below* the osculating altitude you started
from — that offset is the J2 signature, not an error. Use mean elements for
clean secular trends and osculating elements for instantaneous geometry.
```

## What you learned

You have:

- built an environment with a detailed gravity field and an atmosphere;
- described a spacecraft state carrying mass, area and drag coefficient;
- assembled dynamics automatically from the environment;
- propagated numerically and observed drag-driven orbital decay.

This `state → dynamics → propagator` pattern is the backbone of the how-to
guides that follow.

## Where to go next

```{seealso}
- [Configure the forces on a spacecraft](../how-to/configure-dynamics) — pick
  and combine dynamics terms explicitly.
- [Write a custom dynamics term](../how-to/custom-dynamics) — add a force the
  toolkit doesn't ship.
- {doc}`Plan a station-keeping maneuver </notebooks/Mission Analysis/Station-Keeping Sequence>`
  — put thrust into the propagation.
```
