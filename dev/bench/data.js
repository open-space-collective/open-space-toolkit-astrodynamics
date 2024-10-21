window.BENCHMARK_DATA = {
  "lastUpdate": 1729542298417,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "44425077+tapiab@users.noreply.github.com",
            "name": "Benoit Tapia",
            "username": "tapiab"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "3a0487a5a63c830894cffa28521c7a8e6971cd7b",
          "message": "fix: fix C++17 build (#450)",
          "timestamp": "2024-10-21T20:07:11Z",
          "tree_id": "00c2a37814bf4f0ed6b42be94eae57cf5f42c05e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/3a0487a5a63c830894cffa28521c7a8e6971cd7b"
        },
        "date": 1729542296010,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 586530808.4999981,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 586493473.5999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 873107060.2999977,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 873052743.3000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3174509790.100001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3174303172.5999994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3171650579.9000063,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3171290476.1999974 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3171929362.5000033,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3171729469.899999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}