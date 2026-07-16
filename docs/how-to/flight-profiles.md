# How to build a flight profile

A **flight profile** describes a spacecraft's attitude over time — where it
points. Given an orbit, a `Profile` produces a full state at any instant,
including the attitude quaternion and angular velocity. This guide covers the
common pointing modes.

The {doc}`Nadir Tracking </notebooks/Mission Analysis/Nadir Tracking (With Yaw Compensation)>`
and {doc}`Target Tracking </notebooks/Mission Analysis/Target Tracking>` notebooks
show these profiles applied end to end.

## Nadir pointing

The simplest profile keeps the spacecraft aligned with a local orbital frame.
Use the VVLH frame (its Z axis points toward nadir) for straightforward nadir
pointing:

```python
from ostk.physics import Environment
from ostk.physics.time import Instant, DateTime, Scale, Time
from ostk.physics.unit import Length
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.flight import Profile

epoch = Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC)
environment = Environment.default()
earth = environment.access_celestial_object_with_name("Earth")

orbit = Orbit.sun_synchronous(epoch, Length.kilometers(500.0), Time(14, 0, 0), earth)

profile = Profile.local_orbital_frame_pointing(orbit, Orbit.FrameType.VVLH)
```

Other frame types include `LVLH`, `QSW`, `TNW`, `VNC` and `NED` — pick whichever
matches your convention.

## Read the attitude

Query the profile like any trajectory. A flight state adds attitude information:

```python
state = profile.get_state_at(epoch)

attitude = state.get_attitude()             # Quaternion: reference frame -> body frame
angular_velocity = state.get_angular_velocity()   # numpy array, body-frame rad/s
```

`profile.get_states_at([...])` returns a list, and `profile.get_axes_at(instant)`
gives the body axes directly.

## Pointing at a target with yaw compensation

For Earth-observation attitudes you usually want one body axis at a target and
another constrained (say, along the velocity) to fix the roll about the boresight.
Build such a profile with `custom_pointing`, supplying an **alignment** target and
a **clocking** target.

For nadir pointing with the along-track axis clocked to the ground-relative
velocity (yaw-compensated), point at a nadir trajectory:

```python
from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory.model import Nadir

nadir_trajectory = Trajectory(Nadir(orbit))

profile = Profile.custom_pointing(
    orbit=orbit,
    alignment_target=Profile.TrajectoryTarget.target_position(
        trajectory=nadir_trajectory, axis=Profile.Axis.Z
    ),
    clocking_target=Profile.TrajectoryTarget.target_velocity(
        trajectory=nadir_trajectory, axis=Profile.Axis.X
    ),
)
```

To track a fixed point on the ground instead, point the boresight at that
target's trajectory and clock against the orbital momentum:

```python
profile = Profile.custom_pointing(
    orbit=orbit,
    alignment_target=Profile.TrajectoryTarget.target_position(
        trajectory=ground_target_trajectory, axis=Profile.Axis.Z
    ),
    clocking_target=Profile.Target(
        type=Profile.TargetType.OrbitalMomentum,
        axis=Profile.Axis.Y,
        anti_direction=True,
    ),
)
```

Targets can also point at the `Sun`, `Moon`, `VelocityECI`, a `GeodeticNadir`,
or a fully custom direction via `Profile.CustomTarget(lambda state: [...], axis)`.

## Inertial pointing

To hold a fixed orientation in inertial space, give `inertial_pointing` a
quaternion:

```python
from ostk.mathematics.geometry.d3.transformation.rotation import Quaternion

# An Orbit is already a Trajectory, so it can be passed directly.
profile = Profile.inertial_pointing(orbit, Quaternion.unit())
```

```{seealso}
- Applied pointing with plots and yaw compensation:
  {doc}`Nadir Tracking </notebooks/Mission Analysis/Nadir Tracking (With Yaw Compensation)>`
  and {doc}`Target Tracking </notebooks/Mission Analysis/Target Tracking>`.
- How profiles fit alongside orbits and trajectories: [Concepts](../explanation/concepts).
```
