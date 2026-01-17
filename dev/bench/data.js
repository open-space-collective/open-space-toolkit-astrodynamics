window.BENCHMARK_DATA = {
  "lastUpdate": 1768608668903,
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
          "id": "01afa3e4724b40c1844b50235223eb60587c39da",
          "message": "feat: add lower/upper bound for RootSolver Solution (#635)\n\n* feat: add lower/upper bound for RootSolver Solution\n\n* chore: format",
          "timestamp": "2026-01-16T08:22:20-08:00",
          "tree_id": "8245342675e095a9843999fecca07a128e9a077c",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/01afa3e4724b40c1844b50235223eb60587c39da"
        },
        "date": 1768582215486,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4830618545.799996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4714571518.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4077924329.7999983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4077453226.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7206130220.099998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7205377649.100001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7197363584.799995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7196506983.100001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7192529107.7,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7191712059.900002 ns\nthreads: 1"
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
          "id": "fe9563bdeb896ea9cc08b4b0b0ef32f48a9e9655",
          "message": "fix: numerical solver root bounding (#638)\n\n* fix: improve InstantCondition and NumericalSolver crossing detection\n\n- Fix InstantCondition to calculate time relative to target instant instead of J2000\n- Simplify Array concatenation in Segment.cpp\n- Update NumericalSolver tests for new InstantCondition API\n\nCo-Authored-By: Claude Opus 4.5 <noreply@anthropic.com>\n\n* feat: add Pau's tests\n\n* chore: fix typos\n\n---------\n\nCo-authored-by: Claude Opus 4.5 <noreply@anthropic.com>",
          "timestamp": "2026-01-16T15:43:56-08:00",
          "tree_id": "bc6e06b70a152699dca9a3dd4dc99edb73a5cad2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/fe9563bdeb896ea9cc08b4b0b0ef32f48a9e9655"
        },
        "date": 1768608666935,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4850904535.800004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4712051380.800001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4084930702.3000054,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4084472081.199998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7194077278.000003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7193258634.200002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7193935977.800005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7193247059.1 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7193815304.300016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7192960314.600003 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}