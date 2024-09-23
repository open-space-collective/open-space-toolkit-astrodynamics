window.BENCHMARK_DATA = {
  "lastUpdate": 1727125932061,
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
          "id": "6b10bf1800d06ce2db1c8285b0642699ae4b9d7c",
          "message": "feat: always return states in gcrf for trajectory models (#439)\n\n* feat: always return states in GCRF\r\n\r\n* feat: add tests\r\n\r\n* feat: fix python tests\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\r\n\r\n* feat: address feedback\r\n\r\n---------\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2024-09-23T13:54:37-07:00",
          "tree_id": "952e79ddb3cd01517b0a62cacd2dbc1649d33331",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6b10bf1800d06ce2db1c8285b0642699ae4b9d7c"
        },
        "date": 1727125929292,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 576710749.4000015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 576503495.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 874815304.8999939,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 874464333.9000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3190371714.9000044,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189985036.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3189431815.2999973,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189100792.499998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3187809359.2000256,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3187659100.3 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}