window.BENCHMARK_DATA = {
  "lastUpdate": 1724235396375,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "136001643+AlexandreLoftOrbital@users.noreply.github.com",
            "name": "AlexandreLoftOrbital",
            "username": "AlexandreLoftOrbital"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "df5fb49c3b61a04803b89c93f79f38cb23864252",
          "message": "chore: bump Physics to 9.0 (#428)",
          "timestamp": "2024-08-20T23:17:37Z",
          "tree_id": "a18a0b43b0219e4bea39d97532a2c09d1610f637",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/df5fb49c3b61a04803b89c93f79f38cb23864252"
        },
        "date": 1724235394024,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 546450498.1000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 546430826.9000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 869855224.7000067,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 869811538.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3187066483.7999926,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186791365.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3188813646.7000036,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188709926.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3184137550.7000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3183897157.000003 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}