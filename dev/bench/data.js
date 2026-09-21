window.BENCHMARK_DATA = {
  "lastUpdate": 1790013995697,
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
      }
    ]
  }
}