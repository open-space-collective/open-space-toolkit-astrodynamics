window.BENCHMARK_DATA = {
  "lastUpdate": 1771262729425,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "35860220+alex-liang3@users.noreply.github.com",
            "name": "Alex Liang",
            "username": "alex-liang3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "2851844363ff9cd8fcb3dd9ef95d26baceca9577",
          "message": "fix: fix starting lower bound when finding precise access AOS (#648)\n\n* fix: fix precise crossing search start\n\n* chore: address comments",
          "timestamp": "2026-02-16T15:05:30Z",
          "tree_id": "9e8f7ccdead1a92f1e54ece782645a43f25baf62",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/2851844363ff9cd8fcb3dd9ef95d26baceca9577"
        },
        "date": 1771256014645,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4721390550.299993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4634528197.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4112310291.5000176,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4111905979.0999985 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7218362471.1,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7217824502.999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7218546479.899992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7218092437.099993 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7216201148.100003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7215749118.899992 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "antoine.paletta@loftorbital.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "antoine.paletta@loftorbital.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "distinct": true,
          "id": "ee00684d04d051ed8f44d7d298edd8eee56379f9",
          "message": "docs: improve numerical solver python docs",
          "timestamp": "2026-02-16T16:56:38Z",
          "tree_id": "cdb1884418245d91682707e949d589f1a8d5bc56",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ee00684d04d051ed8f44d7d298edd8eee56379f9"
        },
        "date": 1771262726650,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4792090978.79999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4645080324.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4125595198.5999956,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4124877323.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7238275793.600001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7237479883.399996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7242693211.900007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7241742936.300005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7250925502.500012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7250104005.499997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}