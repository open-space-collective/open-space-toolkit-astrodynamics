# How to find close approaches

A **close approach** is the moment two objects come nearest each other. Given two
trajectories, the conjunction `Generator` finds every close approach in an
interval and reports the time and miss distance — the starting point for
collision-avoidance screening.

## Set up two trajectories

Any two trajectories will do — propagated orbits, TLEs, or tabulated ephemerides.
Here are two circular orbits on crossing planes:

```python
from ostk.physics import Environment
from ostk.physics.unit import Length, Angle
from ostk.physics.time import Instant, DateTime, Scale, Duration, Interval
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.orbit.model import Kepler
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE

environment = Environment.default()
earth = environment.access_celestial_object_with_name("Earth")
epoch = Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC)


def circular_orbit(inclination_deg, true_anomaly_deg):
    return Orbit(
        Kepler(
            COE(
                Length.kilometers(7000.0),
                0.0,
                Angle.degrees(inclination_deg),
                Angle.degrees(0.0),
                Angle.degrees(0.0),
                Angle.degrees(true_anomaly_deg),
            ),
            epoch,
            earth,
            Kepler.PerturbationType.No,
        ),
        earth,
    )


primary = circular_orbit(0.0, 0.0)
secondary = circular_orbit(90.0, 0.001)
```

## Compute the close approaches

Build a `Generator` from the primary ("reference") trajectory, then screen the
secondary against it over an interval:

```python
from ostk.astrodynamics.conjunction.close_approach import Generator

generator = Generator(
    reference_trajectory=primary,
    step=Duration.minutes(20.0),        # coarse search step
    tolerance=Duration.milliseconds(1.0),  # refinement tolerance for the TCA
)

close_approaches = generator.compute_close_approaches(
    trajectory=secondary,
    search_interval=Interval.closed(epoch, epoch + Duration.hours(3.0)),
)
```

The `step` is the coarse sampling used to bracket each approach; the `tolerance`
controls how precisely the time of closest approach (TCA) is refined. Returns an
empty list when there are no approaches in the interval.

## Read each close approach

```python
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.physics.coordinate import Frame

for ca in close_approaches:
    tca = ca.get_instant()
    miss = float(ca.get_miss_distance().in_meters())
    print(tca.get_date_time(Scale.UTC), f"{miss:.1f} m")

    # Miss-distance components in the primary's radial / in-track / cross-track frame
    radial, in_track, cross_track = ca.compute_miss_distance_components_in_local_orbital_frame(
        local_orbital_frame_factory=LocalOrbitalFrameFactory.QSW(Frame.GCRF()),
    )
```

Besides `get_instant()` and `get_miss_distance()`, each `CloseApproach` exposes
`get_object_1_state()` / `get_object_2_state()` (both objects at the TCA),
`get_relative_state()` (object 2 relative to object 1), and
`compute_miss_distance_components_in_frame(frame)` for components in an arbitrary
frame.

```{note}
The CCSDS Conjunction Data Message (CDM) is modelled separately in
`ostk.astrodynamics.conjunction.message.ccsds`. It parses and represents CDMs
exchanged with conjunction data providers and is independent of the `Generator`
flow above — use it when you need to read or produce that message format.
```

```{seealso}
- The concepts behind trajectories and local orbital frames: [Concepts](../explanation/concepts).
```
