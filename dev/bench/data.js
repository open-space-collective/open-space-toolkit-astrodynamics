window.BENCHMARK_DATA = {
  "lastUpdate": 1790291362231,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "vishwa2710@gmail.com",
            "name": "Vishwa Shah",
            "username": "vishwa2710"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "5ce35a244845ec7dd749704ebae567baea766aa2",
          "message": "perf: only evaluate thruster dynamics for maneuvering states in extractManeuvers (#722)\n\nSegment::Solution::extractManeuvers built the ungated thruster contribution over\nevery state in the segment solution, then sliced out the rows belonging to maneuver\nblocks and discarded the rest. For a state-dependent guidance law such as Q-Law,\neach discarded row costs a full guidance law evaluation, so the work grows with the\nsegment's coasting states even though none of them can contribute to a maneuver.\n\nEvaluate the thruster dynamics per maneuver-block state instead, using the same\nread-subset reduction getDynamicsContribution applied. Output is unchanged: the rows\nthat are kept are computed from the same state, in the same frame, by the same\ndynamics.\n\nHow much this saves depends on how much of the segment is coasting. On the Q-Law\norbit raise in benchmark/.../Sequence.benchmark.cpp, where roughly 90% of the states\nsit inside maneuver intervals, extractManeuvers goes from a median of 93.9 ms to\n82.5 ms over 7 repetitions (means 89.0 ms and 83.2 ms, standard deviation about\n9 ms), so this scenario is close to the worst case for the change. A segment that\nmostly coasts, or a Sequence whose coast segments dominate, saves proportionally\nmore. The propagation itself is untouched, and every scenario in the benchmark\nreports identical states, maneuvers, propellant and final elements.\n\n\nClaude-Session: https://claude.ai/code/session_016zS8dRNQPH1VizthzErmuG\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-09-21T17:32:56Z",
          "tree_id": "1dc612fcf170d738ddbd7db4959ba12411d53cb2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/5ce35a244845ec7dd749704ebae567baea766aa2"
        },
        "date": 1790013993915,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2089298014.800005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2067529588.3 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 735.9891513333233,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 735.9340306666671 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2192.092243666669,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2191.9282403333327 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 8310.12192933332,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 8309.551957666665 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2146091091.8999958,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2145960264.8000023 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4520410808.600013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4520109011.899997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4522027671.4000025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4521558067.599995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4518166948.600003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4517695011.199999 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.6920393569999987,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.6919536229999608 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 3.8730004049999707,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 3.872739725999992 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 9.035102904999917,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 9.034362006999999 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 5.941128961000004,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.94061101600002 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 8.896718680000163,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.89606041700003 s\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "vishwa2710@gmail.com",
            "name": "Vishwa Shah",
            "username": "vishwa2710"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "a682c33554cdfbae43d862da1847ea79c00d062a",
          "message": "test: add Sequence benchmark for a duty-cycle constrained Q-Law orbit raise (#721)\n\nAdds a benchmark for the Sequence / Segment maneuver-planning machinery driven by\na Q-Law with effectivity gating, which is the configuration where the planner is\nmost expensive and where its cost is hardest to predict from the individual pieces.\n\nScenario: a 540 km -> 550 km sun-synchronous orbit raise at 10:00 LTAN, targeting\nsemi-major axis and argument of periapsis (90 deg), with 0.6 absolute and relative\neffectivity thresholds, a 15.1 mN / 1016 s thruster, maneuver constraints of 10 min\nminimum, 40 min maximum and 30 min minimum separation, a 40 min per 95 min duty\ncycle, and a 2 week horizon. Inclination is derived from the J2 sun-synchronous\ncondition and RAAN from the Sun's right ascension at epoch, so the LTAN is real\nrather than nominal.\n\nFive cases are registered:\n\n- Sequence::solve and Segment::solve on the same scenario, so the Sequence-level\n  overhead can be read off the difference.\n- Segment::solve over the full 2 week horizon (terminated on an InstantCondition\n  rather than the semi-major axis target, which is reached after 60.8 hours), which\n  is what a full planning window actually costs.\n- Segment::Solution::extractManeuvers on its own, since maneuver extraction is\n  post-processing rather than propagation.\n- A Q-Law evaluation micro-benchmark, to relate the end-to-end runtime to the cost\n  of a single guidance law call.\n\nEach case also reports state count, maneuver count, step size extremes, propagation\nduration, propellant consumed and final element errors, so a change that alters the\ntrajectory rather than just its runtime is visible in the benchmark output.\n\n\nClaude-Session: https://claude.ai/code/session_016zS8dRNQPH1VizthzErmuG\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-09-21T18:45:26Z",
          "tree_id": "e6d6d6273de12e9abae327aa65603fa854499865",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/a682c33554cdfbae43d862da1847ea79c00d062a"
        },
        "date": 1790018467901,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2285415053.9000044,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2263386245.4 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 801.5419070000007,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 801.4819946666674 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2400.639005333327,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2400.460655999999 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 8960.344946000003,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 8959.655936000001 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2318426473.7000036,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2318237815.0000033 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 5425835457.199991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5425215501.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5425154762.799991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5424580300.300002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5426187607.8,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5425646263.499993 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.7701581710000482,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.7700239600000032 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 4.084212367000077,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.083719545999941 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 9.92873620599994,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 9.92766982300003 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 6.199979691000067,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.199362257000018 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 8.749684817999992,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.747578694999959 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 12.565164750999998,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 12.56256756099998 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 12.474522161999971,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 12.471207548999985 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 253.65456917699998,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 253.62684726999987 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.10785605099999884,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.10784526400004779 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 114.00265211725485,
            "unit": "us/iter",
            "extra": "iterations: 6140\ncpu: 113.99904022802171 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "vishwa2710@gmail.com",
            "name": "Vishwa Shah",
            "username": "vishwa2710"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "cc3d039e9b7eabb62aa489fe37d98ffa466f4423",
          "message": "perf: hoist the invariant gradient out of the Q-Law effectivity sweep (#723)\n\n* perf: hoist the invariant gradient out of the Q-Law effectivity sweep\n\nWhen an effectivity threshold is configured, every Q-Law evaluation sweeps 50 true\nanomalies and builds the full thrust vector at each one, so that the current thrust\ndirection can be scored against the best and worst directions available over the\norbit.\n\ncomputeThrustVector is the product of two factors: ∂Q/∂oe, which depends only on the\nfive non-anomalistic elements, and the Gauss variational matrix ∂oe/∂F, which depends\non the true anomaly. The sweep holds the five elements fixed and varies only the true\nanomaly, so ∂Q/∂oe was being recomputed identically 50 times per call - and with the\nfinite-difference gradient strategy, each of those recomputations is itself 10\nevaluations of Q.\n\nEvaluate it once before the loop and multiply by the per-anomaly Gauss matrix inside.\nThe arithmetic and its order are unchanged, so results are bit-identical; the NaN\nguard moves with the computation.\n\nMeasured on the Q-Law orbit raise in benchmark/.../Sequence.benchmark.cpp\n(best of 3, analytical gradient):\n\n  Segment::solve                         8.53 s  ->  7.27 s\n  QLaw::calculateThrustAccelerationAt    77.9 us ->  64.9 us\n  extractManeuvers                       86.6 ms ->  59.5 ms\n\nextractManeuvers benefits as well because the ungated guidance law it builds carries\neffectivity thresholds of 0.0, which are defined values and therefore still run the\nsweep. States, maneuvers, propellant and final elements are unchanged in every\nscenario, and the gain is larger with GradientStrategy::FiniteDifference.\n\nCo-Authored-By: Claude Opus 5 <noreply@anthropic.com>\nClaude-Session: https://claude.ai/code/session_016zS8dRNQPH1VizthzErmuG\n\n* chore: apply suggestion from @vishwa2710\n\n---------\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-09-21T13:57:59-07:00",
          "tree_id": "a7b40598d3589c0cd1ceb29b7b1b0749d97c7a1e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/cc3d039e9b7eabb62aa489fe37d98ffa466f4423"
        },
        "date": 1790026307179,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2402005302.499998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2371542456.0000005 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 821.8824996666664,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 821.8083623333332 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2516.3732673333357,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2516.1925209999986 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 9443.09316833333,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 9442.4541 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2476526782.699989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2476354969.3999977 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 5266320454.199996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5265963258.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5275544679.4000025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5275142535.599994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5267786596.299976,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5267370662.099996 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.7759547289999773,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.7759244690000173 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 3.737022887999956,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 3.7367640329999574 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 3.9937256760000537,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 3.993392268999969 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 5.624532564999981,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.624175259999959 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 9.032954746000087,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 9.032246745000009 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 11.220581401000118,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.217893024999967 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 11.127730506000034,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.124616754000044 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 224.49199222200002,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 224.465602595 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.08720134700001836,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.08720155100002103 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 93.87452517696357,
            "unit": "us/iter",
            "extra": "iterations: 7487\ncpu: 93.86701535996664 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "vishwa2710@gmail.com",
            "name": "Vishwa Shah",
            "username": "vishwa2710"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "0b3af6d379d73c28f1dd3da4c44c2fb4852e6c0d",
          "message": "perf: evaluate BrouwerLyddaneMeanLong::toCOE on doubles instead of Real (#725)\n\n* perf: evaluate BrouwerLyddaneMeanLong::toCOE on doubles instead of Real\n\ntoCOE is roughly 300 lines of scalar arithmetic written in ostk::core::type::Real.\nReal is not a zero-cost wrapper over double: its operators are defined in\nlibopen-space-toolkit-core.so, so they cannot inline across the shared-library\nboundary, and each one branches on undefined and infinite operands and calls further\nout-of-line predicates such as isZero, isStrictlyPositive and getSign.\n\nThat cost is multiplied twice over. toCOE runs once per iteration of the\nBrouwer-Lyddane fixed-point solve (up to 75 iterations, 1e-8 relative tolerance),\nand that solve runs on every osculating-to-mean conversion - which for a Q-Law in\nthe BrouwerLyddaneMeanLong domain is every single guidance law evaluation. A\nsampling profile of the Q-Law orbit raise in benchmark/.../Sequence.benchmark.cpp\nput 43% of the whole run inside boxed-Real operators reached through this function.\n\nDeclare the locals as double. The arithmetic is identical - Real's operators perform\nplain double arithmetic - so the results are bit-identical. Real::Pi() and\nReal::TwoPi() are literally M_PI and 2.0 * M_PI, and the Real::Undefined() sentinels\nfor ma, raan and aop become quiet NaN; all three are assigned on every path before\nuse. The diff is mechanical: types, those two constants, and the mod lambda's\nsignature.\n\nMeasured against the merge base on the Q-Law orbit raise (15 repetitions for the\nmicro-benchmark, best of 3 for the rest):\n\n  QLaw::calculateThrustAccelerationAt   mean 92.8 us -> 70.6 us   (median 93.4 -> 70.5)\n  Segment::solve                        8.5-9.2 s   -> 6.52 s\n  extractManeuvers                      78-87 ms    -> 62 ms\n\nThe merge-base figures are given as a range because this container drifts by over\n10% across tens of minutes; this change is well outside that band. States, maneuvers,\npropellant and final elements are unchanged in every benchmark scenario, and the 261\nunit tests covering COE, BrouwerLyddaneMean / Long / Short, QLaw, Segment and\nSequence pass.\n\nOne behavioural edge remains: Real division by exactly zero yields Undefined where\ndouble yields an infinity. In toCOE the only such divisor is the 1 - 5cos^2(i) term,\nthe critical-inclination singularity at i ~ 63.4 and 116.6 degrees, which the\nfunction already detects and warns about separately and where both the old and the\nnew result are meaningless.\n\nCo-Authored-By: Claude Opus 5 <noreply@anthropic.com>\nClaude-Session: https://claude.ai/code/session_016zS8dRNQPH1VizthzErmuG\n\n* perf: solve Kepler's equation on doubles instead of Real (#726)\n\nCOE::EccentricAnomalyFromMeanAnomaly runs a Newton-type refinement of up to 1000\niterations, and its two helper lambdas, keplerstart3 and eps3, took and returned\nostk::core::type::Real by value - about 15 boxed operations per iteration. Real's\noperators live in libopen-space-toolkit-core.so, so they cannot inline across the\nshared-library boundary, and each branches on undefined and infinite operands and\ncalls further out-of-line predicates.\n\nThe function is reached once per iteration of the Brouwer-Lyddane fixed-point solve\n(through getSIVector's anomaly conversion and the Kepler-to-Cartesian step), which\nin turn runs on every osculating-to-mean conversion, so a Q-Law in the\nBrouwerLyddaneMeanLong domain pays it on every guidance law evaluation.\n\nDeclare the lambdas and the loop variables as double. The arithmetic is identical -\nReal's operators perform plain double arithmetic - so results are bit-identical. The\nReal::Undefined() sentinel for E becomes quiet NaN and is still assigned before use\non every path, and the public signature (Angle, Real, Real) is untouched.\n\nOn its own this change is not measurable on the Q-Law benchmark: with toCOE still\nevaluated on Real, that function so dominates the fixed-point solve that the Kepler\nsolver is a small slice, and the difference sits inside this container's roughly\n10% drift (15 repetitions gave 84.5 us against 81.8 us for an adjacently built merge\nbase - noise, not a regression).\n\nStacked on claude/perf-blm-tocoe-doubles, where the profile says it should matter,\nit is unambiguous (15 repetitions each, built back to back):\n\n  QLaw::calculateThrustAccelerationAt   mean 70.6 us -> 51.6 us   (-27%)\n                                        standard deviation 0.8 us and 1.1 us\n\nSo this is worth landing alongside or after that change rather than being judged on\nits own. States, maneuvers, propellant and final elements are unchanged in every\nbenchmark scenario, and the 261 unit tests covering COE, BrouwerLyddaneMean /\nLong / Short, QLaw, Segment and Sequence pass.\n\n\nClaude-Session: https://claude.ai/code/session_016zS8dRNQPH1VizthzErmuG\n\nCo-authored-by: Claude <noreply@anthropic.com>\n\n---------\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-09-21T15:52:29-07:00",
          "tree_id": "71afe2b5d215c0522460fb8a8361df1aaaa2fe6b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/0b3af6d379d73c28f1dd3da4c44c2fb4852e6c0d"
        },
        "date": 1790033134621,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2157310760.7,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2132945851.2 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 754.0201686666705,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 753.9414560000009 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2262.6732056666583,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2262.451366999999 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 8529.964382666682,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 8529.398916000004 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2208817115.7999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2208624967 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4659512104.300001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4659106785.399995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4666158617.799988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4665718829.299991 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4670383144.000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4669963007.499996 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.624849728000072,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.624773724000022 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 2.321392218000028,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.3211649890000103 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 2.5468329860000267,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.5464805410000224 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 3.317843870000047,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 3.3175406199999884 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 8.81206776900001,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.81110349299999 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 6.5944519739999805,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.593785009999976 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 6.54913035900006,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.548401184999989 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 133.88175918699994,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 133.85591232599995 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.043257391999986794,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.04325750100002779 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 40.0786205078215,
            "unit": "us/iter",
            "extra": "iterations: 17447\ncpu: 40.076874133088495 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "vishwa2710@gmail.com",
            "name": "Vishwa Shah",
            "username": "vishwa2710"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d7b1ebc93026f86e10c4245e8ae5c767c29ab7b5",
          "message": "perf: add a double overload of QLaw::Compute_dOE_dF (#724)\n\n* perf: add a double overload of QLaw::Compute_dOE_dF\n\nCompute_dOE_dF is the Gauss variational matrix, and with an effectivity threshold\nconfigured it runs 51 times per Q-Law evaluation - once for the current true anomaly\nand once for each point of the 50-point sweep. Each call converted the gravitational\nparameter out of its Derived unit and then went through COE::ComputeSemiLatusRectum,\nCOE::ComputeAngularMomentum and COE::ComputeRadialDistance, whose arguments and\nresults are ostk::core::type::Real.\n\nReal is not a zero-cost wrapper: its operators live in libopen-space-toolkit-core.so,\nso they cannot inline across the shared-library boundary, and each one branches on\nundefined and infinite operands and calls further out-of-line predicates. For three\none-line formulas evaluated 51 times per guidance law call, that dominates.\n\nAdd an overload taking the gravitational parameter as a double and inline the three\nhelper expressions on plain doubles. QLaw already stores mu_ in SI units, so the hot\npath passes it straight through; the existing Derived overload is kept and simply\nforwards, so no caller or binding has to change.\n\nMeasured on the Q-Law micro-benchmark in benchmark/.../Sequence.benchmark.cpp, built\nfrom this branch and from its merge base back to back (15 repetitions each):\n\n  QLaw::calculateThrustAccelerationAt   mean 92.8 us -> 85.5 us\n                                        median 93.4 us -> 88.3 us\n\nThe container these ran on drifts by more than 10% over tens of minutes, which is the\nsame order as this change, so the two builds were measured adjacently and repeated\nrather than compared against an earlier reading. Segment::solve on the same benchmark\ncame out at 8.0 s against 8.5-9.2 s for the merge base. States, maneuvers, propellant\nand final elements are unchanged.\n\nCo-Authored-By: Claude Opus 5 <noreply@anthropic.com>\nClaude-Session: https://claude.ai/code/session_016zS8dRNQPH1VizthzErmuG\n\n* fix: bindings\n\n---------\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-09-21T17:47:39-07:00",
          "tree_id": "0dfd8ea14e9ebf621d077907dbe5faffae02efe2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/d7b1ebc93026f86e10c4245e8ae5c767c29ab7b5"
        },
        "date": 1790039825834,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 1948898546.1000028,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1901701624.8 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 646.8325786666659,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 646.7774129999999 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2018.2074436666728,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2018.0238439999994 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 7557.105372666664,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 7556.533379000001 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1996901699.099999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1996724479.9999974 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4697410016.300006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4697005863.899999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4697711922.600013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4697301346.299997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4694315227.999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4693778799.500001 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.5560579099999927,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.5560357170000145 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 1.888537600999996,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.8883392699999604 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 2.0642607999999996,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.064103996999961 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 2.653596961000062,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.6533398239999997 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 6.665663179000035,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.665017596000013 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 5.184980714999938,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.184357120000016 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 5.1521234150000055,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.151593910999992 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 104.70026148000011,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 104.680809565 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.024951130999966153,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.024949584000012237 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 23.397890763883847,
            "unit": "us/iter",
            "extra": "iterations: 29926\ncpu: 23.397284535186806 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "vishwa2710@gmail.com",
            "name": "Vishwa Shah",
            "username": "vishwa2710"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "a05b8e66dbd37bb581c43ef1d4634d1c4c7ab9a6",
          "message": "perf: solve closest approach as a root (#717)\n\n* perf: solve closest approach as a root, not a minimization\n\nFindTimeOfClosestApproach minimized range with NLopt's COBYLA, a\ngeneral-purpose derivative-free simplex method, at a relative x tolerance\nof 1 us. A range minimum is locally flat, so that is a badly conditioned\nthing to search for: profiling put the call at 93% of a 20-target\nelevation run, at about 30 objective evaluations per access, each\npropagating and frame-transforming two full states.\n\nRange is smooth across a pass and has a single minimum, so the derivative\nof its square changes sign exactly once. Solve for that sign change with\nthe existing bracketed RootSolver instead. The residual used is\ndx . dv - half the derivative of the squared range - which shares its sign\nand its root with the range rate but needs no division, so it stays finite\nwhen the two trajectories coincide and the range is zero.\n\nTwo cases that a bracketed solve has to handle explicitly, and a\nminimization did not:\n\n- An access clipped by the analysis interval can begin with the satellite\n  already receding, or end with it still approaching. There is then no sign\n  change, and the closest approach sits on the corresponding endpoint.\n- An access captured by a single coarse sample has a zero-length interval\n  and nowhere to search, so the single sample is the answer. This\n  incidentally fixes coarse mode raising \"Cannot find TCA (solution did not\n  converge)\" on such an access.\n\nAlso thread the fixed target's celestial-frame coordinates down from\ncomputeAccessesForFixedTargets. A fixed target does not move in that frame,\nso its position is constant and its velocity zero there: supplying them\nremoves the second state propagation and transform from every iteration of\nthe search, and from the max-elevation evaluation.\n\nMeasured on the benchmark suite, against the same build before the change:\n\n  100 targets | 1 week  | Elevation    9323 ms -> 500 ms   18.7x\n  100 targets | 1 day   | Elevation    1334 ms ->  78 ms   17.1x\n   10 targets | 1 day   | Elevation     137 ms ->  10 ms   13.4x\n   10 targets | 1 day   | AER           143 ms ->  11 ms   13.2x\n    1 target  | 1 day   | Elevation    19.7 ms -> 3.9 ms    5.0x\n   10 targets | 1 day   | LineOfSight   799 ms -> 656 ms    1.2x\n    1 target  | 2 weeks | GCRF out     2494 ms -> 2355 ms   1.1x\n\nThe line-of-sight and GCRF cases move least because they are dominated by\ncrossing refinement and by the coarse scan respectively, not by TCA. The\nspeedup exceeds what the evaluation count alone predicts because the search\nnow visits far fewer distinct instants, which also relieves the frame\ntransform cache.\n\nNLopt is no longer used anywhere in the library.\n\nAdd a test asserting the property that defines the result: the range at the\nreported instant is smaller than the range one second either side of it.\n\nCo-Authored-By: Claude Opus 5 <noreply@anthropic.com>\nClaude-Session: https://claude.ai/code/session_01T8xLC4jVuShuAQPp81rJ4Y\n\n* chore: Update test/OpenSpaceToolkit/Astrodynamics/Access/Generator.test.cpp\n\n* feat: address feedback\n\n* chore: style\n\n---------\n\nCo-authored-by: Vishwa <vishwa@loftorbital.com>\nCo-authored-by: Claude Opus 5 <noreply@anthropic.com>",
          "timestamp": "2026-09-23T00:46:25Z",
          "tree_id": "09b87b2591fd339b88e7b5fb4fe86c6bb0b0c9af",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/a05b8e66dbd37bb581c43ef1d4634d1c4c7ab9a6"
        },
        "date": 1790126252957,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2351274935.799995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2166458320.600001 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 637.0107093333294,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 636.9432346666671 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2290.4697986666633,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2290.210360000001 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 508.8235609999856,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 508.7479076666668 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2325526333.100026,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2325258087.2999983 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 5428590883.800018,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5427613203.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5429166268.200015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5428556331.599999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5424474966.200011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5423550232.600004 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.6822025000000167,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.6821598629999812 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 2.263981274999992,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.263735378000007 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 2.514846683999963,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.5145476489999794 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 3.183025574999988,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 3.1826462719999995 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 8.293785786000058,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.292944076999987 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 5.943160905000013,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.942497944999985 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 5.933989910000037,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.933279674999966 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 109.88004334699986,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 109.86909793399997 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.0320825789999617,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.03208174600001712 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 29.113223313904708,
            "unit": "us/iter",
            "extra": "iterations: 24020\ncpu: 29.11018263946704 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "vishwa2710@gmail.com",
            "name": "Vishwa Shah",
            "username": "vishwa2710"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "62d4a6d148e048e4323c59578fd1431ea03d0392",
          "message": "perf: refine crossings on a continuous residual, not a boolean (#718)\n\n* perf: refine crossings on a continuous residual, not a boolean\n\ncomputePreciseCrossings fed RootSolver a function returning +/-1. TOMS748's\ninterpolation steps are worthless on a step function, so it degenerates to\nbisection: measured at about 29 criterion evaluations per crossing to reach\nthe 1 us tolerance.\n\nReplace the boolean condition with a signed residual - positive while the\ncriterion is satisfied, negative while it is not - so that a crossing is\nstill a sign change, but the solver sees the geometry rather than a step.\n\nOnly the elevation-interval criterion gets a genuine margin here:\nmin(elevation - lower, upper - elevation), which is the distance in radians\nto whichever bound is nearer and handles a crossing of either. The AER\ninterval, AER mask and line-of-sight criteria keep the boolean, expressed\nas +/-1, so their behaviour is unchanged; line-of-sight in particular is an\nintersection test with no natural margin, and would need a separate\nsurrogate such as the segment's perpendicular distance to the ellipsoid.\n\nMeasured against main, this is worth almost nothing on its own - 1.00x to\n1.03x across the suite, and 1.52x on a single-target elevation run - because\ncrossing refinement is only about 5% of an elevation run while closest\napproach is 93% of it. It is worth having once that 93% is addressed.\n\nCo-Authored-By: Claude Opus 5 <noreply@anthropic.com>\nClaude-Session: https://claude.ai/code/session_01T8xLC4jVuShuAQPp81rJ4Y\n\n* refactor: naming\n\n* chore: format\n\n* chore: improve comments\n\n---------\n\nCo-authored-by: Vishwa <vishwa@loftorbital.com>\nCo-authored-by: Claude Opus 5 <noreply@anthropic.com>",
          "timestamp": "2026-09-24T22:41:43Z",
          "tree_id": "d6eca04a0e79e57d9b528697e6f0b2e598ba747f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/62d4a6d148e048e4323c59578fd1431ea03d0392"
        },
        "date": 1790291360407,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2457850946.999997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2336776521.6000004 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 644.7061513333286,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 644.5665526666661 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2461.50609833334,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2461.0796943333344 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 208.73338733332503,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 208.70763333333525 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2573339286.900006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2572780593.4000015 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6045049115.600005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6044254718.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6044868737.900009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6044214290.700001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6044753992.599977,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6044068837.600002 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.7137342170000238,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.7136540790000083 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 2.4265378679999685,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.4262535449999802 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 2.658751198999994,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.6584329229999923 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 3.3921553520000316,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 3.391746473000069 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 8.46278858300002,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.46174436299998 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 6.316832757000157,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.315910852999991 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 6.299755148999907,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.298849345000008 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 116.29428914300001,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 116.268994835 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.03237249900007555,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.03237003200001709 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 30.28169261477084,
            "unit": "us/iter",
            "extra": "iterations: 23046\ncpu: 30.28014978738213 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}