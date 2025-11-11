window.BENCHMARK_DATA = {
  "lastUpdate": 1762889357632,
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
          "id": "684be293e910c5e61578f2912451ed17e35379a9",
          "message": "refactor: use const for heterogeneous guidance law (#598)\n\n* refactor: use const for heterogenous guidance law\n\n* fix: format",
          "timestamp": "2025-11-11T19:07:34Z",
          "tree_id": "4327536dae020e9cb50e35f0ddcf6deb80aa6c4e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/684be293e910c5e61578f2912451ed17e35379a9"
        },
        "date": 1762889355832,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 996021766.0000069,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 816953350.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 894244376.6999872,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 894171655.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3199544404.3000135,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199287632.299999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3197077294.3000154,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196852021.900001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3197201177.7999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196975737.2000017 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}