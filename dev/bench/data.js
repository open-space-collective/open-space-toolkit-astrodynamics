window.BENCHMARK_DATA = {
  "lastUpdate": 1726869142141,
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
          "id": "af0155d3579d5a1d9a1c4c51ada9b2c921677a4f",
          "message": "fix: state frame conversion in ComputeTargetDirectionVector (#438)",
          "timestamp": "2024-09-20T14:34:59-07:00",
          "tree_id": "b55475d1cae8fc9ca6b48634f0c0e66ea0bab490",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/af0155d3579d5a1d9a1c4c51ada9b2c921677a4f"
        },
        "date": 1726869139504,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 543392788.299991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 543285818.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 857862387.4000016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 857817909.3999994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3200059393.4999986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199700237.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3198465758.7999945,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3198333428.3999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3198718831.9999914,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3198431134.7999964 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}