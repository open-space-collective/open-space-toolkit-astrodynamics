window.BENCHMARK_DATA = {
  "lastUpdate": 1701460555172,
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
          "id": "306b07e1d03582eb52b25ce2b692951f4aeef97c",
          "message": "feat: relative/absolute effectivity thresholds for QLaw (#290)\n\n* wip:\r\n\r\n* feat: fix effectivity calculation\r\n\r\n* feat: address remaining feedback",
          "timestamp": "2023-12-01T11:28:41-08:00",
          "tree_id": "ff0afd71d1d35378375809585d860ee0fd37bf48",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/306b07e1d03582eb52b25ce2b692951f4aeef97c"
        },
        "date": 1701460553709,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 553437235.7999928,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 553402050.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791525139.5000155,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791480700.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2785697518.0999823,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2785576330.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2787358747.7999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787223070.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2784454449.4999924,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784304859.999997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}