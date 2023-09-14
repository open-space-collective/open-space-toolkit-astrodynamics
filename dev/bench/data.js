window.BENCHMARK_DATA = {
  "lastUpdate": 1694688218311,
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
          "id": "77fa2aaf8e3681d2f6f4bcc915a36d3a606ecdf2",
          "message": "fix: fix angle delta calculations in Event Conditions (#220)\n\n* fix: fix angle delta calculations in Event Conditions\r\n\r\n* feat: first cut at Angular Condition\r\n\r\n* feat: fix tests\r\n\r\n* feat: style and tests\r\n\r\n* feat: fix remaining tests\r\n\r\n* feat: fix tests for real\r\n\r\n* feat: address remaining feedback",
          "timestamp": "2023-09-11T23:04:05Z",
          "tree_id": "bc3e2b5ea8ca7f04ae790aa205373d5e172bad17",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/77fa2aaf8e3681d2f6f4bcc915a36d3a606ecdf2"
        },
        "date": 1694475578944,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 682143666.6000011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 682079029.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1443793178.6999967,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1443678679.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4147754647.900007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4147516989.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4148592181.300006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4147987380.0000024 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4148933720.400004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4148031509.999994 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "kyle.ray.cochran@gmail.com",
            "name": "kyle-cochran",
            "username": "kyle-cochran"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "7024164307131a647a994c74e255d3bb891c5196",
          "message": "build: add CMAKE cache option for Python versions (#221)",
          "timestamp": "2023-09-14T12:08:49+02:00",
          "tree_id": "c1e89d6e14d568305fcc237c0a15427f99713774",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/7024164307131a647a994c74e255d3bb891c5196"
        },
        "date": 1694688216580,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 856871185.6999983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 856735439.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1930197999.899997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1929689330 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4889765554.599996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4888201860 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4890371753.999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4888344820 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4888389588.100017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4887096260.000002 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}