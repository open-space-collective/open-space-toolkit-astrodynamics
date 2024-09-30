window.BENCHMARK_DATA = {
  "lastUpdate": 1727737475901,
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
          "id": "6b10bf1800d06ce2db1c8285b0642699ae4b9d7c",
          "message": "feat: always return states in gcrf for trajectory models (#439)\n\n* feat: always return states in GCRF\r\n\r\n* feat: add tests\r\n\r\n* feat: fix python tests\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\r\n\r\n* feat: address feedback\r\n\r\n---------\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2024-09-23T13:54:37-07:00",
          "tree_id": "952e79ddb3cd01517b0a62cacd2dbc1649d33331",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6b10bf1800d06ce2db1c8285b0642699ae4b9d7c"
        },
        "date": 1727125929292,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 576710749.4000015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 576503495.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 874815304.8999939,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 874464333.9000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3190371714.9000044,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189985036.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3189431815.2999973,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189100792.499998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3187809359.2000256,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3187659100.3 ns\nthreads: 1"
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
          "id": "dbd1fa3c491d7087eeda7516880898a12cce1f24",
          "message": "feat: compute LTAN (#437)\n\n* feat: fix tests\r\n\r\n* feat: add better tests and LTAN\r\n\r\n* feat: fix tests\r\n\r\n* feat: use latest physics",
          "timestamp": "2024-09-24T22:12:53Z",
          "tree_id": "71744b27b72761fa245f8d98c021750c5ccb22e3",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/dbd1fa3c491d7087eeda7516880898a12cce1f24"
        },
        "date": 1727217148218,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 582195918.2000058,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 582112739.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 872626752.399998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 872581659.5000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3192982435.6999974,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192860078.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3190305843.400006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3190134345.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3193406989.5999928,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193262499.8999968 ns\nthreads: 1"
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
          "id": "67b94b7914e7956c9b4a5b074a90c616fe87562e",
          "message": "style: update code docstrings (#440)\n\n* style: update code docstrings\r\n\r\n* chore: fix\r\n\r\n* chore: remove test",
          "timestamp": "2024-09-30T15:47:26-07:00",
          "tree_id": "3403ec566baa851876509ac6f75cde0c46bcfd9f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/67b94b7914e7956c9b4a5b074a90c616fe87562e"
        },
        "date": 1727737473286,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 579823746.0999985,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 579778853.0999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 876166721.4,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 875836047.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3192879717.599999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192719162.100001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3192662007.500013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192328194.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3192759206.89999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192580880.3999968 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}