window.BENCHMARK_DATA = {
  "lastUpdate": 1711950070324,
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
          "id": "78f4969b8b5adb76ac5ca549642e0ab42b17a15c",
          "message": "chore: update dependabot interval (#382)",
          "timestamp": "2024-03-31T22:22:22-07:00",
          "tree_id": "ceb7c7933bdb7a4aeee8df98cea8ce2378e164d7",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/78f4969b8b5adb76ac5ca549642e0ab42b17a15c"
        },
        "date": 1711950068600,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 544106518.5999947,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 544081970.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796114874.3000023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796074080.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2779820603.200017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2779704709.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2781281634.6999965,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781149817.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2777788757.100001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2777642238.099999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}