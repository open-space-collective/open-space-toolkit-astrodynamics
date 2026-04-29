window.BENCHMARK_DATA = {
  "lastUpdate": 1777488547014,
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
          "id": "0e7cdf482b58a6503a071aaef4156f00619ae14c",
          "message": "fix: analytical gradient calculations in QLaw (#670)",
          "timestamp": "2026-04-29T20:19:20+02:00",
          "tree_id": "9da3020ec973a90fbf0237e81a98ba9a2d0eec07",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/0e7cdf482b58a6503a071aaef4156f00619ae14c"
        },
        "date": 1777488544757,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4734951118.399994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4582853853.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4053922262.1000023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4053587326.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7183130297.7000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7182550197.599998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7187154950.799998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7186608337.899994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7190605609,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7190017310.199997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}