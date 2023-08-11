/** Copyright © Loft Orbital Solutions Inc. */

window.BENCHMARK_DATA = {
  "lastUpdate": 1691760410445,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "name": "open-space-collective",
            "username": "open-space-collective"
          },
          "committer": {
            "name": "open-space-collective",
            "username": "open-space-collective"
          },
          "id": "4074b642964cde6a87e813e58c82961fd4fe5988",
          "message": "feat: add benchmarking framework",
          "timestamp": "2023-07-15T11:32:08Z",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/pull/186/commits/4074b642964cde6a87e813e58c82961fd4fe5988"
        },
        "date": 1691757216964,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 7250704210.900017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7244453970.000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 30593727167.900024,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 30588553730.000008 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 30559702602.899982,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 30554176989.99999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 33425676501.100018,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 33420154630.00001 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "open-space-collective",
            "username": "open-space-collective"
          },
          "committer": {
            "name": "open-space-collective",
            "username": "open-space-collective"
          },
          "id": "4074b642964cde6a87e813e58c82961fd4fe5988",
          "message": "feat: add benchmarking framework",
          "timestamp": "2023-07-15T11:32:08Z",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/pull/186/commits/4074b642964cde6a87e813e58c82961fd4fe5988"
        },
        "date": 1691760408727,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 6502170034.899973,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6498832199.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 28542083618.800007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 28540435910 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 28538496149.899994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 28536556540.000008 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 28520565391.00001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 28518744340 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}