window.BENCHMARK_DATA = {
  "lastUpdate": 1694018223723,
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
      }
    ]
  }
}