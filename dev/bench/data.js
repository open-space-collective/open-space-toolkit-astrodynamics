window.BENCHMARK_DATA = {
  "lastUpdate": 1711629187786,
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
          "id": "fbe0d5046e8fd8947b6bf5ef88e3653ae805b27e",
          "message": "feat: add shared pointer holder types for Orbit and Profile [for Simulation] (#379)\n\nfeat; add shared pointer holder types for Orbit and Profile [for use with simulation]",
          "timestamp": "2024-03-28T05:16:13-07:00",
          "tree_id": "3fc59fc8e380a3dfafb20442fbcbbd751ff59da7",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/fbe0d5046e8fd8947b6bf5ef88e3653ae805b27e"
        },
        "date": 1711629186418,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 554012221.8000022,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 553995662.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 797795858.6,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 797764552.8000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2781816049.3999987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781741014.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2776206795.099995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2776129846.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2780014889.4000072,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2779918206.900001 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}