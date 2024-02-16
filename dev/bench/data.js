window.BENCHMARK_DATA = {
  "lastUpdate": 1708118555397,
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
        "date": 1708118553364,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 557120489.4999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 557085510 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796572825.100003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796528500.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2792682421.8999996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792435759.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789578146.3000107,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789422630.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2795467527.200003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2795372179.999998 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}