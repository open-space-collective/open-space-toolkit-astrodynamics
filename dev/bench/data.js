window.BENCHMARK_DATA = {
  "lastUpdate": 1701218851334,
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
          "id": "809be19392c2cfdc81d2c3e761cf5a859854732d",
          "message": "feat: use analytical derivatives for QLaw (#272)\n\n* feat: use analytical derivatives for QLaw\r\n\r\n* feat: add enum for gradient computation\r\n\r\n* feat: fix python bindings and tests\r\n\r\n* feat: fix remaining few nans\r\n\r\n* feat: cleanup from rebase",
          "timestamp": "2023-11-28T16:19:48-08:00",
          "tree_id": "7db09c23160052d85105fcc4a10d23366311bca8",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/809be19392c2cfdc81d2c3e761cf5a859854732d"
        },
        "date": 1701218850702,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 561516357.7999965,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 561484340.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792775215.9000022,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792730609.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2783440610.7999997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783322749.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2781593233.4999957,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781468249.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2782091291.0000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781966949.999999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}