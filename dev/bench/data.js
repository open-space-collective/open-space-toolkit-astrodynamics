window.BENCHMARK_DATA = {
  "lastUpdate": 1706611065171,
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
          "id": "f30b9b8ade7c50e49c0fa84e1f3fd17ced25e170",
          "message": "feat: add new validation scenarios (#330)\n\n* feat: add all nw scenarios\r\n\r\n* feat: add 30 min maneuvers\r\n\r\n* fix: constant thrust exponential scenario",
          "timestamp": "2024-01-30T10:09:58Z",
          "tree_id": "d8230c21193a85a381d4ea73a6cd262e4cdde110",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f30b9b8ade7c50e49c0fa84e1f3fd17ced25e170"
        },
        "date": 1706611063856,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 552894385.299993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 552880150.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791312116.5000064,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791293400.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2783397175.900012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783319910.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2794285717.6999893,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794148310 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2782989877.300008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782839900.0000005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}