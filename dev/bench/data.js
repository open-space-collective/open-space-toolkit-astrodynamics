window.BENCHMARK_DATA = {
  "lastUpdate": 1721151811188,
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
          "id": "10cb83aa1217030da417cadc2905cc4752f8b1eb",
          "message": "build: bump numpy, ostk-core, ostk-math, ostk-physics (#422)\n\n* build: bump numpy\r\n\r\n* Update requirements.txt\r\n\r\n* Update requirements.txt",
          "timestamp": "2024-07-16T18:49:10+02:00",
          "tree_id": "762a170b738d046edda5a9d1f6be55b96355773b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/10cb83aa1217030da417cadc2905cc4752f8b1eb"
        },
        "date": 1721151809547,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 553322922.5999889,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 553303897.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 793918998.2000073,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 793894311.4999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2788035867.2000055,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787961178.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789665397.7999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789591902.300001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2790869403.5000052,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2790795978.900002 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}