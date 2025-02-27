window.BENCHMARK_DATA = {
  "lastUpdate": 1740665206157,
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
          "id": "d91c39d5330a3e73e0e18e42ac681a3efcdf8eeb",
          "message": "fix: dangling references when custom pointing (#530)\n\n* fix: dangling references when custom pointing\n\n* feat: add a test\n\n* feat: add tests",
          "timestamp": "2025-02-26T04:10:25Z",
          "tree_id": "e4a12972b20153c0e62c9d95d1db47317eac553b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/d91c39d5330a3e73e0e18e42ac681a3efcdf8eeb"
        },
        "date": 1740665204011,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 552982779.9999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 552923423.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 890481278.4000057,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 890246429 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3193485341.600007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193070274.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3189884903.6000113,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189749861.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3189850850.9999943,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189714861.500001 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}