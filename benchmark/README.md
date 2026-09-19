# Access generation — performance analysis

Profile of `ostk::astrodynamics::access::Generator` as it stands today: where the time goes, and what
would move the needle. Numbers below are wall-clock on a 4-core x86-64 container, and the attribution
percentages come from Callgrind instruction counts.

Unless stated otherwise the workload is: a 500 km / 10:30 LTAN sun-synchronous satellite as a tabulated
trajectory with an **ITRF** output frame (so the satellite state needs no per-step rotation), ground
stations in ITRF, a one-day window, the default 1 minute step and the default 1 µs tolerance.

## How the algorithm spends its time

`computeAccessesForFixedTargets` has three phases:

1. **Coarse scan** — walk the interval at `step_`, compute the satellite state once per step, and
   evaluate the visibility criterion for every target (vectorized across targets for the AER and
   elevation criteria, a scalar loop for line-of-sight and AER masks).
2. **Crossing refinement** (`computePreciseCrossings`, skipped when `coarse = true`) — for each coarse
   interval boundary, run `RootSolver` (TOMS748) on the criterion, which is a boolean, to find the
   crossing to within `tolerance_`.
3. **Access construction** (`generateAccessesFromIntervals`) — for each access, find the time of closest
   approach with NLopt COBYLA, then evaluate the elevation there.

Inclusive share of total instructions, by scenario:

| Scenario (1 day)                          | Coarse scan | Refinement | TCA + access build |
| ----------------------------------------- | ----------: | ---------: | -----------------: |
| 1 target, line-of-sight                   |         11% |        62% |                27% |
| 20 targets, line-of-sight                 |          <1% |        74% |                25% |
| 20 targets, elevation                     |          1% |         5% |            **94%** |
| 1 target, elevation, **GCRF** output frame |     **89%** |         7% |                 4% |

Two things fall out of that table. The coarse scan — the part that is actually vectorized across
targets — is a rounding error in every case except when the trajectory model forces a per-step frame
rotation. And for the workload people actually run (many ground stations, elevation mask), **89% of the
time is spent finding times of closest approach**.

## Bottleneck 1 — frame transforms, and a cache that is too small

`Frame::getTransformTo` accounts for **73–99%** of instructions across all scenarios. Inside it,
`CIRF::getTransformAt` → `iauNut06a` → `iauNut00a` — the full 1365-term IAU 2000A nutation series — is
**29–59%** of total runtime on its own.

The frame manager caches transforms, but the cache holds 1000 entries by default
(`OSTK_PHYSICS_FRAME_MANAGER_MAX_TRANSFORM_CACHE_SIZE`). A one-day analysis at a 1 minute step already
asks for 1441 distinct instants, and refinement and TCA query thousands more at arbitrary instants, so
the cache starts thrashing partway through and never recovers. The effect is a cliff, not a gradient:

| Analysis window | Default (1000) | Cache 100 000 | Speedup |
| --------------- | -------------: | ------------: | ------: |
| 1 day           |        11.8 ms |       12.1 ms |    1.0× |
| 4 days          |        48.0 ms |       46.8 ms |    1.0× |
| 5 days          |       222.4 ms |       57.8 ms |    3.8× |
| 7 days          |       406.2 ms |       82.0 ms |    5.0× |
| 14 days         |       834.3 ms |      163.9 ms |    5.1× |

(1 target, line-of-sight. Scaling is exactly linear at 100 000; all of the super-linearity is cache
thrashing. Table size is not a factor — a fixed 14-day tabulated model analysed over sub-windows gives
the same curve.)

Raising the cache is a configuration change, and on multi-target workloads it is worth more than
anything else measured here:

| 100 targets, 1 day | Default (1000) | Cache 100 000 | Speedup |
| ------------------ | -------------: | ------------: | ------: |
| Line-of-sight      |       6804 ms |       1081 ms | **6.3×** |
| Elevation interval |       1352 ms |        183 ms | **7.4×** |
| AER interval       |       1397 ms |        197 ms | **7.1×** |

