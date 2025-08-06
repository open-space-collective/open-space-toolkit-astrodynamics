window.BENCHMARK_DATA = {
  "lastUpdate": 1754508113931,
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
          "id": "6746b6d8652092647892c39c81952dc85c4fe7a1",
          "message": "fix: access generator stepping outside analysis interval (#580)\n\n* fix: access generator stepping outside analysis interval\n\n* fix: last step logic",
          "timestamp": "2025-08-06T18:55:41Z",
          "tree_id": "5572eda9a8c1617e338fb6b5da6e7d5471b78675",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6746b6d8652092647892c39c81952dc85c4fe7a1"
        },
        "date": 1754508111559,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 789634707.100015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 773818380.5000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 891851612.4000008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 891796702.7999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3205102871.700001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204925157.6000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3208807346.7000017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3208487385.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3208406162.7000093,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3208092519.3 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}