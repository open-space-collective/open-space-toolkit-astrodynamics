window.BENCHMARK_DATA = {
  "lastUpdate": 1730906448031,
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
          "id": "75f26892d663bdebe5ea9ccf192190fdab1df886",
          "message": "feat: add more fields for pystate.from_dict (#453)",
          "timestamp": "2024-10-31T07:51:06-07:00",
          "tree_id": "502355188ea5e6ff0499d6c7b16e9093266affe5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/75f26892d663bdebe5ea9ccf192190fdab1df886"
        },
        "date": 1730387371419,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 592191814.4000018,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 592165365.8000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 880499410.4999993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 880427407.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3175123512.2000027,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3174789518.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3176185892.500001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3176054772.5000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3179416397.500006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3179310188.6000013 ns\nthreads: 1"
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
          "id": "503eae8c1d7f9c36cbc6f76a5cd748df4e5806d7",
          "message": "feat: add getInterval methods to the Solution classes for Sequence/Solution classes (#455)\n\n* feat: add getInterval methods to the Solution classes for Sequence/Segment (and tests)\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\r\n\r\n* feat: address feedback\r\n\r\n---------\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2024-10-31T21:14:07+01:00",
          "tree_id": "a82de9a34be41dd23e427a369d40fc9d957fb0d8",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/503eae8c1d7f9c36cbc6f76a5cd748df4e5806d7"
        },
        "date": 1730406727063,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 583570678.6000116,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 583514932.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 879472784.5000012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 879384019.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3172115641.900001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3171861723.7000017 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3172221949.499999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3172039976.3999977 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3172838880.6,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3172645599.8999996 ns\nthreads: 1"
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
          "id": "32769f6d9c56b213ee69a70aa5f13b7740c293a1",
          "message": "fix: sequence relative event condition (#456)\n\n* feat: add clone methods to EventCondition classes\r\n\r\n* fix: sequence relative event condition update\r\n\r\n* patch: things",
          "timestamp": "2024-10-31T16:39:37-07:00",
          "tree_id": "9128eab26856fec71e895bc499495de9eb14a979",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/32769f6d9c56b213ee69a70aa5f13b7740c293a1"
        },
        "date": 1730419066224,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 586715301.2000017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 586700320.5000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 877690561.0000029,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 877640109.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3180599967.1999984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3180404476.100002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3180117406.800008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3180019247.9000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3179987387.500006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3179833940.0999966 ns\nthreads: 1"
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
          "id": "7f4d98847b44e5b96fecfda552aecbba2c3266b9",
          "message": "feat: add profile access_model, repr and missing bindings (#458)\n\n* feat: add profile access_model, repr and missing bindings\r\n\r\n* feat: add test file for Transform\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: coderabbitai[bot] <136622811+coderabbitai[bot]@users.noreply.github.com>\r\n\r\n* Update bindings/python/test/flight/test_profile.py\r\n\r\n* Update include/OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp\r\n\r\n* fix: tests\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\r\n\r\n---------\r\n\r\nCo-authored-by: coderabbitai[bot] <136622811+coderabbitai[bot]@users.noreply.github.com>\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2024-11-04T19:10:07-08:00",
          "tree_id": "b62668bf62932c5bec8c12823e979d4affa8821e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/7f4d98847b44e5b96fecfda552aecbba2c3266b9"
        },
        "date": 1730777278660,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 580351997.0999901,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 580323044.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 874995995.3999904,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 874954599.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3197622537.7999926,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197524957.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3190376207.0000086,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3190252746.900002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3199489164.900007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199361588.099997 ns\nthreads: 1"
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
          "id": "750e3dba4297a9cf1240de1ef81c57a0976c8ef8",
          "message": "docs: add ostk.astrodynamics.dataframe to docs (#459)",
          "timestamp": "2024-11-06T07:02:14-08:00",
          "tree_id": "2a39412a0216fcb56c554521ced494987035595a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/750e3dba4297a9cf1240de1ef81c57a0976c8ef8"
        },
        "date": 1730906445557,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 578446035.5999954,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 578411532.8000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 883947600.3000015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 883891659.5999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3176057015.3999934,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3175855459.200001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3172767458.79999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3172584394.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3171369816.1999903,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3171241071.7 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}