**Opportunity.** Raise the default cache size for access generation, or size it from the analysis
interval. Beyond that, the generator asks for a full transform at instants it chooses itself; a
precomputed, interpolated Earth-rotation table over the analysis window would cut the nutation series
out of the inner loop entirely.

## Bottleneck 2 — time of closest approach dominates multi-target runs

`FindTimeOfClosestApproach` is **93% of a 20-target elevation run**. It is a 1-D minimization of range²
over the access interval, solved with NLopt's COBYLA — a general-purpose derivative-free method — at
`xtol_rel = 1 µs`. Measured cost per access: ~30 objective evaluations, and each evaluation does

```cpp
const auto [queryFromState, queryToState] = contextPtr->getStatesAt(queryInstant);
// ... both states transformed into the celestial frame
```

two **full `State::inFrame`** transforms — position *and* velocity — where only positions are used
(`State::inFrame` alone is 88.5% of the 20-target elevation run). For a fixed ground target the "from"
state is *constant in ITRF*, yet it is re-propagated and re-transformed on every iteration.

This is also why the batched multi-target API does not pay off. The coarse scan shares the satellite
state across targets, but TCA runs per access, so total cost is linear in accesses:

| Targets | Elevation, 1 day | per target |
| ------: | ---------------: | ---------: |
|       1 |           6.0 ms |     6.0 ms |
|      20 |          273.2 ms |    13.7 ms |
|     100 |         1351.7 ms |    13.5 ms |

**Opportunities**, roughly in order of payoff:

- Hoist the fixed-target position out of the objective. For `AccessTarget::Type::Fixed` it is a
  constant vector in ITRF; computing it once removes about half the transforms in the objective.
- Transform only the position, not the whole state. `computeAccessesForFixedTargets` already does this
  in the coarse loop ("transform only the position, not the whole state"); TCA does not.
- Replace COBYLA with a bracketed 1-D method. Range² over a pass is smooth and unimodal; Brent's method
  converges in far fewer evaluations than a general-purpose simplex method, and `RootSolver` already
  wraps Boost for exactly this kind of problem.
- Seed the search from the coarse grid. The coarse scan already evaluates the geometry at every step;
  the minimum-range sample is a good bracket and is currently thrown away.
- Make TCA optional. Callers that only want acquisition/loss of signal pay for it unconditionally.

## Bottleneck 3 — line-of-sight is evaluated once per (instant, target)

`VisibilityCriterion::LineOfSight::isSatisfied` calls `Environment::setInstant(anInstant)` and then
`Environment::intersects(...)`, which re-poses **every** object in the environment (Earth, Sun, Moon)
into the common frame before testing a two-point segment against it. Callgrind call counts for a 20-target,
one-day run: `Environment::setInstant` is called **37 109** times, i.e. once per (instant, target) pair,
for 1441 distinct instants. With 100 targets that is 100 redundant re-poses of the whole environment per
step.

The cost shows up directly — 20 targets, 1 day: line-of-sight 1360 ms vs elevation 273 ms vs AER
285 ms, a 5× gap against criteria that are closed-form vector maths on the same grid.

**Opportunities.** Hoist `setInstant` out of the per-target loop in `computeAccessesForFixedTargets`
(it depends only on the instant). Transform the segment into each body's frame rather than transforming
the bodies' geometry. Intersect against the central body only, or pre-filter with a cheap analytic
Earth-grazing test and fall back to the full geometry test only near the horizon.

## Bottleneck 4 — `coarse = true` is unusable at the default step

`coarse = true` is the documented way to trade accuracy for speed, but it raises on ordinary inputs:

```
RuntimeError: Cannot find TCA (solution did not converge):
  2023-01-01 09:12:00 [UTC] - 2023-01-01 09:12:00 [UTC].
```

