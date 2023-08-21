window.BENCHMARK_DATA = {
  "lastUpdate": 1692636190360,
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
          "id": "7ae0ecbfc434a7cf1e3f68023deb1535d8568d02",
          "message": "feat: add benchmarking framework",
          "timestamp": "2023-07-15T11:32:08Z",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/pull/186/commits/7ae0ecbfc434a7cf1e3f68023deb1535d8568d02"
        },
        "date": 1692636188745,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 796462741.900018,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796126020.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1142784322.5000062,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1142322709.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4805762735.100006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4804910859.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4775079111.399998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4774005079.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4800303550.399974,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4799572949.999998 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}