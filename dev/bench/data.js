window.BENCHMARK_DATA = {
  "lastUpdate": 1744184356772,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
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
          "id": "3af4f20c33f0dd9b3846eab3793cd0cd93f789c9",
          "message": "feat: add ballistic coefficient subset (#538)\n\n* feat: add ballistic coefficient subset\n\n* fix: format",
          "timestamp": "2025-04-09T06:34:42Z",
          "tree_id": "4d8f202ea2db289093e19e556353c64b1bf6ea94",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/3af4f20c33f0dd9b3846eab3793cd0cd93f789c9"
        },
        "date": 1744184354848,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 543557374.7000091,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 543450140.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 880364682.1000086,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 880306624.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3194316351.00002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194020167.4999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3191852669.9000154,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3191695067.899998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3199749181.3999887,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199545108.800001 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}