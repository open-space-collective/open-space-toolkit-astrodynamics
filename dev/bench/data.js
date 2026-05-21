window.BENCHMARK_DATA = {
  "lastUpdate": 1779322653580,
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
          "id": "6d2ff7e60e47924ef6a2a0cfb8cf0cd8bc8967d4",
          "message": "fix: consider the original numerical solver maximum step size (as well as adding the python bindings for it) (#680)\n\n* chore: add python bindings to numerical solver set and get maximum step size\n\n* refactor: consider the original solver maximum step size too\n\n* chore: format\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>\nCo-authored-by: vishwa shah <vishwa2710@gmail.com>",
          "timestamp": "2026-05-20T16:48:06-07:00",
          "tree_id": "a8cbe31f4a9166b3f610b42f268af3dba2d44da9",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6d2ff7e60e47924ef6a2a0cfb8cf0cd8bc8967d4"
        },
        "date": 1779322651778,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4982684123.80001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4883892667.400001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4495011214.5000105,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4494473148.900001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7993031276.900002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7992248807.499997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7993268125.7999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7992495800.400002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7990922498.500026,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7990126534.399994 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.2575736999999663,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.2574744299999452 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.64715585600004,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.646665831000007 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.070194521999952,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.069097155000009 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 8.24497750899991,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.244193743999972 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.598191776999897,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.596911201000012 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}