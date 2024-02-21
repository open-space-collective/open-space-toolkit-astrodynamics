window.BENCHMARK_DATA = {
  "lastUpdate": 1708539971799,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "98616558+apaletta3@users.noreply.github.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "956bb7b4145517238720c7414ff1efd9aa8f7217",
          "message": "docs: add release badge (#363)",
          "timestamp": "2024-02-21T18:10:14Z",
          "tree_id": "dc94435a4787adac5641f4df13feb9d1c1d2b60e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/956bb7b4145517238720c7414ff1efd9aa8f7217"
        },
        "date": 1708539969586,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 543975561.3999921,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 543959930 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 800654686.0000014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 800500569.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2797121159.800008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2797031900 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2796374510.299989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2796281160.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2794647824.099991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794294109.9999986 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}