window.BENCHMARK_DATA = {
  "lastUpdate": 1724783600851,
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
          "id": "133716b085a0fdd218c104636581dad9dce813bc",
          "message": "refactor: local orbital frame provider (#431)\n\n* feat: add custom local orbital frame provider\r\n\r\n* feat: allow custom local orbital frames\r\n\r\n* feat: add more tests\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Pau Hebrero <65550121+phc1990@users.noreply.github.com>\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <65550121+phc1990@users.noreply.github.com>",
          "timestamp": "2024-08-27T11:13:37-07:00",
          "tree_id": "44a1d345f4bf398aec3ba86ba087709a5488ef11",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/133716b085a0fdd218c104636581dad9dce813bc"
        },
        "date": 1724783598214,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 541448242.2999924,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 541413156.3000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 869572253.9999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 869510862.7000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3171082953.899997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3170895178.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3176354822.2,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3176171946.1000013 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3172158455.999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3172000342.000003 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}