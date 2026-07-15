# How-to guides

How-to guides are **task-oriented**. Each one is a short, practical recipe that
answers a specific question of the form *"How do I …?"*. They assume you
already know the basics — if you don't, start with the
[tutorials](../tutorials/index).

Some guides are hand-written recipes; others are runnable Jupyter notebooks you
can also open under [`notebooks/`](https://github.com/open-space-collective/open-space-toolkit-astrodynamics/tree/main/notebooks)
in the repository and execute yourself.

## Installation

```{toctree}
:maxdepth: 1

install
```

## Propagation & dynamics

Model the forces on a spacecraft and integrate its motion.

```{toctree}
:maxdepth: 1

configure-dynamics
custom-dynamics
Constant thrust dynamics </notebooks/Flight Dynamics/Constant Thrust Dynamics>
Stop on a custom event condition </notebooks/Flight Dynamics/Custom Event Condition>
Generate custom windows </notebooks/Flight Dynamics/Custom Window Generation>
Atmospheric density (NRLMSISE-00) </notebooks/Mission Analysis/Constant Flux and Geomagnetic Index NRLMSISE-00>
Visualize a satellite trajectory (Cesium) </notebooks/Flight Dynamics/Visualize Satellite Trajectory (Cesium)>
```

## Access & visibility

Determine when objects can see one another.

```{toctree}
:maxdepth: 1

compute-accesses
Access computation </notebooks/Flight Dynamics/Access Computation>
Find RF interference accesses </notebooks/Flight Dynamics/Find RF Interference Accesses>
```

## Guidance & maneuvers

Design thrust, guidance and station-keeping campaigns.

```{toctree}
:maxdepth: 1

custom-guidance-law
Station-keeping with QLaw </notebooks/Mission Analysis/QLaw Examples>
Station-keeping sequence </notebooks/Mission Analysis/Station-Keeping Sequence>
Altitude + eccentricity station-keeping [QLaw] </notebooks/Mission Analysis/Station-Keeping Sequence [QLaw]>
```

## Attitude & flight profiles

Point the spacecraft — nadir, target tracking, and custom attitudes.

```{toctree}
:maxdepth: 1

flight-profiles
Nadir tracking (with yaw compensation) </notebooks/Mission Analysis/Nadir Tracking (With Yaw Compensation)>
Target tracking </notebooks/Mission Analysis/Target Tracking>
Target tracking (with yaw compensation) </notebooks/Mission Analysis/Target Tracking (With Yaw Compensation)>
```

## Estimation

Fit orbits and element sets to observations.

```{toctree}
:maxdepth: 1

orbit-determination
solve-tle
Orbit determination </notebooks/Flight Dynamics/Orbit Determination>
TLE determination </notebooks/Flight Dynamics/TLE Determination>
```

## Conjunction analysis

Screen for close approaches between objects.

```{toctree}
:maxdepth: 1

close-approaches
```

## Sensor modelling

Project sensor geometry onto the ground and compute coverage.

```{toctree}
:maxdepth: 1

Sensor modeling </notebooks/Sensor Modeling/Sensor Modeling>
Area coverage </notebooks/Sensor Modeling/Area Coverage>
```

## Cross-platform validation

Reproduce mission-sequence scenarios and compare Open Space Toolkit against
[GMAT](https://gmat.gsfc.nasa.gov/) and [Orekit](https://www.orekit.org/).

```{toctree}
:maxdepth: 1

Mission sequence — scenario 1 </notebooks/Cross Platform Validation/OSTk Cross Platform Validation (Mission Sequence Scenario 1)>
Mission sequence — scenario 2 </notebooks/Cross Platform Validation/OSTk Cross Platform Validation (Mission Sequence Scenario 2)>
Mission sequence — scenario 3 </notebooks/Cross Platform Validation/OSTk Cross Platform Validation (Mission Sequence Scenario 3)>
Mission sequence — scenario 4 </notebooks/Cross Platform Validation/OSTk Cross Platform Validation (Mission Sequence Scenario 4)>
```

```{seealso}
For the concepts behind these recipes — trajectories, orbits, dynamics,
accesses and sequences — see [Concepts](../explanation/concepts).
```
