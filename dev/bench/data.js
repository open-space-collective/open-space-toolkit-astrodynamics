window.BENCHMARK_DATA = {
  "lastUpdate": 1723225650791,
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
          "id": "989f86295e21afed775db56652d67d4388e31a35",
          "message": "refactor!: remove unnecessary converters and add 'Z' parsing (#427)\n\nrefactor: remove unnecessary converters and add 'Z' parsing",
          "timestamp": "2024-08-09T10:29:28-07:00",
          "tree_id": "5d46e8393cb41dc44dd8e44b633c7980ebd4b873",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/989f86295e21afed775db56652d67d4388e31a35"
        },
        "date": 1723225647730,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 556059062.9000046,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 556018196.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 871128353.1999868,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 871074978.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3174018318.3000116,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3173837183.7000012 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3171412316.6000147,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3171267780.7999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3172536297.600033,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3172398965.399998 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}