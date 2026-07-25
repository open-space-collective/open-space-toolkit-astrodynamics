# How to configure the forces on a spacecraft

The forces acting on a spacecraft are modelled as a list of independent
**dynamics** contributions. A `Propagator` integrates whatever list you give it,
so configuring a force model means picking and combining the terms your analysis
needs.

This guide assumes you can already [propagate an orbit](../tutorials/numerical-propagation).

## Let the environment assemble the standard set

The quickest route is to let the environment build the dynamics for you. It adds
central-body gravity for the primary body, third-body gravity for every other
celestial object, atmospheric drag for any body with an atmosphere model, and a
position derivative:

```python
from ostk.astrodynamics import Dynamics

dynamics = Dynamics.from_environment(environment)
print([d.get_name() for d in dynamics])
```

With `Environment.default()` (Earth, Sun and Moon) this yields central-body
gravity for Earth, third-body gravity for the Sun and Moon, and the position
derivative. Drag appears only if the Earth was built with an atmospheric model.

## Assemble the dynamics explicitly

For full control over which effects are active, build the list yourself:

```python
from ostk.physics.environment.object.celestial import Sun, Moon
from ostk.astrodynamics.dynamics import (
    CentralBodyGravity,
    ThirdBodyGravity,
    AtmosphericDrag,
    PositionDerivative,
)

dynamics = [
    CentralBodyGravity(earth),        # oblate Earth gravity
    ThirdBodyGravity(Sun.default()),  # solar point-mass perturbation
    ThirdBodyGravity(Moon.default()), # lunar point-mass perturbation
    AtmosphericDrag(earth),           # requires mass / area / Cd on the state
    PositionDerivative(),             # links velocity to position — always include it
]
```

Each term takes the celestial object it applies to (`PositionDerivative` takes
nothing). Include exactly one `PositionDerivative`; without it the integrator has
no d(position)/dt.

```{important}
`AtmosphericDrag` reads the spacecraft mass, cross-sectional area and drag
coefficient from the state. Build the state with those coordinate subsets (see
the [propagation tutorial](../tutorials/numerical-propagation)); otherwise
propagation fails with a missing-subset error.
```

## Feed the dynamics to a propagator

```python
from ostk.astrodynamics.trajectory import Propagator
from ostk.astrodynamics.trajectory.state import NumericalSolver

propagator = Propagator(NumericalSolver.default(), dynamics)
final_state = propagator.calculate_state_at(state, epoch + Duration.days(1.0))
```

## Replay a recorded acceleration profile

To inject a time series of accelerations — for example a maneuver reconstructed
from telemetry — use the `Tabulated` dynamics. It interpolates a profile of
contributions onto the integration grid:

```python
from ostk.astrodynamics.dynamics import Tabulated
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.mathematics.curve_fitting import Interpolator

tabulated = Tabulated(
    instants,                          # list[Instant]
    contribution_profile,             # ndarray [n_instants x n_coordinates]
    [CartesianVelocity.default()],    # the subsets the columns map to
    Frame.GCRF(),
    Interpolator.Type.BarycentricRational,
)
dynamics.append(tabulated)
```

```{seealso}
- Add a force the toolkit doesn't ship: [Write a custom dynamics term](custom-dynamics).
- Drive thrust from a control law: [Write a custom guidance law](custom-guidance-law).
- Concepts behind dynamics and propagation: [Concepts](../explanation/concepts).
```
