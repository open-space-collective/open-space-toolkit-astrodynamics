window.BENCHMARK_DATA = {
  "lastUpdate": 1704235300089,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
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
          "id": "c68e61ec7d7f86c5f666ca8b2dbd7337f5f2a0ec",
          "message": "build: use the shorthand seed data env var (#306)",
          "timestamp": "2023-12-20T10:06:23+01:00",
          "tree_id": "ed1bcd0c8270614e04c9dda098525d0765efbc11",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/c68e61ec7d7f86c5f666ca8b2dbd7337f5f2a0ec"
        },
        "date": 1703065183882,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 556517673.899998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 556486420 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791742560.900002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791693850.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2787612161.900012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787483419.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2783198067.100011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783053769.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2787218722.199998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787086890.0000005 ns\nthreads: 1"
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
          "id": "bb710b4754f1142fe53269ab8f49d1b8dd0de598",
          "message": "test: update past data expectation with wiggle room for CSSI rollover (#308)\n\ntest: update past data failure expectation with wiggle room for CSSI rollover",
          "timestamp": "2024-01-02T12:44:40-08:00",
          "tree_id": "5a46cb5d664e8e55596f1a74b5bb603d71ed362a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/bb710b4754f1142fe53269ab8f49d1b8dd0de598"
        },
        "date": 1704229910082,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 555248976.099989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 555229789.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791086978.0999974,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791060689.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2800634598.4999995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2800415979.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2793550776.6,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2793460019.9999976 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2795970846.100005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2795854009.999998 ns\nthreads: 1"
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
          "id": "e9c4d174144ecb4f17b5045cfa87afdb104c4b04",
          "message": "chore: add sequence solution default constructor bindings (#309)",
          "timestamp": "2024-01-02T22:14:22Z",
          "tree_id": "a4464411af8009075c0d94a30cd823e280c65022",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e9c4d174144ecb4f17b5045cfa87afdb104c4b04"
        },
        "date": 1704235299460,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 541708426.2999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 541681540 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 785632216.800002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 785586599.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2795084752.7000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794956690 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2793038547.9999986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792958959.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2794680632.3000035,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794526729.9999995 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}