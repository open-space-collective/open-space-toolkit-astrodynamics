window.BENCHMARK_DATA = {
  "lastUpdate": 1766014931582,
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
          "id": "256fd10eb869846c7700cc36707214d8b8c557c9",
          "message": "fix: constant lof direction and constraint maneuver segment only producing one maneuver (#624)\n\n* test: add test to track the issue\n\n* fix: constant lof and constraint segment missing  maneuvers beyond the first one\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2025-12-17T23:16:20Z",
          "tree_id": "16037111a651a98709a0627bee7c925d192151e8",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/256fd10eb869846c7700cc36707214d8b8c557c9"
        },
        "date": 1766014930115,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4722749892.100006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4708303168.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4105644888.099988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4105204292.899999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7217928727.500021,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7217041121.799995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7221191785.7,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7220323064.600001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7217704574.699997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7216830165.800014 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}