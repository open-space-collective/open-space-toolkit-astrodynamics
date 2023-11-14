window.BENCHMARK_DATA = {
  "lastUpdate": 1699930574879,
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
          "id": "f477d29706fd665b20da39b5dd96698149058d4e",
          "message": "feat: add periapsis scaling for QLaw and fix small bugs in oe_xx calculations (#270)\n\n* feat: add periapsis scaling for QLaw and fix some small bugs\r\n\r\n* feat: works well end to end\r\n\r\n* feat: fix style\r\n\r\n* feat: fix tests\r\n\r\n* feat: add more tests\r\n\r\n* chore: remove stdcout\r\n\r\n* feat: use Eigen map",
          "timestamp": "2023-11-13T18:21:35-08:00",
          "tree_id": "5464c02f3c5237746a62dbc3ac1e9aef6be2ce3e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f477d29706fd665b20da39b5dd96698149058d4e"
        },
        "date": 1699930573914,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 573823098.0999959,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 573769209.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 907453205.600001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 907386580 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4109037569.3,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4108166020.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4108809345.6999955,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4108231720.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4109015670.1000185,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4108319360.000003 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}