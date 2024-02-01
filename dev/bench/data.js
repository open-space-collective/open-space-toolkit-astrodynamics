window.BENCHMARK_DATA = {
  "lastUpdate": 1706816759411,
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
          "id": "b26f945bb0d2fcbbf457224c53863fff33c999fc",
          "message": "feat: provide initial revolution number for Propagated model (#332)\n\n* feat: provide initial revolution number for Propagated model\r\n\r\n* feat: fix tests\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\r\n\r\n* feat: address feedback\r\n\r\n* feat: add one more test\r\n\r\n---------\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2024-02-01T11:17:36-08:00",
          "tree_id": "c97c2a7d99ce6e2ae463a4701dee34f5ba44af13",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b26f945bb0d2fcbbf457224c53863fff33c999fc"
        },
        "date": 1706816758711,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 550828094.7999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 550806610 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791856884.5999914,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791833870.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2784117434.6999993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783988599.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2783644578.4999967,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783507909.999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2783106717.0999804,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783022880.0000014 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}