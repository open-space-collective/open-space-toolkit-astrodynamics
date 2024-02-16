window.BENCHMARK_DATA = {
  "lastUpdate": 1708090198098,
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
          "id": "419c50ef072dd9c03ce2dd69da170b9e7d0520e3",
          "message": "refactor: propulsion system to use Reals instead of Scalar (#353)\n\n* refactor: propulsion system to use Reals instead of Scalar\r\n\r\n* fix: shift to string issue",
          "timestamp": "2024-02-16T14:05:26+01:00",
          "tree_id": "3420506dfebf68e27d5891cdcd46bd760014e82d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/419c50ef072dd9c03ce2dd69da170b9e7d0520e3"
        },
        "date": 1708090196462,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 557684229.900002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 557663300.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 798736947.6999959,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 798713049.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2790017766.9000015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789935519.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789833428.199995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789739480 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2789079450.100013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789004389.999998 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}