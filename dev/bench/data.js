window.BENCHMARK_DATA = {
  "lastUpdate": 1733468431653,
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
          "id": "cd884947ab377d678fe08d863e8a9f4c135423fe",
          "message": "chore: add startup script for jupyter image (#470)",
          "timestamp": "2024-12-05T23:42:09-07:00",
          "tree_id": "85eb1574c4d60308037a38bc83f9d841f8f299b2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/cd884947ab377d678fe08d863e8a9f4c135423fe"
        },
        "date": 1733468428502,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 590978782.0999895,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 590941872.1000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 885751000.0999981,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 885716026.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3176524290.600003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3176418189.2000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3180432031.5999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3180278917.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3177794910.800003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3177590333.4999995 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}