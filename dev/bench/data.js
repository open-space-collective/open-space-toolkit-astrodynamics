window.BENCHMARK_DATA = {
  "lastUpdate": 1713194778734,
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
          "id": "52c3ab3b8ad464fa78c8de83efb7b3d3480d9d32",
          "message": "build: add ostk-io to requirements.txt (#391)",
          "timestamp": "2024-04-15T15:09:41Z",
          "tree_id": "1995aa15dfdd560232d7e1ca393f6e2bc4026c31",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/52c3ab3b8ad464fa78c8de83efb7b3d3480d9d32"
        },
        "date": 1713194776995,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 550340557.700008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 550318936.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 798974898.0000058,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 798936155.9000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2793532009.799998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2793426929.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789383970.0999937,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789284524.399999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2790402581.5000067,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2790295425.399999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}