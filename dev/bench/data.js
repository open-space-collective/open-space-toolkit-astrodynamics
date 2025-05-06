window.BENCHMARK_DATA = {
  "lastUpdate": 1746570397049,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "137164378+loft-nicolas-sanchez@users.noreply.github.com",
            "name": "loft-nicolas-sanchez",
            "username": "loft-nicolas-sanchez"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e09372248ca9765d5849ec415a8e4ee30440ae07",
          "message": "feat: add celestial body direction to viewer (#549)\n\n* feat: add sun direction to viewer\n\n* feat: generalize for any celestial body\n\n* fix: set celestial as mandatory argument\n\n* chore: apply suggestions\n\n* feat: compute half-angle from celestial body size\n\n* refactor: apply suggestions\n\n* fix: fix celestial diameter computation\n\n* refactor: use if instead of match\n\n* fix: fix celestial direction computation\n\n* docs: fix docstring",
          "timestamp": "2025-05-05T15:38:15+02:00",
          "tree_id": "c94f182b30633480d16e889c2f16cd0447dd127e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e09372248ca9765d5849ec415a8e4ee30440ae07"
        },
        "date": 1746453508187,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 541934814.8999859,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 541886512.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 880849969.7999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 880779980.7000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3197476726.899993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197150855.099999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3194951969.100009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194666240.0999985 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3192782591.8999987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192560301.599998 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "137164378+loft-nicolas-sanchez@users.noreply.github.com",
            "name": "loft-nicolas-sanchez",
            "username": "loft-nicolas-sanchez"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "ee8622834fa29bc555d943446a74d39eda4958a0",
          "message": "feat: add ground track to viewer (#550)\n\n* feat: add ground track to viewer\n\n* fix: remove wrong argument\n\n* test: fix test",
          "timestamp": "2025-05-05T12:56:55-05:00",
          "tree_id": "e29752b548d193b329e79ae0b7daef1adcd5096b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ee8622834fa29bc555d943446a74d39eda4958a0"
        },
        "date": 1746469008828,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 542531903.2000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 542335008.3000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 884667295.4999974,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 884557615.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3204116343.1000032,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3203828365.400001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3204204888.9,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204021568.399998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3204889178.099995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204610868.000002 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "137164378+loft-nicolas-sanchez@users.noreply.github.com",
            "name": "loft-nicolas-sanchez",
            "username": "loft-nicolas-sanchez"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "53b2adde4c9656c31a6632e5504b807fe9c8e593",
          "message": "feat: add moving point and set transparent celestial direction in viewer (#551)\n\n* feat: add moving point in viewer\n\n* refactor: apply suggestions",
          "timestamp": "2025-05-06T11:48:32+02:00",
          "tree_id": "5db3fe361c2c9e1aa5a3cb3e923e9c905ce80151",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/53b2adde4c9656c31a6632e5504b807fe9c8e593"
        },
        "date": 1746526098939,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 545926569.2999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 545812097.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 887197701.7999996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 886820168.9000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3218171490.100002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3217762823.2000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3218151152.8999863,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3217719026.6000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3212591131.2000027,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3212165157.300002 ns\nthreads: 1"
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
          "id": "e98e167e6389c76117d3ab4a7d09e00395894a9b",
          "message": "feat: add support for state filter with fixed targets (#552)\n\n* feat: add support for state filter with fixed targets\n\n* Apply suggestions from code review\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n---------\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2025-05-06T22:06:43Z",
          "tree_id": "d11f009e21bba72931c9a7bfc88f82cef9c77da8",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e98e167e6389c76117d3ab4a7d09e00395894a9b"
        },
        "date": 1746570394945,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 537453844.1999903,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 537352397.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 878268594.4999969,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 877898657.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3203526061.5999956,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3203081102.3999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3188483083.5000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188377509.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3188288697.899992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188177086.9000006 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}