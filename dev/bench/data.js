window.BENCHMARK_DATA = {
  "lastUpdate": 1712886617813,
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
          "id": "f056a9aae8f62d1c8f3052eda41e6627238a26d9",
          "message": "build: requirements pinned to floating minor (#390)\n\n* build: requirements pinned to floating minor\r\n\r\n* build: remove IO\r\n\r\n* chore: fix dockerfile formatting",
          "timestamp": "2024-04-12T01:31:30Z",
          "tree_id": "dba72f35e56cca76d5fef5b7ef8175240aceef13",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f056a9aae8f62d1c8f3052eda41e6627238a26d9"
        },
        "date": 1712886615716,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 540950456.1000063,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 540922236 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 790900235.6999962,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 790856073.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2794234796.8999853,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794116617.1000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789162518.0000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789121835.2000012 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2792252182.5000105,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792214717.999998 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}