window.BENCHMARK_DATA = {
  "lastUpdate": 1707209740048,
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
          "id": "b26f945bb0d2fcbbf457224c53863fff33c999fc",
          "message": "feat: provide initial revolution number for Propagated model (#332)\n\n* feat: provide initial revolution number for Propagated model\r\n\r\n* feat: fix tests\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\r\n\r\n* feat: address feedback\r\n\r\n* feat: add one more test\r\n\r\n---------\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2024-02-01T11:17:36-08:00",
          "tree_id": "c97c2a7d99ce6e2ae463a4701dee34f5ba44af13",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b26f945bb0d2fcbbf457224c53863fff33c999fc"
        },
        "date": 1706816758711,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 550828094.7999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 550806610 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791856884.5999914,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791833870.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2784117434.6999993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783988599.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2783644578.4999967,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783507909.999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2783106717.0999804,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783022880.0000014 ns\nthreads: 1"
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
          "id": "2821cdb3767e80d6baa7fbe9b901a32fa0d85039",
          "message": "feat: new documentation page (#329)\n\n* feat: add orbit pass computation logic\r\n\r\n* feat: add more tests\r\n\r\n* feat: fix tests\r\n\r\n* chore: remove unnecessary variable\r\n\r\n* chore: bad rebase\r\n\r\n* feat: working build, have to add make commands\r\n\r\n* feat: add make command and cleanup\r\n\r\n* feat: working well\r\n\r\n* feat: fix makefile\r\n\r\n* feat: update makefile\r\n\r\n* feat: final fix\r\n\r\n* feat: remove group and update docstrings\r\n\r\n* feat: update index.html\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* chore: fix cpp docs\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2024-02-06T00:17:02-08:00",
          "tree_id": "c0e4ab7c00c7d82fae3b44b1a090ff9bacf0d60d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/2821cdb3767e80d6baa7fbe9b901a32fa0d85039"
        },
        "date": 1707209738772,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 555859002.2000004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 555821600.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 793789373.3999943,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 793749300.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2786938963.7999973,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786821759.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789117246.8999968,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788984899.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2784464342.400008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784317530.000001 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}