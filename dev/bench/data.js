window.BENCHMARK_DATA = {
  "lastUpdate": 1695838979697,
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
          "id": "d816da0ef52c5f28516388625cfae21a290a929b",
          "message": "feat: add getCoordinatesSubsets method to States (#226)\n\n* feat: add getCoordinatesSubsets method to States\r\n\r\n* style: re-order to put method next to other getters",
          "timestamp": "2023-09-21T10:45:33-07:00",
          "tree_id": "c6b7c0aaf2f63fd64edcdd16f4e268f056e30933",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/d816da0ef52c5f28516388625cfae21a290a929b"
        },
        "date": 1695320655676,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 693467060.3000029,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 693397219.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1476146409.900008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1476021359.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4142377877.799993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4141610610.0000024 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4143550426.800016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4142735380 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4144032487.699997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4143234150.0000024 ns\nthreads: 1"
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
          "id": "6e065ee7e3601ba2fb047e0aab314310e7793456",
          "message": "feat: add Sequence class (#223)\n\n* feat: simplify interface as suggested by Kyle\r\n\r\n* feat: fix tests\r\n\r\n* feat: simplify interface as suggested by Kyle\r\n\r\n* feat: first cut at Sequence\r\n\r\n* feat: update tests\r\n\r\n* fix: COE true anomaly calculation\r\n\r\n* feat: add more tests\r\n\r\n* chore: style\r\n\r\n* feat: address comments, add tests for bindings\r\n\r\n* feat: rebase from main MR\r\n\r\n* chore: missed a few spots\r\n\r\n* feat: fix tests",
          "timestamp": "2023-09-21T20:28:49Z",
          "tree_id": "54f087408faff8d2b7d554a50edf8e7dbbbc7633",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6e065ee7e3601ba2fb047e0aab314310e7793456"
        },
        "date": 1695330281340,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 694118355.7000158,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 694050890.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1477272626.8999918,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1476629690.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4137402579.7000113,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4136464990 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4137277530.4999833,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4136482049.9999976 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4141471053.2999945,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4140899680.000001 ns\nthreads: 1"
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
          "id": "7cbb3bdbaaf0312ffdafdea5d8957a2c2fb64f39",
          "message": "feat: add static constructors for constant thrust (#228)\n\n* feat: add static constructors for constant thrust\r\n\r\n* chore: reset propagator.test.cpp\r\n\r\n* feat: improve tests\r\n\r\n* feat: fix test",
          "timestamp": "2023-09-27T10:42:22-07:00",
          "tree_id": "0ea754cedab726845126c2be483fb045f553f769",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/7cbb3bdbaaf0312ffdafdea5d8957a2c2fb64f39"
        },
        "date": 1695838978356,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 696765939.4000008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 696526690.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1486454915.7999818,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1486179549.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4142612238.7000074,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4141805760.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4143899572.6000032,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4143251269.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4142665712.300004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4141823559.9999976 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}