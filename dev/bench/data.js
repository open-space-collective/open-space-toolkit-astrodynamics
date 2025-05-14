window.BENCHMARK_DATA = {
  "lastUpdate": 1747242370633,
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
          "id": "7007551986b9a3588668a7e39fdaa912b4d9ecb4",
          "message": "fix: ground trajectory discontinuities due to finite differencing step size (#553)\n\n* fix: finite difference step size\n\n* fix: tests\n\n* chore: style\n\n* fix: test tolerance",
          "timestamp": "2025-05-14T11:43:06-05:00",
          "tree_id": "38912896fa515b593c6f429aa0280b6f2f5900c2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/7007551986b9a3588668a7e39fdaa912b4d9ecb4"
        },
        "date": 1747242368483,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 538754837.5999984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 538726321.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 878950814.8000038,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 878683859.6000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3196782950.3000135,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196461277.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3195345212.3999968,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3195204884.600002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3198222990.299985,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3198072129.3999977 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}