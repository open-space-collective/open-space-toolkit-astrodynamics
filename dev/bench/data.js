window.BENCHMARK_DATA = {
  "lastUpdate": 1779306668635,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "65550121+phc1990@users.noreply.github.com",
            "name": "Pau Hebrero",
            "username": "phc1990"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "34c83fc25e68bfa56a6743a9bdc22c86fd91ff69",
          "message": "fix: amend manuver interval and maneuver extraction inconsistency (#679)\n\n* fix: amend manuver interval and maneuver extraction inconsistency\n\n* refactor: rename and consider solve maneuver for interval function\n\n* test: expand test to individual laws\n\n* refactor: apply PR suggestions\n\n* refactor: apply bot suggestions\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-05-20T19:21:06Z",
          "tree_id": "1b09c20b1209503e49ca2193980ce48c79b5acc7",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/34c83fc25e68bfa56a6743a9bdc22c86fd91ff69"
        },
        "date": 1779306666685,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4789864493.0999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4620900365.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4087824275.999986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4087440131.700003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7215365610.099997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7214839660.100001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7231025130.799997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7230127169 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7248987917.69998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7247870640.899999 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.2045564860000013,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.204435521999983 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.365794938000022,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.3654444390000435 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.08996723100006,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.089025592000041 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.9599000829999795,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.959240421999937 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.325466595999956,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.324311237000018 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}