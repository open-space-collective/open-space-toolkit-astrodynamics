window.BENCHMARK_DATA = {
  "lastUpdate": 1730406729813,
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
          "id": "75f26892d663bdebe5ea9ccf192190fdab1df886",
          "message": "feat: add more fields for pystate.from_dict (#453)",
          "timestamp": "2024-10-31T07:51:06-07:00",
          "tree_id": "502355188ea5e6ff0499d6c7b16e9093266affe5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/75f26892d663bdebe5ea9ccf192190fdab1df886"
        },
        "date": 1730387371419,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 592191814.4000018,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 592165365.8000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 880499410.4999993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 880427407.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3175123512.2000027,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3174789518.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3176185892.500001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3176054772.5000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3179416397.500006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3179310188.6000013 ns\nthreads: 1"
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
          "id": "503eae8c1d7f9c36cbc6f76a5cd748df4e5806d7",
          "message": "feat: add getInterval methods to the Solution classes for Sequence/Solution classes (#455)\n\n* feat: add getInterval methods to the Solution classes for Sequence/Segment (and tests)\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\r\n\r\n* feat: address feedback\r\n\r\n---------\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2024-10-31T21:14:07+01:00",
          "tree_id": "a82de9a34be41dd23e427a369d40fc9d957fb0d8",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/503eae8c1d7f9c36cbc6f76a5cd748df4e5806d7"
        },
        "date": 1730406727063,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 583570678.6000116,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 583514932.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 879472784.5000012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 879384019.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3172115641.900001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3171861723.7000017 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3172221949.499999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3172039976.3999977 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3172838880.6,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3172645599.8999996 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}