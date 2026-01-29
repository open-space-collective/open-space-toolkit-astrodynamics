window.BENCHMARK_DATA = {
  "lastUpdate": 1769698439446,
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
          "id": "668f536f05e8d4831aa7d618e7adaba925166937",
          "message": "fix: filter out leading zero mass flow rates in Maneuvers (#646)\n\n* test: add regression test\n\n* wip: for Vishwa\n\n* Revert \"wip: for Vishwa\"\n\nThis reverts commit 1af201dc5b8335451ea80191d76212b171bc5436.\n\n* fix: filter out leading zeroes on maneuver profiles\n\n* refactor: apply PR suggestions\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-01-29T14:17:22Z",
          "tree_id": "b56d3cc351bc67764eb3ad97766c1c30a27bde23",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/668f536f05e8d4831aa7d618e7adaba925166937"
        },
        "date": 1769698437546,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4805887227.099993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4659383270.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4059706888.7999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4059360607.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7158195665.599998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7157751302.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7171340166.899996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7170714656.399994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7179191829.000002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7178260560.799986 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}