# How-to guides

How-to guides are **task-oriented**. Each one is a short, practical recipe that
answers a specific question of the form *"How do I …?"*. They assume you
already know the basics — if you don't, start with the
[tutorials](../tutorials/index).

Most of these guides are runnable Jupyter notebooks: you can read them here or
open the originals under [`notebooks/`](https://github.com/open-space-collective/open-space-toolkit-astrodynamics/tree/main/notebooks)
in the repository and execute them yourself.

## Installation

```{toctree}
:maxdepth: 1

install
```

## Flight dynamics

Computing orbits, accesses and events for a single spacecraft.

```{toctree}
:maxdepth: 1
:caption: Flight dynamics

Access Computation </notebooks/Flight Dynamics/Access Computation>
Constant Thrust Dynamics </notebooks/Flight Dynamics/Constant Thrust Dynamics>
Custom Event Condition </notebooks/Flight Dynamics/Custom Event Condition>
Custom Window Generation </notebooks/Flight Dynamics/Custom Window Generation>
Find RF Interference Accesses </notebooks/Flight Dynamics/Find RF Interference Accesses>
Orbit Determination </notebooks/Flight Dynamics/Orbit Determination>
TLE Determination </notebooks/Flight Dynamics/TLE Determination>
Visualize a Satellite Trajectory (Cesium) </notebooks/Flight Dynamics/Visualize Satellite Trajectory (Cesium)>
```

## Mission analysis

Manoeuvre design, attitude/pointing profiles and atmospheric modelling.

```{toctree}
:maxdepth: 1
:caption: Mission analysis

Atmospheric Density with Constant Flux (NRLMSISE-00) </notebooks/Mission Analysis/Constant Flux and Geomagnetic Index NRLMSISE-00>
Nadir Tracking (With Yaw Compensation) </notebooks/Mission Analysis/Nadir Tracking (With Yaw Compensation)>
Target Tracking </notebooks/Mission Analysis/Target Tracking>
Target Tracking (With Yaw Compensation) </notebooks/Mission Analysis/Target Tracking (With Yaw Compensation)>
Station-keeping with QLaw </notebooks/Mission Analysis/QLaw Examples>
Station-keeping Sequence </notebooks/Mission Analysis/Station-Keeping Sequence>
Altitude + Eccentricity Station-keeping Sequence </notebooks/Mission Analysis/Station-Keeping Sequence [QLaw]>
```

## Sensor modelling

Projecting sensor geometry onto the ground and computing coverage.

```{toctree}
:maxdepth: 1
:caption: Sensor modelling

Sensor Modeling </notebooks/Sensor Modeling/Sensor Modeling>
Area Coverage </notebooks/Sensor Modeling/Area Coverage>
```

## Cross-platform validation

Reproducing mission-sequence scenarios and comparing Open Space Toolkit against
[GMAT](https://gmat.gsfc.nasa.gov/) and [Orekit](https://www.orekit.org/).

```{toctree}
:maxdepth: 1
:caption: Cross-platform validation

Mission Sequence — Scenario 1 </notebooks/Cross Platform Validation/OSTk Cross Platform Validation (Mission Sequence Scenario 1)>
Mission Sequence — Scenario 2 </notebooks/Cross Platform Validation/OSTk Cross Platform Validation (Mission Sequence Scenario 2)>
Mission Sequence — Scenario 3 </notebooks/Cross Platform Validation/OSTk Cross Platform Validation (Mission Sequence Scenario 3)>
Mission Sequence — Scenario 4 </notebooks/Cross Platform Validation/OSTk Cross Platform Validation (Mission Sequence Scenario 4)>
```

```{seealso}
For the concepts behind these recipes — trajectories, orbits, dynamics,
accesses and sequences — see [Concepts](../explanation/concepts).
```
