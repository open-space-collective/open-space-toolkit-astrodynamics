window.BENCHMARK_DATA = {
  "lastUpdate": 1790018469731,
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
      }
    ]
  }
}