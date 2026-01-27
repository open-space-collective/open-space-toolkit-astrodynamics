window.BENCHMARK_DATA = {
  "lastUpdate": 1769531537375,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "65550121+phc1990@users.noreply.github.com",
            "name": "Pau Hebrero",
            "username": "phc1990"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "a496ee65c433390c4e6e8454c1f9953b7afd4936",
          "message": "test: add a Constant LOF Maneuver regression test (#641)\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-01-23T11:10:06-08:00",
          "tree_id": "2d454c21b1dd5322acc4a254b59c436d369b287e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/a496ee65c433390c4e6e8454c1f9953b7afd4936"
        },
        "date": 1769197540731,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4757039221.500013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4658841606.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4070743351.2999954,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4070453755.1000023 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7182498841.500012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7181808089.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7182605022.29997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7182087066.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7172251491.00001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7171779953.899997 ns\nthreads: 1"
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
          "id": "77b16b39b02314068b03eb999ce5573564b1ca03",
          "message": "fix: segments logic solve for precise maneuver start and provide strictly monotonic states (#643)\n\n* fix: segments should solve for precise maneuver start\n\n* refactor: clean up\n\n* chore: fix tests\n\n* feat: add bindings and fix python tests\n\n* refactor: apply PR suggestions\n\n* test: apply Github suggestion\n\n* test: added regression test on unsorted or duplicate arrays\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-01-27T16:04:52Z",
          "tree_id": "473f78434d80aa886abd9f9d20761d49d44f25ed",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/77b16b39b02314068b03eb999ce5573564b1ca03"
        },
        "date": 1769531535711,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4748704958.399992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4688595664.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4089964942.8999963,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4089436597.3000007 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7192860403.200002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7191897329.399999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7190845173.099996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7189927756.399992 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7199964228.200007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7198870222.000005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}