window.BENCHMARK_DATA = {
  "lastUpdate": 1702582827792,
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
          "id": "2675b54ffa75537e4ead881a9bf06a613bb441ae",
          "message": "fix: Satellite System Builder test import (#302)\n\n* fix: Satellite System Builder test import\r\n\r\n* fix: Satellite System Builder bindings tests\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2023-12-14T20:12:50+01:00",
          "tree_id": "ac37c8c630e074113e7cc0dc6c580d20a5daab94",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/2675b54ffa75537e4ead881a9bf06a613bb441ae"
        },
        "date": 1702582826896,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 549825633.599994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 549793190 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792504831.6000016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792461530.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2815131032.399995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2814982020.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2812866139.200014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2812733500.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2809423762.699987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2809350869.999999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}