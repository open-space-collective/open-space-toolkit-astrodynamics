window.BENCHMARK_DATA = {
  "lastUpdate": 1697565334449,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "vishwa2710@gmail.com",
            "name": "Vishwa Shah",
            "username": "vishwa2710"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "caaddaad98f69105d76debc0a9e137c00ab7752c",
          "message": "refactor: rename files with ':' for windows compatibility (#247)",
          "timestamp": "2023-10-15T17:31:07-07:00",
          "tree_id": "fed2914e24defed59b76b7e9eeabe7cd6c737017",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/caaddaad98f69105d76debc0a9e137c00ab7752c"
        },
        "date": 1697418760068,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 741883583.8000007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 741713740 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1664343014.600007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1664023969.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4134698230.799989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4134179070 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4136220242.0000076,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4135704440.000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4127968436.6000137,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4127475380.0000014 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "vishwa2710@gmail.com",
            "name": "Vishwa Shah",
            "username": "vishwa2710"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "8907f7d50e1ffacaff3fd01d88c6c7bce6b2afa4",
          "message": "fix: fix missing binding for Segment::Solution (#253)",
          "timestamp": "2023-10-17T10:08:47-07:00",
          "tree_id": "d7fc79adbd4eac7df5c63a0a95dd1d6791da326b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/8907f7d50e1ffacaff3fd01d88c6c7bce6b2afa4"
        },
        "date": 1697565333392,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 696827563.3999951,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 696776090 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1479400698.099994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1479267869.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4143721584.799988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4143455219.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4145226144.999981,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4144638230 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4144807616.599996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4144472429.9999976 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}