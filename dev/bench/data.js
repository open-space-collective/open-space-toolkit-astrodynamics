window.BENCHMARK_DATA = {
  "lastUpdate": 1739796055719,
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
          "id": "9ffed92858911991eafbe2064b24fac82e4cd85f",
          "message": "feat: simplify elevation computations (#522)\n\n* feat: simplify elevation computations and improve performance\n\n* feat: add tests\n\n* chore: style\n\n* Apply suggestions from code review\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n* feat: address remaining feedback\n\n---------\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2025-02-15T22:26:57Z",
          "tree_id": "07133f569d2a6a928494099679113e0c9d372bc4",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/9ffed92858911991eafbe2064b24fac82e4cd85f"
        },
        "date": 1739659595188,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 546910239.9000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 546732439.1000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 878518362.2000034,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 878382876.7000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3190948445.100003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3190720643.9000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3191830944.4999976,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3191695183.4000015 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3194709450.3000073,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194566045.8999968 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "f6823e644e85aba54b84792cf4c200b3efbb37c5",
          "message": "chore: rename groundTarget to accessTarget (#523)",
          "timestamp": "2025-02-17T12:21:03Z",
          "tree_id": "e0d2304101404dde7905cdb3dd5c9ec784e1d8a7",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f6823e644e85aba54b84792cf4c200b3efbb37c5"
        },
        "date": 1739796054353,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 536537831.6999966,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 536512310.6999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 873437165.2999926,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 873404939.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3185200649.100011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3185104373.9000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3183896373.9000177,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3183845964.800001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3184968141.9000034,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3184875454.200002 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}