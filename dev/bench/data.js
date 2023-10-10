window.BENCHMARK_DATA = {
  "lastUpdate": 1696897383970,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "kyle.ray.cochran@gmail.com",
            "name": "kyle-cochran",
            "username": "kyle-cochran"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "973343a01931575f39843dbd7aebfdc770370512",
          "message": "feat: add functions to extract multiple subsets at once. (#234)\n\n* feat: add functions to State and CoordinatesBroker to extract multiple subsets at once.\r\n\r\n* style: missed a couple of doc-string newlines\r\n\r\n* feat: add bindings (and fix typo bug\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2023-10-09T16:45:24-07:00",
          "tree_id": "3eb9066bb2e33db74fe48e4c08bb68b3a5c43a56",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/973343a01931575f39843dbd7aebfdc770370512"
        },
        "date": 1696897382398,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 822385429.7999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 822261089.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1857799782.3000035,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1857324519.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4689460322.1,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4687877980 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4621238339.400008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4619962179.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4634613640.899994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4633127270 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}