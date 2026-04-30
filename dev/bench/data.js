window.BENCHMARK_DATA = {
  "lastUpdate": 1777542288752,
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
      },
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
          "id": "609a7f54e380b590776d5ea1bbfd5d662c0350e4",
          "message": "fix: add missing QLaw bindings (#672)\n\nfix: bindings",
          "timestamp": "2026-04-30T11:14:51+02:00",
          "tree_id": "c9426968ff9e62fb42bad518e23c0a37a713f4aa",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/609a7f54e380b590776d5ea1bbfd5d662c0350e4"
        },
        "date": 1777542286287,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4776817175.700012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4618401577.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4064122395.300006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4063607962.0999985 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7202814499.099993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7202330226.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7195565716.700003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7194936816.200002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7210036566.400003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7209464460.900009 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}