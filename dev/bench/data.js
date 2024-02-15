window.BENCHMARK_DATA = {
  "lastUpdate": 1707955953344,
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
          "id": "6287426a5e26a57687fe8e2ebd3c70cc9e298806",
          "message": "docs: add missing modules (#350)",
          "timestamp": "2024-02-14T15:48:47-08:00",
          "tree_id": "b24518d42f61aaa6dc761bceb6ebcc6239f7c9a9",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6287426a5e26a57687fe8e2ebd3c70cc9e298806"
        },
        "date": 1707955951717,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 534913725.00000125,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 534886580 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 779343658.700003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 779311260.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2707727297.2000003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2707633740.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2691473155.700015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2691379459.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2712191336.2000155,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2712088269.999998 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}