When an access is captured by a single coarse sample, `ComputeIntervals` emits a zero-length interval.
Precise mode widens it; coarse mode does not, so `FindTimeOfClosestApproach` is handed
`upperBound = 0.0`, COBYLA returns a non-success code, TCA comes back undefined, and `GenerateAccess`
throws because the interval does not touch the analysis boundaries (so it is typed `Complete`).

The trigger is any pass captured by a single coarse sample. An above-the-horizon mask on the SSO
satellite above survives the default step, but a **10° mask — an entirely ordinary station
constraint — does not**, because it shortens the marginal passes below 60 s:

| Step  | 1 target, 0° mask | 20 targets, 0° mask | 20 targets, 10° mask |
| ----- | ----------------- | ------------------- | -------------------- |
| 300 s | throws            | throws              | throws               |
| 60 s  | ok                | ok                  | **throws**           |
| 30 s  | ok                | ok                  | ok                   |
| 10 s  | ok                | ok                  | ok                   |

**Opportunity.** Handle the degenerate interval: skip TCA (or return the sample instant) when the
interval has zero duration, rather than letting the optimizer fail. The benchmark scenarios here use a
10 s step to stay in the regime where coarse mode works at all.

## Bottleneck 5 — trajectory targets take a much slower path

Trajectory targets bypass the vectorized path entirely and go through `TemporalConditionSolver` with
the scalar `getConditionFunction`, which for any non-line-of-sight criterion calls
`Generator::CalculateAer` — and that **builds an NED frame object per sample**
(`aCelestialSPtr->getFrameAt(referencePoint_LLA, Celestial::FrameType::NED)`) before transforming both
positions into it.

Same geometry, same window, 1 day: line-of-sight 12.2 ms fixed vs 152.6 ms as a trajectory target
(**12.5×**); elevation 4.2 ms vs 157.2 ms (**37.6×**). The trajectory path costs about the same either
way, because the NED frame construction dominates whatever the criterion.

**Opportunity.** A stationary trajectory target is geometrically a fixed target; more generally, the
SEZ-rotation approach already used for fixed targets works for a moving target too, recomputed per
step, and avoids constructing a frame object each time.

## Recovering TCA and the crossings by interpolation

The coarse scan already evaluates the geometry at every grid point and throws the numbers away — only a
boolean survives into `inAccessPerTarget`. `computeElevations` computes `dx.colwise().norm()` as its own
denominator, so **range is already in hand at every coarse sample** for the elevation criterion, and the
AER path computes azimuth, elevation and range outright. Fitting a cubic spline through those retained
samples and solving analytically costs no new geometry evaluations at all.

Three scalars are available in closed form from a state, with `u` the station's unit vertical and
`s = sin(elevation) = (u · dx)/|dx|`:

```
rdot = (dx · v) / |dx|                       zero at closest approach
sdot = [ (u · v) - s * rdot ] / |dx|         zero at peak elevation
edot = sdot / sqrt(1 - s^2)
```

Root-find `sdot`, not `edot`: they share the root, but `edot` blows up near zenith where
`sqrt(1 - s^2)` goes to zero.

### Test bed

All figures below are on **SGP4**, plus unperturbed Kepler orbits for the circular and eccentric cases.
`Orbit::SunSynchronous` is deliberately excluded — see *A model bug this exposes* — because its reported
velocity does not match the derivative of its own position, which silently corrupts every `rdot`-based
measurement. The references are `brentq` on the exact `rdot` and `sdot`, cross-checked against direct
parabola fits to range and elevation; on the self-consistent models the two agree to 14–320 µs, which is
the parabola fit's own truncation.

### Seeding: range² beats range-rate, decisively

Both splines cost zero new evaluations. Worst error over the passes in a one-day window:

| Coarse step | Cubic spline on range² | Cubic spline on rdot |
| ----------- | ---------------------: | -------------------: |
| 30 s        |              0.0002 s  |             0.0114 s |
| 60 s        |              0.0023 s  |             0.6768 s |
| 120 s       |              0.0340 s  |             4.0929 s |

