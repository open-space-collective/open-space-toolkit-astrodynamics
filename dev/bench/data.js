window.BENCHMARK_DATA = {
  "lastUpdate": 1705711442229,
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
          "id": "142c4d0e6c3eec3624c6944802e4de8eb226a7f6",
          "message": "refactor: change the index returned by pass map to be the last index (#323)\n\nrefactor: change the index returned by pass map to be the last index",
          "timestamp": "2024-01-19T16:15:42-08:00",
          "tree_id": "c157fcd7a71ac482f717ceca0b1fc5012d0260a1",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/142c4d0e6c3eec3624c6944802e4de8eb226a7f6"
        },
        "date": 1705711441494,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 558758607.8999947,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 558642940 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796973971.0000112,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796920029.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2793163421.7000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2793042660.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2794685650.5000086,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794381589.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2791608839.6000133,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791480629.999998 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}