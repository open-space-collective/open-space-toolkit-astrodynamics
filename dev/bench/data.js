window.BENCHMARK_DATA = {
  "lastUpdate": 1732811169658,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "65550121+phc1990@users.noreply.github.com",
            "name": "Pau Hebrero",
            "username": "phc1990"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "080c7d99a8bd8a97c655b50474684139393f098c",
          "message": "fix: downgrade physics dep. to 10.1 (#467)\n\n* fix: downgrade phwyics dep. to 10.1\r\n\r\n* fix: downgrade physics to 10.x\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2024-11-28T17:05:55+01:00",
          "tree_id": "a75ec5a988e86930b6def2352a5f5b44d9f4e86a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/080c7d99a8bd8a97c655b50474684139393f098c"
        },
        "date": 1732811167115,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 586603390.2999959,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 586578421.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 882368651.8999977,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 882343939.1 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3170893237.199988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3170838372.9000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3169251498.500006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3169210050.4000015 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3170595404.699992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3170501245.400001 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}