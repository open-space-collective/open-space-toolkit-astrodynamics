window.BENCHMARK_DATA = {
  "lastUpdate": 1704933096704,
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
          "id": "7032be1ff60efdb0f8af047db4c81b95774e10a8",
          "message": "feat: Tabulated dynamics for custom contribution profiles (#317)\n\n* feat: Tabulated dynamics for custom contribution profiles\r\n\r\n* feat: add print and ostream\r\n\r\n* test: add simple python tests\r\n\r\n* chore: args type in python fixed\r\n\r\n* feat: add access and get instants methods\r\n\r\n* fix: attempt to fix eigen in the CI\r\n\r\n* fix: use full path on load data files in test cpp\r\n\r\n* feat: fix test\r\n\r\n* feat: remove extra test\r\n\r\n* feat: address remaining feedback\r\n\r\n* feat: add frame getters\r\n\r\n---------\r\n\r\nCo-authored-by: Remy Derollez <remy@loftorbital.com>",
          "timestamp": "2024-01-11T00:03:11Z",
          "tree_id": "81760736363e732c8268b5fba1ba1d2ac573793a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/7032be1ff60efdb0f8af047db4c81b95774e10a8"
        },
        "date": 1704933095789,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 558842283.9999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 558810700 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794188609.6000075,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794138320.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2790858989.999998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2790651050 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789173161.8999987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789018239.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2788168324.6999955,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788055499.999999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}