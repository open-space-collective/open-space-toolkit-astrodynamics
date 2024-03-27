window.BENCHMARK_DATA = {
  "lastUpdate": 1711530213619,
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
          "id": "9e2d2aa29bfec060a4d64a6ffa31e78edf14cb82",
          "message": "build: arm64 with debug symbols off (#377)\n\n* build: arm64 with debug symbols off\r\n\r\n* temp: point to other branch for ci\r\n\r\n* fix: debug symbols toggle\r\n\r\n* fix: package build on pr workflow branch\r\n\r\n* fix: point back to main for workflow",
          "timestamp": "2024-03-27T09:47:02+01:00",
          "tree_id": "92b1be133c24d347772157b82a776b881a23632d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/9e2d2aa29bfec060a4d64a6ffa31e78edf14cb82"
        },
        "date": 1711530212342,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 550253917.4000049,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 550243483.5000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796642337.8000002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796615424.0999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2785818687.300008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2785728941.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2785918255.000007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2785856280.7000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2790458502.6999884,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2790403219.3999996 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}