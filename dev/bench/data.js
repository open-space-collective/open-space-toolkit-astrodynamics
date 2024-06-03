window.BENCHMARK_DATA = {
  "lastUpdate": 1717419250803,
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
          "id": "672e5e55e08729e66138f3702114563c8368e7a4",
          "message": "feat: make coordinate subsets hashable (#402)",
          "timestamp": "2024-06-03T13:57:09+02:00",
          "tree_id": "379ff6b39ded1098a0082798a2df88666297dd2c",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/672e5e55e08729e66138f3702114563c8368e7a4"
        },
        "date": 1717419248949,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 558435679.9999909,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 558408089.3999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796769825.4999902,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796737806.6000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2776493491.0999956,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2776333511.5000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2774336403.0000067,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2774240659.900002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2775156487.9,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2774977858.1999993 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}