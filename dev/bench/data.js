window.BENCHMARK_DATA = {
  "lastUpdate": 1691763640705,
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
        "date": 1691763638976,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 7470328013.899996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7463209950.000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 40226035108.399994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 40222680759.999985 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 44526361896.2,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 44520950090 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 44481873764.900024,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 44475673790.00005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}