window.BENCHMARK_DATA = {
  "lastUpdate": 1746469011364,
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
      }
    ]
  }
}