Range² is very nearly parabolic across a pass, so a cubic captures it almost exactly. `rdot` is its
derivative — an S-curve with far more curvature — and a cubic through the same sample points is
**50–300× worse**. Splining `rdot` also needs velocity at every coarse sample, which the scan does not
currently transform; splining range² needs only what is already there.

At a 60 s step the free range² estimate (2.3 ms) is already better than what COBYLA returns after ~30
evaluations (2.9 ms).

### Refining: secant on the exact rdot

Worst error after a given number of true-geometry evaluations, SGP4:

| Step  | Seed | 3 evals | 4 evals | 6 evals |
| ----- | ---- | ------: | ------: | ------: |
| 60 s  | range² |  3.1e-5 s | 3.7e-8 s | 2.5e-8 s |
| 60 s  | rdot   |  8.9e-5 s | 3.9e-8 s | 2.6e-8 s |
| 120 s | range² |  1.2e-4 s | 4.7e-8 s | 2.0e-8 s |
| 120 s | rdot   |  6.4e-3 s | 1.9e-5 s | 2.5e-8 s |

Four evaluations reach ~4e-8 s — about five orders of magnitude better than COBYLA's 2–3 ms, at an
eighth of the cost. The seed matters most at coarse steps: at 120 s the range² seed is still converged
at 4 evaluations while the rdot seed needs 6. For reference, Brent on `rdot` over the raw coarse
bracket, with no spline seed, takes **20–76 evaluations** to reach the same tolerance.

The 2-evaluation column is omitted because it is just the initial bracket point, not an estimate.

### Peak elevation: root-find sdot, do not spline the elevation

| Method | SGP4, 30 s | SGP4, 60 s | SGP4, 120 s | Kepler e = 0.15, 60 s |
| ------ | ---------: | ---------: | ----------: | --------------------: |
| Cubic spline argmax of elevation | 3.436° | 5.234° | 23.877° | 0.241° |
| Elevation evaluated at TCA | 0.00021° | 0.00021° | 0.00021° | **2.029°** |
| Secant on sdot, 3 evals | 8.1e-9° | 1.2e-3° | 0.964° | 1.1e-8° |
| Secant on sdot, 4 evals | 4.4e-9° | 2.7e-7° | 3.5e-3° | 2.3e-9° |
| Secant on sdot, 6 evals | 1.1e-10° | 2.6e-9° | 2.6e-9° | 5.0e-12° |

Splining the elevation samples is the one variant that fails outright — elevation is sharply peaked near
zenith and a cubic under-resolves it by degrees. Evaluating elevation at TCA is accurate for a
near-circular orbit and wrong for an eccentric one. **Four secant steps on `sdot` are accurate for
both**, and cost about the same as one COBYLA iteration.

### Cost

Velocity is nearly free once the position is being transformed: the Callgrind profile puts
`CartesianVelocity::inFrame` at 1.63% against `CartesianPosition::inFrame` at 23.12%, because both ride
the same frame transform. So the choice between position-only and full-state evaluation is roughly a 7%
difference per evaluation, not 2× — the argument against splining `rdot` is accuracy, not cost.

Per access, end to end:

| | Evaluations | What each costs |
| --- | ---: | --- |
| Today | ~30 | two full `State::inFrame`, one of them a constant fixed target |
| Range² spline + 4 secant on rdot + 4 secant on sdot | 8 | one satellite state each |

That is roughly a 7× reduction in state transforms on the phase that is 93% of a multi-target elevation
run, and it returns a TCA five orders of magnitude tighter plus a peak elevation that is correct for
eccentric orbits.

### Crossings: a good seed, not a drop-in answer

The same idea applied to acquisition and loss of signal is weaker, because the crossing falls wherever
it falls between two grid points rather than near the middle of a fitted span. Cubic spline through the
four real grid samples straddling each crossing, error against `brentq` on the true residual:

| Coarse step | Mask | Spline root, worst | median | Linear between the two straddling samples |
| ----------- | ---- | -----------------: | -----: | ----------------------------------------: |
| 60 s        | 0°   |            76.5 ms | 5.0 ms |                                    2050 ms |
| 60 s        | 10°  |           310.0 ms | 218 ms |                                    3899 ms |
| 120 s       | 0°   |           1444 ms  | 323 ms |                                    7284 ms |

