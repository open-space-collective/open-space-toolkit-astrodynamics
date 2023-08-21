window.BENCHMARK_DATA = {
  "lastUpdate": 1692633110231,
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
          "id": "5d94438a5d94c30a43144c51b220fdbca17bfb37",
          "message": "feat: add benchmarking framework",
          "timestamp": "2023-07-15T11:32:08Z",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/pull/186/commits/5d94438a5d94c30a43144c51b220fdbca17bfb37"
        },
        "date": 1692633108956,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1232654031.6000092,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1227662879.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 5200800263.700023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5199387119.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5149296719.000017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5147863160.000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5327176193.500009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5326331020.000002 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}