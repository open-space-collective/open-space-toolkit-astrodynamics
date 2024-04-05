window.BENCHMARK_DATA = {
  "lastUpdate": 1712324507252,
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
          "id": "f42bf9d22b1612d02dae7b340facdad4d23e9569",
          "message": "fix: variable naming error (#383)",
          "timestamp": "2024-04-05T15:25:12+02:00",
          "tree_id": "21b81f11b15ac2dd42161ec13410facff39a2eb6",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f42bf9d22b1612d02dae7b340facdad4d23e9569"
        },
        "date": 1712324505434,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 543407408.0999892,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 543375436.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 790716240.9999955,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 790687734.7000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2780535312.4000077,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2780392977.7000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2781236262.000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781013166.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2780675557.7000103,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2780544881.8000016 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}