window.BENCHMARK_DATA = {
  "lastUpdate": 1737823893604,
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
          "id": "ce35abf5dfe8c3cd30da62ca697884b4e0bcbdeb",
          "message": "feat!: include states with Maneuver (#432)\n\n* wip:\n\n* wip:\n\n* fix: tests\n\n* feat: address remaining feedback\n\n* fix: python tests\n\n* Apply suggestions from code review\n\n* feat: address last bit of feedback\n\n* chore: style\n\n* Apply suggestions from code review\n\nCo-authored-by: coderabbitai[bot] <136622811+coderabbitai[bot]@users.noreply.github.com>\n\n* feat: use maneuver static var\n\n* fix: tests\n\n---------\n\nCo-authored-by: coderabbitai[bot] <136622811+coderabbitai[bot]@users.noreply.github.com>",
          "timestamp": "2025-01-23T17:25:44-06:00",
          "tree_id": "46f17bad0ab4ddbe3d5cdc88bb41099e439d180e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ce35abf5dfe8c3cd30da62ca697884b4e0bcbdeb"
        },
        "date": 1737823892051,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 526063160.4000082,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 525980286.3000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 866858324.2999887,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 866627963.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3195252497.6000066,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3195132053.4000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3152921840.500005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3152801861.0999985 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3129821751.199995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3129736019.2999988 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}