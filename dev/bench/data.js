window.BENCHMARK_DATA = {
  "lastUpdate": 1775072527094,
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
          "id": "b1f8c846c2b1393e41e5ae915f13a13c652b9779",
          "message": "fix: local orbital frame getTransformAt comparison (#661)\n\n* fix: local orbita frame getTransformAt comparison\n\n* chore: style",
          "timestamp": "2026-04-01T19:05:10Z",
          "tree_id": "7c92594702b02dfb685b0542940143ff02e71c1f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b1f8c846c2b1393e41e5ae915f13a13c652b9779"
        },
        "date": 1775072524858,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4546045560.200008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4527280452.500001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4035770178.000018,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4035436557.6999993 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7155262170.300006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7154654136.499998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7153423632.700003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7152813273.800001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7151389135.0000105,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7150923002.000005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}