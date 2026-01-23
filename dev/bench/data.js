window.BENCHMARK_DATA = {
  "lastUpdate": 1769197542359,
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
          "id": "a496ee65c433390c4e6e8454c1f9953b7afd4936",
          "message": "test: add a Constant LOF Maneuver regression test (#641)\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-01-23T11:10:06-08:00",
          "tree_id": "2d454c21b1dd5322acc4a254b59c436d369b287e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/a496ee65c433390c4e6e8454c1f9953b7afd4936"
        },
        "date": 1769197540731,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4757039221.500013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4658841606.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4070743351.2999954,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4070453755.1000023 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7182498841.500012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7181808089.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7182605022.29997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7182087066.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7172251491.00001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7171779953.899997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}