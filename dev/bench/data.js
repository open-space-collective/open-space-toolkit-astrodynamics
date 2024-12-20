window.BENCHMARK_DATA = {
  "lastUpdate": 1734690545342,
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
          "id": "911b0ba439f1a4d20b81122b105337990ba13306",
          "message": "build: add python3.13 support (#481)",
          "timestamp": "2024-12-19T21:48:24-08:00",
          "tree_id": "c87f726d45ff37ca974734280f03985af24f1544",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/911b0ba439f1a4d20b81122b105337990ba13306"
        },
        "date": 1734690543702,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 580288840.0999905,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 580255507.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 877166665.9999937,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 877122454.3999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3175285161.399995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3175081701.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3171114233.899982,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3171019637.1000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3182087185.600005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3181828402.900001 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}