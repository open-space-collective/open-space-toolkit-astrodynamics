# How to write a custom dynamics term

When you need a force the toolkit doesn't ship — solar radiation pressure with
your own shadow model, a differential-drag control law, an empirical
acceleration — subclass `Dynamics` in Python and drop it into the dynamics list
like any built-in term.

This guide assumes you are comfortable [configuring dynamics](configure-dynamics)
and [propagating an orbit](../tutorials/numerical-propagation).

## The interface to implement

A `Dynamics` subclass must implement four methods:

| Method | Purpose |
| --- | --- |
| `is_defined(self)` | Return `True` when the term is usable. |
| `get_read_coordinate_subsets(self)` | The state quantities this term reads. |
| `get_write_coordinate_subsets(self)` | The derivative quantities this term contributes to. |
| `compute_contribution(self, instant, state_vector, frame)` | Return the contribution vector. |

The `state_vector` passed to `compute_contribution` is a 1-D NumPy array laid out
in the order of `get_read_coordinate_subsets()`, expressed in `frame`. The array
you return must match `get_write_coordinate_subsets()` in order and length, also
in `frame`. Remember to call `super().__init__(name)` — the base class needs a
name.

## Example: a constant along-track acceleration

This term reads the velocity and contributes a small acceleration aligned with
it (a crude model of continuous low thrust or a drag-make-up force):

```python
import numpy as np

from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity


class ConstantAlongTrackAcceleration(Dynamics):
    """A constant acceleration aligned with the velocity vector."""

    def __init__(self, acceleration: float = 1e-6, name: str = "AlongTrackAccel"):
        super().__init__(name)                 # required: give the base a name
        self._acceleration = acceleration      # m/s^2

    def is_defined(self):
        return True

    def get_read_coordinate_subsets(self):
        # `state_vector` will hold the 3 velocity components, in this order
        return [CartesianVelocity.default()]

    def get_write_coordinate_subsets(self):
        # the returned vector holds 3 acceleration components, in this order
        return [CartesianVelocity.default()]

    def compute_contribution(self, instant, state_vector, frame):
        velocity = np.asarray(state_vector)
        speed = np.linalg.norm(velocity)
        if speed == 0.0:
            return np.zeros(3)
        return self._acceleration * velocity / speed
```

## Use it in a propagator

Add the instance to the dynamics list alongside the built-in terms:

```python
from ostk.astrodynamics.dynamics import CentralBodyGravity, PositionDerivative
from ostk.astrodynamics.trajectory import Propagator
from ostk.astrodynamics.trajectory.state import NumericalSolver

dynamics = [
    CentralBodyGravity(earth),
    PositionDerivative(),
    ConstantAlongTrackAcceleration(acceleration=1e-6),
]

propagator = Propagator(NumericalSolver.default(), dynamics)
final_state = propagator.calculate_state_at(state, epoch + Duration.hours(1.0))
```

The propagator calls `compute_contribution` at every integration step and sums
your term with the rest of the force model.

```{tip}
Read and write different quantities freely. A solar-radiation-pressure term
might read `CartesianPosition`, mass and area and write into the
`CartesianVelocity` derivative. The read/write subset lists are how the
propagator knows which slice of the state to hand you and where to add your
result.
```

```{seealso}
- Combine your term with the built-in forces: [Configure the forces on a spacecraft](configure-dynamics).
- For thrust specifically, a guidance law is usually a better fit than raw
  dynamics: [Write a custom guidance law](custom-guidance-law).
```
