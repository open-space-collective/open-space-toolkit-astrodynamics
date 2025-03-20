window.BENCHMARK_DATA = {
  "lastUpdate": 1742498568631,
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
          "id": "6f739131254ffa3e882d953b4222f9705c0b82a3",
          "message": "feat: add extended states to dataframe utility (#535)\n\n* feat: add extended states to dataframe utility\n\n* Apply suggestions from code review\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n---------\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2025-03-20T14:02:55-05:00",
          "tree_id": "28ed1d160f31adb3a2de2840f6682e2a578c7066",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6f739131254ffa3e882d953b4222f9705c0b82a3"
        },
        "date": 1742498567298,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 542829714.6999967,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 542651618.4000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 877814356.9000008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 877732352.2000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3199362555.6999917,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199178754.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3203281831.499993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3203145707.1999974 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3199999376.700015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199875805.2000017 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}