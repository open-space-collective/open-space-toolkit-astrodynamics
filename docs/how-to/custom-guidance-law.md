# How to write a custom guidance law

A **guidance law** decides the direction (and share of the available magnitude)
of thrust over time. The toolkit ships `ConstantThrust` and `QLaw`, but when you
need your own steering logic you subclass `GuidanceLaw` in Python and drive a
`Thruster` with it.

This guide assumes you can [propagate an orbit](../tutorials/numerical-propagation)
and have seen the built-in laws (see the
{doc}`station-keeping notebook </notebooks/Mission Analysis/Station-Keeping Sequence>`).

## The interface to implement

A `GuidanceLaw` subclass implements one method:

```python
def calculate_thrust_acceleration_at(
    self,
    instant,                 # ostk.physics.time.Instant
    position_coordinates,    # numpy array [x, y, z], in output_frame
    velocity_coordinates,    # numpy array [vx, vy, vz], in output_frame
    thrust_acceleration,     # scalar magnitude = thrust / current mass
    output_frame,            # ostk.physics.coordinate.Frame
) -> np.ndarray:             # return a length-3 acceleration vector, in output_frame
    ...
```

The `thrust_acceleration` scalar is supplied by the `Thruster` at propagation
time — it is the engine thrust divided by the spacecraft's current mass. Your law
chooses a *direction* and scales it by that magnitude. Call `super().__init__(name)`
in your constructor.

## Example: constant inertial thrust direction

```python
import numpy as np

from ostk.astrodynamics import GuidanceLaw


class ConstantInertialThrust(GuidanceLaw):
    """Thrust along a fixed unit direction in the output (inertial) frame."""

    def __init__(self, direction, name="Constant Inertial Thrust"):
        super().__init__(name)
        direction = np.asarray(direction, dtype=float)
        self._direction = direction / np.linalg.norm(direction)

    def calculate_thrust_acceleration_at(
        self,
        instant,
        position_coordinates,
        velocity_coordinates,
        thrust_acceleration,
        output_frame,
    ):
        return thrust_acceleration * self._direction
```

Your logic can use any of the arguments — steer relative to the velocity for an
along-track burn, toward a target derived from `position_coordinates`, or vary
the direction with `instant` for a time-scheduled profile.

## Drive a Thruster with it

A guidance law becomes a force through a `Thruster`, which needs a
`SatelliteSystem` (mass, geometry, and a `PropulsionSystem` giving thrust and
specific impulse):

```python
import numpy as np

from ostk.physics.unit import Mass
from ostk.mathematics.geometry.d3.object import Cuboid, Composite, Point
from ostk.astrodynamics.dynamics import Thruster
from ostk.astrodynamics.flight.system import SatelliteSystem, PropulsionSystem

geometry = Composite(
    Cuboid(Point(0.0, 0.0, 0.0), [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]], [0.5, 0.5, 0.5])
)
satellite_system = SatelliteSystem(
    Mass.kilograms(100.0),          # dry mass
    geometry,
    np.eye(3),                      # inertia tensor
    1.0,                            # cross-sectional area [m^2]
    2.2,                            # drag coefficient
    PropulsionSystem(1.0, 150.0),   # thrust [N], specific impulse [s]
)

thruster = Thruster(satellite_system, ConstantInertialThrust([1.0, 0.0, 0.0]))
```

Because the thruster consumes propellant, the propagation state must carry the
spacecraft **mass** in addition to position and velocity:

```python
from ostk.physics.coordinate import Frame
from ostk.astrodynamics.dynamics import CentralBodyGravity, PositionDerivative
from ostk.astrodynamics.trajectory import Propagator, StateBuilder
from ostk.astrodynamics.trajectory.state import NumericalSolver, CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    CartesianPosition,
    CartesianVelocity,
)

builder = StateBuilder(
    Frame.GCRF(),
    [CartesianPosition.default(), CartesianVelocity.default(), CoordinateSubset.mass()],
)
state = builder.build(epoch, np.append(initial.get_coordinates(), [110.0]))  # 110 kg total

propagator = Propagator(
    NumericalSolver.default(),
    [CentralBodyGravity(earth), PositionDerivative(), thruster],
)
burned = propagator.calculate_state_at(state, epoch + Duration.minutes(10.0))
print("mass after burn:", float(burned.get_coordinates()[6]), "kg")
```

The propagator integrates the thrust acceleration *and* the mass flow, so the
spacecraft mass decreases as it burns. When the mass reaches the dry mass the
thruster raises an out-of-fuel error.

```{tip}
To schedule different laws over different intervals — a constant burn followed by
a Q-law targeting phase, say — wrap them in
`HeterogeneousGuidanceLaw([(law_a, interval_a), (law_b, interval_b)])`. For
full maneuver campaigns, drive the thruster from a
{doc}`Sequence </notebooks/Mission Analysis/Station-Keeping Sequence>` of
thrust and coast segments.
```

```{seealso}
- Closed-loop element targeting with the built-in Q-law:
  {doc}`Station-keeping with QLaw </notebooks/Mission Analysis/QLaw Examples>`.
- Add non-thrust forces instead: [Write a custom dynamics term](custom-dynamics).
```
