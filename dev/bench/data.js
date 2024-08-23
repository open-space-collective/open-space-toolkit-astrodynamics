window.BENCHMARK_DATA = {
  "lastUpdate": 1724382764906,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "136001643+AlexandreLoftOrbital@users.noreply.github.com",
            "name": "AlexandreLoftOrbital",
            "username": "AlexandreLoftOrbital"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "df5fb49c3b61a04803b89c93f79f38cb23864252",
          "message": "chore: bump Physics to 9.0 (#428)",
          "timestamp": "2024-08-20T23:17:37Z",
          "tree_id": "a18a0b43b0219e4bea39d97532a2c09d1610f637",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/df5fb49c3b61a04803b89c93f79f38cb23864252"
        },
        "date": 1724235394024,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 546450498.1000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 546430826.9000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 869855224.7000067,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 869811538.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3187066483.7999926,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186791365.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3188813646.7000036,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188709926.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3184137550.7000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3183897157.000003 ns\nthreads: 1"
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
          "id": "122a3e0c85b6c887d4294d5bc105bf2c9c493c0e",
          "message": "feat: getBodyFrame and getAxesAt for Tabulated Profile (#429)\n\n* feat: getBodyFrame and getAxesAt for Tabulated Profile\r\n\r\n* Apply suggestions from code review",
          "timestamp": "2024-08-21T21:37:26Z",
          "tree_id": "d2d1f77a7a2178e7c2e900e279ad44dff08489ed",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/122a3e0c85b6c887d4294d5bc105bf2c9c493c0e"
        },
        "date": 1724277280474,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 551445749.0999973,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 551423778.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 875489661.3000085,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 875459276.6999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3172865964.100009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3172739739.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3170615449.399997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3170460801.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3171740711.2999926,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3171583862.4 ns\nthreads: 1"
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
          "id": "50cdf2a5199e18014d0e1e5b6643c191210c210b",
          "message": "chore: c++ 17 compatibility (#430)\n\n* feat: c++ 17 compatibility\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2024-08-23T02:15:12Z",
          "tree_id": "8be61ac37ddd35fe627a2cf268c78e6bee993396",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/50cdf2a5199e18014d0e1e5b6643c191210c210b"
        },
        "date": 1724382762657,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 553820272.5999951,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 553794846.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 866255258.0000067,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 866221456.5999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3172993908.7999926,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3172906825.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3173346093.000009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3173257781.900002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3173229097.4000025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3173151653.6 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}