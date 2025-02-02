window.BENCHMARK_DATA = {
  "lastUpdate": 1738525322323,
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
          "id": "f0b8972f1a00e1c39a72a7b71de2bb21983d81d8",
          "message": "feat: Least Squares Solver (#500)\n\n* feat: Least Squares Solver\n\n* Apply suggestions from code review\n\nCo-authored-by: Pau Hebrero <65550121+phc1990@users.noreply.github.com>\n\n* feat: address remaining feedback\n\n* Apply suggestions from code review\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n* refactor: use public variables for Analysis and Step\n\n* feat: address remaining feedback\n\n* chore: style\n\n* feat: address remaining feedback\n\n* Apply suggestions from code review\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n* wip:\n\n* refactor: rename vars for consistency\n\n* fix: python bindings\n\n* fix: enhance unit tests\n\n* feat: fix state subtraction/addition bug\n\n* fix: tests\n\n---------\n\nCo-authored-by: Pau Hebrero <65550121+phc1990@users.noreply.github.com>\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\nCo-authored-by: Antoine Paletta <antoine.paletta@loftorbital.com>",
          "timestamp": "2025-02-02T13:23:12-06:00",
          "tree_id": "084326c320cb2ef1e03249aa7bc14a19c79a8626",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f0b8972f1a00e1c39a72a7b71de2bb21983d81d8"
        },
        "date": 1738525320062,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 532818471.90000916,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 532658765.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 871913615.5000057,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 871869696.7000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3192156495.699993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3191956635.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3193663993.5999894,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193585815.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3200597053.899997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3200370009.100003 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}