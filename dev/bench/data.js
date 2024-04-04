window.BENCHMARK_DATA = {
  "lastUpdate": 1712238055912,
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
      },
      {
        "commit": {
          "author": {
            "email": "65550121+phc1990@users.noreply.github.com",
            "name": "Pau Hebrero",
            "username": "phc1990"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "c7a29212c5363aca563c561a88e5ff31903e1fa5",
          "message": "feat: expose Local Orbital Frame Factory parent Frame (#370)\n\n* feat: expose Local Orbital Frame Factory parent Frame\r\n\r\n* chore: apply MR suggestions\r\n\r\n* tests: fixt LocalOrbitalFrameFactory tests\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2024-04-04T15:24:24+02:00",
          "tree_id": "41aa54ce2373575a5d98d3ebc00b9a5876d50bc5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/c7a29212c5363aca563c561a88e5ff31903e1fa5"
        },
        "date": 1712238054433,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 547107684.6000017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 547095080.2999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792100823.5000045,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792065394.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2791576080.3999944,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791449304.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2796775712.2999954,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2796694625.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2790872688.499991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2790790754.6999993 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}