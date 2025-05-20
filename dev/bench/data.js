window.BENCHMARK_DATA = {
  "lastUpdate": 1747715194338,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "35860220+alex-liang3@users.noreply.github.com",
            "name": "Alex Liang",
            "username": "alex-liang3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e6f1964e0c5f2054fb28540a0eff76321d5c2b9a",
          "message": "docs: fix statebuilder docs (#556)\n\n* docs: fix statebuilder docs\n\n* docs: clarify docs further\n\n* fix: tests\n\n---------\n\nCo-authored-by: vishwa shah <vishwa2710@gmail.com>",
          "timestamp": "2025-05-16T00:00:59Z",
          "tree_id": "e1655b53ee4a1c18f243e74dcd9cecf0de4cf1fc",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e6f1964e0c5f2054fb28540a0eff76321d5c2b9a"
        },
        "date": 1747354842039,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 540393123.5000017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 540167640.5000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 879336137.80001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 879217173.3000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3188835050.39999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188601124.6000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3188067901.1999974,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3187807581.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3187069556.3999844,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186902494.6000013 ns\nthreads: 1"
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
          "id": "6475d2d43f1f2cf076b2a64e2e57627394fb1b7c",
          "message": "feat: nadir trajectory model (#554)\n\n* feat: add nadir trajectory model and static constructors\n\n* feat: add bindings and tests\n\n* feat: address feedback\n\n* chore: style\n\n* chore: style\n\n* fix: tests\n\n* chore: style\n\n* feat: address remaining feedback\n\n* feat: remove extraneous trajectory constructors\n\n* chore: style\n\n* feat: address Alex's feedback",
          "timestamp": "2025-05-19T16:26:23-05:00",
          "tree_id": "00c1bce6a8c27d9388132130e72264000e6e1e67",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6475d2d43f1f2cf076b2a64e2e57627394fb1b7c"
        },
        "date": 1747691215383,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 544817875.3999969,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 544768797.3999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 881471707.3000225,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 881172649.4999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3192489528.8000245,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192259369.1000013 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3193551976.8000174,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193285056.8999987 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3190226221.600005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189978889.6999993 ns\nthreads: 1"
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
          "id": "a429cc199ebc5dd67fd3bfbc69ea2ba67099ee65",
          "message": "feat: improve tabulated profile model (#558)\n\n* feat: profile use trajectory model\n\n* feat: improve tabulated profile model\n\n* fix: bugs\n\n* feat: address remaining feedback\n\n* feat: add more tests\n\n* feat: add missing tests\n\n* fix: test",
          "timestamp": "2025-05-19T23:06:47-05:00",
          "tree_id": "c36aa93b2acdaf917b3d99abd94ebe38f0b0b1da",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/a429cc199ebc5dd67fd3bfbc69ea2ba67099ee65"
        },
        "date": 1747715192820,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 537657789.7000118,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 537432066.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 878494700.0000045,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 878234662.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3189361346.2000074,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189153021.300001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3191594452.6000032,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3191382732.599999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3189435124.700009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189202457.9000025 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}