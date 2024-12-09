window.BENCHMARK_DATA = {
  "lastUpdate": 1733710996496,
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
          "id": "827f75f77688336e62bbc540216388797cd01e02",
          "message": "fix: startup script for jupyter (#471)",
          "timestamp": "2024-12-08T19:04:43-07:00",
          "tree_id": "0eedb04296dcfccf31bfc6f28cffa4034c5f4421",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/827f75f77688336e62bbc540216388797cd01e02"
        },
        "date": 1733710993304,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 589627758.3000085,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 589592527.6000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 884427258.1999916,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 884369169.7000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3188399030.800008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188162319.5999994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3188446686.599997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188298265.7000017 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3196785188.400008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196688772.7999988 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}