window.BENCHMARK_DATA = {
  "lastUpdate": 1718300542525,
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
          "id": "33fabf48e2ae986e11ae17cd71f7608a2ffe4790",
          "message": "feat: add backward propagation get pass with rev number (#410)\n\n* feat: add reverse proapgation for orbit.getPassWithNumber\r\n\r\n* feat: ensure it works with a seeded pass\r\n\r\n* refactor: simplify forward backward propagation\r\n\r\n* feat: address remaining feedback\r\n\r\n* feat: simplify further\r\n\r\n* feat: cleanup\r\n\r\n* feat: python bindings\r\n\r\n* chore: fix pass test\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* feat: address comments\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2024-06-13T17:23:23Z",
          "tree_id": "798aba20186871d279922cff053c49aad4a73c1d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/33fabf48e2ae986e11ae17cd71f7608a2ffe4790"
        },
        "date": 1718300541308,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 556140444.7999961,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 556105395.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 797584587.4999892,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 797541777.8000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2787705210.7999988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787539522.2999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2785808304.7999854,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2785675428.600003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2788384977.3999944,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788229000.700001 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}