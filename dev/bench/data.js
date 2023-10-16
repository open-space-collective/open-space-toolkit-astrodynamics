window.BENCHMARK_DATA = {
  "lastUpdate": 1697418761047,
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
          "id": "caaddaad98f69105d76debc0a9e137c00ab7752c",
          "message": "refactor: rename files with ':' for windows compatibility (#247)",
          "timestamp": "2023-10-15T17:31:07-07:00",
          "tree_id": "fed2914e24defed59b76b7e9eeabe7cd6c737017",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/caaddaad98f69105d76debc0a9e137c00ab7752c"
        },
        "date": 1697418760068,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 741883583.8000007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 741713740 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1664343014.600007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1664023969.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4134698230.799989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4134179070 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4136220242.0000076,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4135704440.000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4127968436.6000137,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4127475380.0000014 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}