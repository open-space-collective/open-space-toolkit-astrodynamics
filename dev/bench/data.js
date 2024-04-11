window.BENCHMARK_DATA = {
  "lastUpdate": 1712866838362,
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
          "id": "3f8e792a990bf120045545aeb39910437a725ebc",
          "message": "fix: LVLHGDGT orientation (#389)\n\n* fix: align with velocity direction\r\n\r\n* fix: fix orientation of LVLHGDGT frame to be aligned with velocity",
          "timestamp": "2024-04-11T20:03:16Z",
          "tree_id": "f3a0d52ee930dbc93ca3f8dbc469adbd717cdc02",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/3f8e792a990bf120045545aeb39910437a725ebc"
        },
        "date": 1712866836890,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 544532166.199997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 544497703.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791669066.8000114,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791638394.1999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2778160136.800011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2778007784.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2779073652.6000047,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2778886102.499998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2777575651.9999957,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2777448302.2 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}