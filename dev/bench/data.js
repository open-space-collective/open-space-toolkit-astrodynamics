window.BENCHMARK_DATA = {
  "lastUpdate": 1694781470024,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "43561754+Derollez@users.noreply.github.com",
            "name": "Remy Derollez",
            "username": "Derollez"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "3b311c5c8fb28b511cf249c85b8c2b71e6837123",
          "message": "test: fix 320km exponential Orekit validation case (#224)\n\n* test: regenerate 320km altitude validation data and update tests\r\n\r\n* chore: style",
          "timestamp": "2023-09-15T14:00:10+02:00",
          "tree_id": "5c0c436dd9ecbf2eebd6be37361e7b6bdd078dfe",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/3b311c5c8fb28b511cf249c85b8c2b71e6837123"
        },
        "date": 1694781468976,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 738192128.2999997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 738038839.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1659177280.3000197,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1658624249.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4168802337.200003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4167679370.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4169363422.9999986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4168286260.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4169541058.2000074,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4167955909.9999976 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}