window.BENCHMARK_DATA = {
  "lastUpdate": 1740320120481,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "35860220+alex-liang3@users.noreply.github.com",
            "name": "Alex Liang",
            "username": "alex-liang3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "8ee514e72c200e65555d5db6a9883d1612f79df6",
          "message": "style: re-run format with clang 18 (#528)",
          "timestamp": "2025-02-22T17:55:10+01:00",
          "tree_id": "d5e19ddeebf7cffb18dc840d10eb17bc31b48130",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/8ee514e72c200e65555d5db6a9883d1612f79df6"
        },
        "date": 1740320118737,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 539402581.2999985,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 539395347.9000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 878402265.6000048,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 878151094.4000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3198977715.6000004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3198650107 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3199096078.1000013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3198916846.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3198829755.8999987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3198735268.7999988 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}