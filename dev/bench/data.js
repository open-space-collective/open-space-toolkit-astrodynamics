window.BENCHMARK_DATA = {
  "lastUpdate": 1746037392209,
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
          "id": "96ab38866dd984bd760fad24a58795bee2db2733",
          "message": "feat: target velocity profile target type (#548)\n\n* feat: target velocity profile target type\n\n* feat: address feedback",
          "timestamp": "2025-04-30T13:03:01-05:00",
          "tree_id": "c221a36a91735cd13f7f98ef55f9cf70fca57edb",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/96ab38866dd984bd760fad24a58795bee2db2733"
        },
        "date": 1746037389678,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 542313812.3000001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 542237363.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 888517593.1999981,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 888087347.3999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3195195069.0999923,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194621889.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3195452119.000004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3195197017.600002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3196718858.199995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196409784.1000013 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}