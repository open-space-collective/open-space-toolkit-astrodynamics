window.BENCHMARK_DATA = {
  "lastUpdate": 1701691767352,
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
      },
      {
        "commit": {
          "author": {
            "email": "43561754+Derollez@users.noreply.github.com",
            "name": "Remy Derollez",
            "username": "Derollez"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "1be8bfdb47de4e7744137e659eef84e4c72c43be",
          "message": "chore: use eta variables for return statement to remove build warning (#292)",
          "timestamp": "2023-12-04T11:41:46Z",
          "tree_id": "eabb659b657d177f354f39fb84fd7a011829be20",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/1be8bfdb47de4e7744137e659eef84e4c72c43be"
        },
        "date": 1701691766776,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 553236074.7999973,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 553209280 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794745810.5999942,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794709660.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2791369533.0999928,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791258719.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2795800137.100008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2795695420.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2797987141.0000043,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2797872390.0000005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}