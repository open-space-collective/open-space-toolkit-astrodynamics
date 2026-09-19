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

Measured against a well-conditioned reference — `brentq` on the exact range-rate, rather than minimizing
range, which is locally flat and only resolves its own argmin to ~10 ms in double precision — on a
500 km SSO against a ground station, one-day window:

| Coarse step | Current (COBYLA, ~30 evals) | Cubic spline on range², 0 evals | Spline seed + secant on exact ṙ |
| ----------- | --------------------------: | ------------------------------: | ------------------------------: |
| 30 s        |                     0.020 s |                         0.021 s |                       8e-10 s |
| 60 s        |                     0.019 s |                         0.022 s |                       7e-10 s |
| 120 s       |                     0.021 s |                         0.099 s |                       5e-10 s |

Worst case over six passes. Three things fall out:

- **What COBYLA actually delivers is ~20 ms, not the 1 µs the tolerance implies.** `xtol_rel` is
  *relative*, and a simplex method on a locally flat minimum does not do better than that.
- **The free spline estimate matches it** at the 30 s and 60 s steps, for zero additional geometry
  evaluations. Spline range², not range: range² is very nearly parabolic across a pass, and splining
  range directly is 60× worse (worst 1.35 s at a 60 s step).
- **A spline seed plus a secant on the exact range-rate reaches sub-nanosecond in 4–6 evaluations.**
  ṙ = (Δr · Δv)/|Δr| is exact at any instant, because the state already carries velocity, so TCA is a
  root-find on a smooth scalar rather than a minimization of a black box. Six evaluations of the
  satellite state, against ~30 evaluations of *two* full states today — one of which, for a fixed
  target, is constant.

Note the split: the spline wants only positions (so it is free), and only the handful of refinement
steps need velocity.

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

### Do not spline the elevation peak

Fitting the peak from the elevation samples is the one place this approach fails outright — elevation is
sharply peaked near zenith and a cubic spline under-resolves it badly:

| Coarse step | Worst peak-elevation error, spline | Evaluating elevation at the converged TCA |
| ----------- | ---------------------------------: | ----------------------------------------: |
| 30 s        |                             0.574° |                                  0.000000° |
| 60 s        |                             1.536° |                                  0.000000° |
| 120 s       |                            16.210° |                                  0.000000° |

One exact evaluation at the converged TCA beats the spline by orders of magnitude, which is what
`GenerateAccess` already does. Keep it; just give it an accurate TCA.

### A correctness bug this exposes

That last column holds because closest approach and peak elevation coincide for a **circular** orbit.
They do not in general, and `Access::getMaxElevation()` reports elevation at TCA regardless:

| Orbit                            | TCA vs peak-elevation time | Error in reported "max elevation" |
| -------------------------------- | -------------------------: | --------------------------------: |
| SSO 500 km, e = 0                |                    18.1 ms |                         0.000000° |
| Kepler a = 8000 km, e = 0.15     |               **204.8 s**  |                        **2.029°** |

For an eccentric orbit the reported maximum elevation is simply wrong. The fix is the same machinery:
root-find the elevation derivative (also available in closed form from position and velocity), seeded
from the elevation spline, instead of assuming the peak sits at closest approach.

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
