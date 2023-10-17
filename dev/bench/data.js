window.BENCHMARK_DATA = {
  "lastUpdate": 1697575273336,
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
          "id": "c7eb9fd9e31338599545ca87c211b29b523e57ab",
          "message": "feat: add boost logging + logs for sequence and segments (and solutions) (#251)\n\n* feat: add metadata for segment and sequence solutions\r\n\r\n* wip:\r\n\r\n* feat: use boost log for logging Sequences\r\n\r\n* feat: improve printing everywhere and update logging\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* feat: address remaining feedback and add tests\r\n\r\n* feat: add metadata for segment and sequence solutions (#250)\r\n\r\n* feat: add metadata for segment and sequence solutions\r\n\r\n* chore: missed a spot\r\n\r\n* feat: fix test typo\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* chore: address remaining feedback\r\n\r\n* chore: missed a spot\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* chore: fix static library boost log\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2023-10-17T19:19:55Z",
          "tree_id": "427136321279334f20411d0aa545d6b630a465e9",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/c7eb9fd9e31338599545ca87c211b29b523e57ab"
        },
        "date": 1697572757607,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 695408107.000003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 695124860 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1489790583.9999852,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1487403500 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4213757683.299997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4212578329.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4212757247.9000154,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4211964769.999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4212702299.3000135,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4211998859.999995 ns\nthreads: 1"
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
          "id": "8dc35fab55695319ad86659b055ab8a92355fc58",
          "message": "feat: add custom logging option for conditional solver (#252)\n\n* feat: add custom logging option for conditional solver\r\n\r\n* feat: fix python tests",
          "timestamp": "2023-10-17T20:03:52Z",
          "tree_id": "bea574394b0bbabadf9bd55c84d9942cfec1846e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/8dc35fab55695319ad86659b055ab8a92355fc58"
        },
        "date": 1697575272116,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 865026755.6999893,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 864793980 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1941405037.0999858,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1940521630 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4890164236.5999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4888293980 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4834418646.199987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4832663550.000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4835747193.2000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4831925269.999994 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}