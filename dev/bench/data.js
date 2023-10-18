window.BENCHMARK_DATA = {
  "lastUpdate": 1697588754437,
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
          "id": "901d00521c431105781cf86a6d54743812d50e80",
          "message": "feat: add missing args for StateBuilder bindings (#255)\n\n* feat: add missing bindings for StateBuilder\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2023-10-17T23:44:19Z",
          "tree_id": "c92a5a8871ea6ad9113822df190fbdcf20d16823",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/901d00521c431105781cf86a6d54743812d50e80"
        },
        "date": 1697588753428,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 699765189.00001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 699625250.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1491869688.3999984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1491664139.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4147709492.2999983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4147354319.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4146347711.6999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4145959760.0000024 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4157144288.300003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4156780970.000004 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}