So a spline root is not accurate enough to *replace* refinement at the current tolerance. It is an
excellent bracket: it localizes the crossing to ~0.1 s instead of the 60 s coarse bracket, a 600×
tighter start. Feed that to a secant on the continuous residual (elevation − mask) and two or three
evaluations finish it, against the ~29 bisection steps the boolean TOMS748 costs today.

### A correctness bug this exposes

Closest approach and peak elevation coincide for a **circular** orbit. They do not in general, and
`Access::getMaxElevation()` reports elevation at TCA regardless:

| Orbit                                    | TCA vs peak-elevation time | Error in reported "max elevation" |
| ---------------------------------------- | -------------------------: | --------------------------------: |
| SGP4 LEO, e ≈ 0.0013                     |                    0.754 s |                         0.00021° |
| Kepler a = 8000 km, e = 0.15             |               **204.8 s**  |                        **2.029°** |

### A model bug this exposes

`Kepler::CalculateJ2StateAt` advances the mean anomaly at the J2-corrected mean motion `n_bar` and
secularly drifts RAAN and AOP, then builds the Cartesian state with `coe.getCartesianState(mu)` — the
*unperturbed* two-body position/velocity relation. The position sequence therefore evolves at a
different rate than the reported velocity describes. Measured as |v| against the central difference of
the model's own position:

| Model                                      | \|v\| / \|dP/dt\| |
| ------------------------------------------ | ----------------: |
| SGP4                                        |         1.0000022 |
| Kepler, `PerturbationType::No`              |         1.0000000 |
| Kepler, `PerturbationType::J2`              |     **1.0013275** |
| `Orbit::SunSynchronous` (J2 under the hood) |     **1.0013282** |

`CalculateJ4StateAt` is built the same way. A 1.3e-3 velocity error is inconsequential for a boolean
visibility test, which is why it has gone unnoticed, but it shifts the root of `rdot` by 2–20 ms and
would put a floor under any derivative-based refinement built on these models.

### What it would take

- Retain range² (and elevation) per target per step in the coarse scan instead of only the boolean —
  one `double` per target per step, and for the elevation and AER criteria the values are already
  computed.
- Line-of-sight does not compute range at all today; it would need adding, which is cheap given both
  ITRF positions are in hand.
- Guard the short-pass case. A pass needs four interior samples to fit a cubic; at a 60 s step with a
  10° mask, one pass in four had only two. That is the same population that breaks `coarse = true`
  today, and both want the same fallback.

## Secondary observations

- **`ostk::core::type::Real` overhead.** In the 1-target line-of-sight profile, `Real::operator*`,
  `Real::operator double`, `Real::getSign`, `Real::isDefined` and `Real::operator+` together account for
  ~13% of instructions. The checked-scalar wrapper is not free in inner loops.
- **Per-step heap traffic.** The `computeAer` and `computeElevations` lambdas allocate fresh `MatrixXd`
  temporaries (`dx`, `dx_SEZ`, and `fromPositionDirection_ITRF`, which is recomputed every step from
  data that never changes) on every coarse step. These could be hoisted into preallocated buffers owned
  by the scan.
- **Refinement root-finding on a boolean.** `computePreciseCrossings` feeds TOMS748 a function returning
  ±1. TOMS748's interpolation steps are worthless on a step function, so it degenerates to bisection —
  measured ~29 criterion evaluations per crossing to reach 1 µs. Root-finding on a *continuous* residual
  instead (elevation minus mask, say) would converge in a handful of evaluations.

## Reproducing

The C++ scenarios live in `Access.benchmark.cpp` and run with `make benchmark`. The measurements above
were taken through the Python bindings against the same generator, with Callgrind
(`--toggle-collect='*Generator::computeAccessesForFixedTargets*'`) for the attribution.
