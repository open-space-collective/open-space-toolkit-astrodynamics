window.BENCHMARK_DATA = {
  "lastUpdate": 1739228024514,
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
          "id": "9f6be69e03c6a22e6daa8fb0c7d2af6ea579d474",
          "message": "feat: od least squares solver (#501)\n\n* feat: add C++ parts of OD Least Squares Solver\n\n* feat: add python bindings and tests\n\n* feat: address remaining feedback\n\n* feat: address remaining feedback\n\n* chore: style\n\n* feat: update from rebase\n\n* refactor: update LeastSquaresSolver.Analysis interface\n\n* chore: style",
          "timestamp": "2025-02-02T15:57:21-06:00",
          "tree_id": "ec82b0f7eb9383aaeb07542bf8859d312d4d22ac",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/9f6be69e03c6a22e6daa8fb0c7d2af6ea579d474"
        },
        "date": 1738534562583,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 542093030.799998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 542059142.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 876488522.1999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 876421700.0000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3197484284.200004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197123662.1000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3203611684.800006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3203414847.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3198796978.100012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3198584737.0000014 ns\nthreads: 1"
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
          "id": "abd465b8aa14c02947c67fc79c9a1b03d5b9d205",
          "message": "feat: tle solver (#510)\n\n* feat: add python bindings and tests\n\n* feat: address remaining feedback\n\n* feat: address remaining feedback\n\n* chore: style\n\n* feat: update from rebase\n\n* refactor: update LeastSquaresSolver.Analysis interface\n\n* chore: style\n\n* feat: TLE solver for estimating TLEs\n\n* feat: cleanup from rebase\n\n* chore: style\n\n* feat: add accessors\n\n* feat: compute residuals in observation frame\n\n* chore: format",
          "timestamp": "2025-02-05T19:25:54-06:00",
          "tree_id": "812cadbd95f21f81316cd351bfbd2fd6ebe7d2f2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/abd465b8aa14c02947c67fc79c9a1b03d5b9d205"
        },
        "date": 1738806367255,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 538658420.1999995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 538473832.8000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 876228463.1,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 876164761.7999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3187501190.3999963,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3187347241.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3185601610.40001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3185426217.900001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3186234338.7999887,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186083865.899998 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "e50b4ecb9b1b937d89bda9ebe91aab40faf8dfc9",
          "message": "build: turn BUILD BENCHMARK off by default (#514)",
          "timestamp": "2025-02-10T11:21:35Z",
          "tree_id": "3f1cac2359d1f8d1b2466d0163943079bf156d29",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e50b4ecb9b1b937d89bda9ebe91aab40faf8dfc9"
        },
        "date": 1739187687465,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 536463995.4999916,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 536419329.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 872931672.8000072,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 872872846.5000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3197722469.0000014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197571851.5999985 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3198173265.200006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3198066403.500001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3190743240.1999927,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3190639133.400002 ns\nthreads: 1"
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
          "id": "a6075a4babe28f73a2eded1057f6badae74c0d11",
          "message": "feat: improve OD solver and TLE solver interfaces (#512)\n\n* feat: address remaining feedback\n\n* chore: style\n\n* feat: add pythin bindings\n\n* fix: OD solver suggestions (#513)\n\n* fix: OD solver suggestions\n\n* fix: tests\n\n* fix: align naming to be consistent with TLESolver <-> ODSolver\n\n* chore: style\n\n* fix: tests\n\n* Apply suggestions from code review\n\n---------\n\nCo-authored-by: vishwa shah <vishwa2710@gmail.com>\n\n* fix: tests and docs\n\n* feat: improve TLE Solver interface (#515)\n\n* feat: improve tle solver interface\n\n* Update src/OpenSpaceToolkit/Astrodynamics/Estimator/TLESolver.cpp\n\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>\n\n---------\n\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>\n\n* chore: style\n\n---------\n\nCo-authored-by: Vishwa <vishwa@Vishwas-MacBook-Air.local>\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\nCo-authored-by: Antoine Paletta <antoine.paletta@loftorbital.com>",
          "timestamp": "2025-02-10T23:34:08+01:00",
          "tree_id": "77ac163d9d398824845414f6581718b29e146a4b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/a6075a4babe28f73a2eded1057f6badae74c0d11"
        },
        "date": 1739228022623,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 540835647.7999803,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 540693214.4000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 875060136.799999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 875008031.5999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3188086366.499999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3187911405.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3191864794.199995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3191785203.4000015 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3187594036.000007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3187440159.800002 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}