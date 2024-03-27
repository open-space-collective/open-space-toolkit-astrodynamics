window.BENCHMARK_DATA = {
  "lastUpdate": 1711582840852,
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
          "id": "ebbe2c17921e60397bb03aa1fdc3475d8d29df3a",
          "message": "chore: align dockerfiles with ostk-simulation (#378)\n\n* chore: align dockerfiles with ostk-simulation\r\n\r\n* Update Dockerfile\r\n\r\n* fix: ostk-data inside sub-folder",
          "timestamp": "2024-03-27T22:22:34Z",
          "tree_id": "f35bba3ec757b10f1f1482e16b9fb25429453b62",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ebbe2c17921e60397bb03aa1fdc3475d8d29df3a"
        },
        "date": 1711582839356,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 547150896.3000076,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 547137232.5999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794645835.5000118,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794628523.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2785760966.200013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2785675679.3999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2784265777.7000055,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784212025.8999987 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2783464076.6000123,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783326991.899997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}