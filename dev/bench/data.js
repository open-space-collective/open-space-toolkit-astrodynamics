window.BENCHMARK_DATA = {
  "lastUpdate": 1726638706605,
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
      },
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
          "id": "09d82b14f92543fe65d28c26678877751e41bb9b",
          "message": "feat!: handful of fixes and improvements to utility methods (#435)\n\n* feat: handful of fixes and improvements to utility methods\r\n\r\n* chore: fix tests",
          "timestamp": "2024-09-17T21:53:55-07:00",
          "tree_id": "11aac8814a83b85617d0548681bc4831f6260600",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/09d82b14f92543fe65d28c26678877751e41bb9b"
        },
        "date": 1726638704317,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 544400941.5000097,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 544372738.8000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 875307686.799988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 875267970.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3175741672.1000085,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3175624746.200001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3176521521.6999993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3176219994.9000015 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3174867082.5000033,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3174769365.3000026 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}