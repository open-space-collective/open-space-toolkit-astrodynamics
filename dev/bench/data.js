window.BENCHMARK_DATA = {
  "lastUpdate": 1699472171340,
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
          "id": "79d7ac2377439a1b140df7d5c068dc55d845c3e7",
          "message": "fix: fix relative logical conditions usage with Sequences (#269)\n\n* fix: fix relative logical conditions usage with Sequences\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* feat: fix last test\r\n\r\n* feat: add print test\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2023-11-08T10:42:32-08:00",
          "tree_id": "09b1f0fc5838ee72c8d129a0a41ed8b9933afa2f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/79d7ac2377439a1b140df7d5c068dc55d845c3e7"
        },
        "date": 1699472169119,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 877665144.9999803,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 877295540.0000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1965459416.4999936,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1964771810.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4976028503.800013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4974863820 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5000121275.700007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4998870669.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4988577794.799982,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4987416520.000002 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}