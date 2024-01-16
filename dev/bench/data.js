window.BENCHMARK_DATA = {
  "lastUpdate": 1705445675205,
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
          "id": "bdb65b92a45fde7479e1f7fbced5a6e25c4ae9c0",
          "message": "fix: set instant from inputs in NumericalSolver::integrateTime (#320)",
          "timestamp": "2024-01-17T09:26:37+11:00",
          "tree_id": "4aaead60f697e66636f96ae7e7cf74ed14c4e35f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/bdb65b92a45fde7479e1f7fbced5a6e25c4ae9c0"
        },
        "date": 1705445673741,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 553438927.3999921,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 553431239.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792521432.0000066,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792415739.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2796425414.6000032,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2796301780 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2795195645.4999957,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2795110909.999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2794197402.0999966,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794118149.999997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}