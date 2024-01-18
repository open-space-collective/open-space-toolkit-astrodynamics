window.BENCHMARK_DATA = {
  "lastUpdate": 1705607653443,
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
          "id": "bdb65b92a45fde7479e1f7fbced5a6e25c4ae9c0",
          "message": "fix: set instant from inputs in NumericalSolver::integrateTime (#320)",
          "timestamp": "2024-01-17T09:26:37+11:00",
          "tree_id": "4aaead60f697e66636f96ae7e7cf74ed14c4e35f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/bdb65b92a45fde7479e1f7fbced5a6e25c4ae9c0"
        },
        "date": 1705445673741,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 553438927.3999921,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 553431239.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792521432.0000066,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792415739.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2796425414.6000032,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2796301780 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2795195645.4999957,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2795110909.999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2794197402.0999966,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794118149.999997 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "98616558+apaletta3@users.noreply.github.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "6e89a7f8ee7b26ce9d2495f844066232f40998fb",
          "message": "feat: add repropagation of intermediate states to sequence and segment (#316)\n\n* feat: add computeStatesAt methods for segment and sequence\r\n\r\n* feat: add python bindings and UTs\r\n\r\n* docs: apply vishwa's suggestions\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>\r\n\r\n* chore: add a couple more unit tests\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2024-01-18T19:26:13Z",
          "tree_id": "e895c1d0eb2d929b3c60b6c3606f08301b5e7a1d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6e89a7f8ee7b26ce9d2495f844066232f40998fb"
        },
        "date": 1705607652398,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 558748541.5999936,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 558696260 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794918998.6000022,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794888330.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2782007779.900016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781898359.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2784820279.2000113,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784543319.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2780043314.5999875,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2779922480.0000005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}