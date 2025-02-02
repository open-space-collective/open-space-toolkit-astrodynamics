window.BENCHMARK_DATA = {
  "lastUpdate": 1738528788770,
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
      },
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
          "id": "4322cd28cd1ceb235850df932ce9c6d73c952bb1",
          "message": "fix: elevation interval was missing in trajectory based access computation (#509)\n\n* fix: elevation interval was missing in trajectory based access computation\n\n* fix: python bindings",
          "timestamp": "2025-02-02T14:20:49-06:00",
          "tree_id": "7cb6d61ac56594cf274818f4a37e0b7a64d6ef08",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/4322cd28cd1ceb235850df932ce9c6d73c952bb1"
        },
        "date": 1738528786606,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 537009898.1999944,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 536547570.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 874807386.699996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 874741992.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3187064505.299992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186951676.300001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3187490111.700015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3187320927.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3185934694.2000003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3185776499.099997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}