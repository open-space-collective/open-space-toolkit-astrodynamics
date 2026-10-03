# How to fit a TLE to an ephemeris

A [Two-Line Element set](https://en.wikipedia.org/wiki/Two-line_element_set)
(TLE) is the mean-element format the satellite catalogue distributes, propagated
with SGP4. When you have a precise ephemeris — from GNSS, or from your own orbit
determination — and need a TLE that reproduces it, the `TLESolver` fits one by
least squares.

The {doc}`TLE Determination </notebooks/Flight Dynamics/TLE Determination>` notebook
runs this on real data; this guide covers the API.

## Provide the observations

Observations are a list of `State` objects, usually read from tracking data:

```python
import pandas as pd
from ostk.physics.coordinate import Frame
from ostk.astrodynamics.dataframe import generate_states_from_dataframe

observations = generate_states_from_dataframe(
    pd.read_csv("gnss_fixes.csv"),
    reference_frame=Frame.ITRF(),
)
```

The solver works natively in the SGP4/TEME element space and handles the frame
conversions internally, so the observations can be in any frame.

## Run the solver

Construct a `TLESolver`, giving it the catalogue metadata for the TLE it will
produce, and call `estimate` with an initial guess and the observations. The
initial guess anchors the epoch and orbit regime, so it should be a TLE close to
the true orbit:

```python
from ostk.astrodynamics.solver import LeastSquaresSolver
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import TLE
from ostk.astrodynamics.estimator import TLESolver

solver = TLESolver(
    solver=LeastSquaresSolver.default(),
    satellite_number=25544,
    international_designator="98067A",
    revolution_number=12345,
    estimate_b_star=False,   # set True to also fit the B* drag term
)

initial_guess = TLE(
    "1 25544U 98067A   22253.00000622  .00000000  00000-0  71655-1 0    02",
    "2 25544  97.5641  21.8296 0012030 155.5301 309.4836 15.14446734123455",
)

analysis = solver.estimate(initial_guess=initial_guess, observations=observations)
```

If you set `estimate_b_star=True`, the initial guess must be a `TLE` or a
`(State, b_star)` tuple so the drag term has a starting value.

## Read the fitted TLE

```python
estimated_tle = analysis.estimated_tle
print(estimated_tle)
print(analysis.solver_analysis.termination_criteria)  # e.g. "RMS Update Threshold"
```

To obtain a ready-to-propagate SGP4 orbit directly, call
`solver.estimate_orbit(initial_guess=..., observations=...)`, which returns an
`Orbit`.

```{tip}
No good initial TLE? Pass a `(State, b_star)` tuple built from your best state
estimate (for example the result of [orbit determination](orbit-determination))
as the `initial_guess`, and let the solver converge from there.
```

```{seealso}
- Determine a Cartesian state instead: [Determine an orbit from observations](orbit-determination).
- Applied example: {doc}`TLE Determination </notebooks/Flight Dynamics/TLE Determination>`.
```
