window.BENCHMARK_DATA = {
  "lastUpdate": 1773759312407,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "65550121+phc1990@users.noreply.github.com",
            "name": "Pau Hebrero",
            "username": "phc1990"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e0a2615d6b8b5ff2d652567a38dbefb836225cc7",
          "message": "feat: solve with multiple maneuvers history (#657)\n\n* feat: solve with multiple maneuvers history\n\n* refactor: address most of MR suggestions\n\n* refactor: overload solve method\n\n* refactor: address MR suggestion\n\n* refactor: amend typo\n\n* amend previous maneuver checking\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-03-17T14:20:12Z",
          "tree_id": "564a0351981e301a0ea2a586f7f62e49bc718a13",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e0a2615d6b8b5ff2d652567a38dbefb836225cc7"
        },
        "date": 1773759310603,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4774896594.099988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4674187643.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4132661861.100007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4132264528.600001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7257312958.799997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7256488606.2999935 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7243191462.999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7242424864.399996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7233731407.099981,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7232974116.8 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}