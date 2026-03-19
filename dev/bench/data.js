window.BENCHMARK_DATA = {
  "lastUpdate": 1773950956832,
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
          "id": "3cd1d8530c8f5cd0978ec5dcdc533cdab1bc6b71",
          "message": "refactor: use boost::regex for thread safety (#656)\n\n* refactor: use boost::regex for thread safety\n\n* Apply suggestions from code review\n\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2026-03-19T12:40:18-07:00",
          "tree_id": "4d3b6f4309a471a83e8760bbb11dbd2f4ae164c8",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/3cd1d8530c8f5cd0978ec5dcdc533cdab1bc6b71"
        },
        "date": 1773950955239,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4728366625.500007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4656952623.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4137798274.3000016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4137357473.7000017 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7259425500.799995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7258682978.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7254735628.100001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7253879625.799997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7248789720.500008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7248191747.600003 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}