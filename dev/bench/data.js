window.BENCHMARK_DATA = {
  "lastUpdate": 1762893924877,
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
          "id": "684be293e910c5e61578f2912451ed17e35379a9",
          "message": "refactor: use const for heterogeneous guidance law (#598)\n\n* refactor: use const for heterogenous guidance law\n\n* fix: format",
          "timestamp": "2025-11-11T19:07:34Z",
          "tree_id": "4327536dae020e9cb50e35f0ddcf6deb80aa6c4e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/684be293e910c5e61578f2912451ed17e35379a9"
        },
        "date": 1762889355832,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 996021766.0000069,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 816953350.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 894244376.6999872,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 894171655.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3199544404.3000135,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199287632.299999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3197077294.3000154,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196852021.900001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3197201177.7999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196975737.2000017 ns\nthreads: 1"
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
          "id": "9baf153348dda16d123e93eace10b58103be8337",
          "message": "feat: allow Q-Law COE domain selection (#599)\n\n* feat: allow Q-Law COE domain selection\n\n* feat: clean it up a bit\n\n* feat; add some tests\n\n* Apply suggestions from code review\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n* feat: address feedback\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2025-11-11T12:23:17-08:00",
          "tree_id": "c242dde84a7a9f9abc5121d2aa2e2035df034a38",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/9baf153348dda16d123e93eace10b58103be8337"
        },
        "date": 1762893923356,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 865900384.2000004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 828158189.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 914691188.8000034,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 914593862.7000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3205039482.4000025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204634836.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3203337438.099993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3202795189.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3203751381.400008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3203292850.1000013 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}