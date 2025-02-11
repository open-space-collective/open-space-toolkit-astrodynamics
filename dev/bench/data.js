window.BENCHMARK_DATA = {
  "lastUpdate": 1739312198916,
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
          "id": "f786ae7d409cc7e531ef0f3d1e89c6780d9c6051",
          "message": "fix: binding naming, imports and docs (#519)",
          "timestamp": "2025-02-11T15:56:33-06:00",
          "tree_id": "ba8712c301417455354d35b28ea7352925c8dc14",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f786ae7d409cc7e531ef0f3d1e89c6780d9c6051"
        },
        "date": 1739312197138,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 539636379.5999889,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 539406521.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 880615148.1999905,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 880588850.3999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3189583945.899989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189433244.300001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3195102016.4,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194926337.400002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3190276527.999998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3190040133.299998 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}