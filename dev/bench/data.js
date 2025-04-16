window.BENCHMARK_DATA = {
  "lastUpdate": 1744832323965,
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
          "id": "d718fd9a0a9503dcb2be5b6f11d366914321b143",
          "message": "feat: update velocity computation ground strip (#540)\n\n* feat: add geodetic nadir trajectory ground strip\n\n* feat: update tests and python\n\n* Apply suggestions from code review\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n* feat: address remaining feedback\n\n---------\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2025-04-16T19:14:45Z",
          "tree_id": "1bf057728a2263a60a933fd055a05e1be9ab8a8b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/d718fd9a0a9503dcb2be5b6f11d366914321b143"
        },
        "date": 1744832322469,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 541338369.4999993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 541312855.4000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 872910227.6000049,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 872730571.7999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3188373073.699995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188268256.1000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3186797245.4000154,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186689798.1 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3187483976.999999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3187379444.1 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}