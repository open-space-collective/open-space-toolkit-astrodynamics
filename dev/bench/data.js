window.BENCHMARK_DATA = {
  "lastUpdate": 1772757146528,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
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
          "id": "ef2e2664f87cec3df5498d9fa5e34b3a33227a1c",
          "message": "chore: improve make clean command",
          "timestamp": "2026-02-18T11:26:29Z",
          "tree_id": "618348bf223b3f03e111c3d00a1b3d656a07c261",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ef2e2664f87cec3df5498d9fa5e34b3a33227a1c"
        },
        "date": 1771415898495,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4811341975.299996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4687145117.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4164613226.600005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4164044013.8000007 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7262048797.200015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7261372656.600006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7265657518.200005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7265107006.599998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7267949884.799986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7267342778.900002 ns\nthreads: 1"
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
          "id": "541313ec25a955ef01e9bff9e35e8665613bbaa0",
          "message": "docs: c++ documentation (#651)\n\n* docs: add C++ documentation\n\n* docs: add C++ documentation",
          "timestamp": "2026-03-05T14:54:29-08:00",
          "tree_id": "b1c6e3580896add9ac1ac57e56be354df308b11f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/541313ec25a955ef01e9bff9e35e8665613bbaa0"
        },
        "date": 1772754256936,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 3441675462.600005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3405656267.4999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 3245533271.499994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3245241692.2000012 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6041203824.799998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6040664522.700002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6039000375.399991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6038508334.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6037337419.099987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6036876232.199995 ns\nthreads: 1"
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
          "id": "765c177e69857f0abd25bdee4f09f113432da226",
          "message": "feat: add option to normalize LeastSquaresSolver inputs (#652)\n\n* fix: normalize TLE inputs when estimating\n\n* chore: format\n\n* feat: normalize at least squares solver level\n\n* refactor: use normalization functions\n\n* docs: add code examples\n\n* feat: address coderabbit feedback",
          "timestamp": "2026-03-06T00:02:52Z",
          "tree_id": "51d9391b9d9fc55ceeb238063ace0307ca34d227",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/765c177e69857f0abd25bdee4f09f113432da226"
        },
        "date": 1772757144583,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4671728100.900054,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4638351454.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4122664690.6000037,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4122249295.1999984 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7251319626.199939,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7250635204.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7251638953.800011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7250936916.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7258656253.400044,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7257879305.5 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}