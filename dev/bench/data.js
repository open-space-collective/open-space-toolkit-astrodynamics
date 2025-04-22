window.BENCHMARK_DATA = {
  "lastUpdate": 1745344362509,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "137164378+loft-nicolas-sanchez@users.noreply.github.com",
            "name": "loft-nicolas-sanchez",
            "username": "loft-nicolas-sanchez"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "4dc1ab5a8a811ca04bb5216d847bb9ad4b5780d0",
          "message": "feat: return self in viewer methods (#545)",
          "timestamp": "2025-04-22T17:32:28Z",
          "tree_id": "c675a9549c9e8f14a626d880d0b3b02af6060a4e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/4dc1ab5a8a811ca04bb5216d847bb9ad4b5780d0"
        },
        "date": 1745344361006,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 535494399.5999974,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 535481449.1 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 867199439.6000059,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 866761638.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3176889741.90001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3176631916.4000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3186559476.30001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186458803.8999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3172030814.9000177,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3171832363.1999993 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}