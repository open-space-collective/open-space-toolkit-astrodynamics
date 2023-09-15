window.BENCHMARK_DATA = {
  "lastUpdate": 1694794295367,
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
      },
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
          "id": "1af89e3bc79d9c69c30b3f9eba7fa35341dfdb56",
          "message": "test: add NRLMSISE00 propagation validation tests (#225)\n\n* test: space weather data file from Kyle\r\n\r\n* test: regenerate Orekit validation files using numerical propagation app\r\n\r\n* test: fix Kyle tests",
          "timestamp": "2023-09-15T17:38:18+02:00",
          "tree_id": "e9b169f5129c6056a9de0d5650c2f5889d1786bd",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/1af89e3bc79d9c69c30b3f9eba7fa35341dfdb56"
        },
        "date": 1694794294303,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 693239874.7999969,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 693165879.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1478734568.0000072,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1478422639.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4138313705.8000097,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4137420030 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4139382872.199997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4138427850 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4138966373.3999953,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4138210500.0000024 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}