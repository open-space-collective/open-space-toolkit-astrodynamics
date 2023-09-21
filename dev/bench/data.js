window.BENCHMARK_DATA = {
  "lastUpdate": 1695320656286,
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
      }
    ]
  }
}