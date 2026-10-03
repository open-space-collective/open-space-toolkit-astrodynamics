# How to determine an orbit from observations

**Orbit determination** (OD) fits a spacecraft state to a set of observations —
typically GNSS position/velocity fixes or ground-tracking measurements — by
least-squares estimation. The `OrbitDeterminationSolver` propagates an initial
guess, compares it to the observations, and iterates until the residuals settle.

The {doc}`Orbit Determination </notebooks/Flight Dynamics/Orbit Determination>` notebook
runs this on real data; this guide covers the API.

## Load the observations

Observations are a list of `State` objects. In practice you read them from a file
of tracking data. The `dataframe` helper turns a pandas `DataFrame` into states —
tell it which frame the coordinates are in:

```python
import pandas as pd
from ostk.physics.coordinate import Frame
from ostk.astrodynamics.dataframe import generate_states_from_dataframe

observations = generate_states_from_dataframe(
    pd.read_csv("gnss_fixes.csv"),
    reference_frame=Frame.ITRF(),   # GNSS fixes are usually Earth-fixed
)
```

Any list of `State` objects with position and velocity works, however you build
it.

## Run the solver

Construct the solver from an environment (which fixes the force model used to
propagate between observations), a numerical solver, and a least-squares solver.
Then call `estimate` with an initial guess and the observations:

```python
from ostk.physics import Environment
from ostk.astrodynamics.solver import LeastSquaresSolver
from ostk.astrodynamics.trajectory.state import NumericalSolver
from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    CartesianPosition,
    CartesianVelocity,
)
from ostk.astrodynamics.estimator import OrbitDeterminationSolver

position = CartesianPosition.default()
velocity = CartesianVelocity.default()

solver = OrbitDeterminationSolver(
    environment=Environment.default(),
    numerical_solver=NumericalSolver.default(),
    solver=LeastSquaresSolver.default(),
)

analysis = solver.estimate(
    initial_guess=observations[0],                  # a State to start from
    observations=observations,
    estimation_coordinate_subsets=[position, velocity],
    initial_guess_sigmas={position: [1e2] * 3, velocity: [1e0] * 3},
    observation_sigmas={position: [1e1] * 3, velocity: [1e-1] * 3},
)
```

The `*_sigmas` dictionaries express the uncertainty (standard deviation per
component) of the initial guess and of the observations; they weight the fit and
are optional.

## Read the result

```python
estimated_state = analysis.estimated_state          # best-fit State
covariance = analysis.solver_analysis.estimated_covariance
print(analysis.solver_analysis.termination_criteria)  # e.g. "RMS Update Threshold"
```

`analysis.solver_analysis` also carries the per-iteration steps (each with an RMS
error) and the computed observations, useful for residual analysis.

To go straight to a propagatable orbit instead of a single state, call
`solver.estimate_orbit(...)` with the same arguments — it returns an `Orbit`.

```{seealso}
- Fit a Two-Line Element set instead of a Cartesian state: [Fit a TLE to an ephemeris](solve-tle).
- Applied example on real tracking data: {doc}`Orbit Determination </notebooks/Flight Dynamics/Orbit Determination>`.
```
