window.BENCHMARK_DATA = {
  "lastUpdate": 1694021857761,
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
          "id": "5ee8a20d50f563ee65b0c41b93538393825d9ad6",
          "message": "feat: add TrajectorySegment class (#208)\n\n* feat: add TrajectorySegment class\r\n\r\n* feat: fix test\r\n\r\n* feat: address feedback\r\n\r\n* feat: simplify interface as suggested by Kyle\r\n\r\n* feat: fix tests",
          "timestamp": "2023-09-06T09:02:24-07:00",
          "tree_id": "2ffa9320712151b530c83131f8c80f313c799268",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/5ee8a20d50f563ee65b0c41b93538393825d9ad6"
        },
        "date": 1694018222238,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 718416754.7999834,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 718274789.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1613997644.1000044,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1613640909.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4122835264.8000116,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4121740780 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4121987751.999995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4121346850.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4116944334.799985,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4116358760.0000014 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "43561754+Derollez@users.noreply.github.com",
            "name": "Remy Derollez",
            "username": "Derollez"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "6903ebcb587a6dc69d1ad8ef67510e368b73a1b7",
          "message": "feat: add Local Orbital Frames (#198)\n\n* feat: add dummy local orbital frame and provider classes\r\n\r\n* refactor: simplify Local Orbital Frame addition (#200)\r\n\r\n* refactor: attempt to simplify\r\n\r\n* feat:  update models and design\r\n\r\n* chore: style\r\n\r\n* feat: go back to no generator and provider instantiation at each Frame generation\r\n\r\n* test: add unit tests on frame factory\r\n\r\n* chore: style\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>\r\nCo-authored-by: Remy Derollez <remy@loftorbital.com>\r\n\r\n* feat: add other LOF types\r\n\r\n* chore: style\r\n\r\n* feat: add local orbital frame direction intermediate class\r\n\r\n* feat: add getters to frame direction\r\n\r\n* fix: bug in VNC transform provider\r\n\r\n* chore: style\r\n\r\n* feat: add undefined constructor on frame factory\r\n\r\n* feat: change naming strategy for LOF factory\r\n\r\n* chore: style\r\n\r\n* docs: add docstrings in header files\r\n\r\n* feat: first pass on bindings addition\r\n\r\n* test: add unit tests for LOFdirection\r\n\r\n* chore: minor tweaks and improvements\r\n\r\n* test: add python tests\r\n\r\n* chore: style\r\n\r\n* chore: style and 1 test\r\n\r\n* test: add unit test on transform frame provider\r\n\r\n* chore: style and apply suggestions\r\n\r\n* chore: apply suggestions\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <65550121+phc1990@users.noreply.github.com>\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>\r\nCo-authored-by: vishwa shah <vishwa2710@gmail.com>",
          "timestamp": "2023-09-06T17:02:47Z",
          "tree_id": "85568ac54309da36dbad1e908e69a58c5f4a97ef",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6903ebcb587a6dc69d1ad8ef67510e368b73a1b7"
        },
        "date": 1694021856101,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 1164757442.9999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1164549859.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2198855353.6999917,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2198470389.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 5597310623.599986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5596552850.000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5481167678.800012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5477933150 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5488055923.7999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5486909909.999997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}