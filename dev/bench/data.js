window.BENCHMARK_DATA = {
  "lastUpdate": 1697235166732,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "kyle.ray.cochran@gmail.com",
            "name": "kyle-cochran",
            "username": "kyle-cochran"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "973343a01931575f39843dbd7aebfdc770370512",
          "message": "feat: add functions to extract multiple subsets at once. (#234)\n\n* feat: add functions to State and CoordinatesBroker to extract multiple subsets at once.\r\n\r\n* style: missed a couple of doc-string newlines\r\n\r\n* feat: add bindings (and fix typo bug\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2023-10-09T16:45:24-07:00",
          "tree_id": "3eb9066bb2e33db74fe48e4c08bb68b3a5c43a56",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/973343a01931575f39843dbd7aebfdc770370512"
        },
        "date": 1696897382398,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 822385429.7999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 822261089.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1857799782.3000035,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1857324519.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4689460322.1,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4687877980 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4621238339.400008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4619962179.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4634613640.899994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4633127270 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "kyle.ray.cochran@gmail.com",
            "name": "kyle-cochran",
            "username": "kyle-cochran"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "9b7810aac14a3d4053c92a04818fdbfa2156ec70",
          "message": "refactor: Use the built-in State method to extract coordinates (#235)\n\n* chore: rebase conflicts\r\n\r\n* feat: add bindings (and fix typo bug\r\n\r\n* refactor: leverage State subsets extraction to simplify\r\n\r\n* chore: remove artifact from rebase",
          "timestamp": "2023-10-10T15:50:49-07:00",
          "tree_id": "e6a286dabee708f95a0725fbfb5d051e11dcdad2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/9b7810aac14a3d4053c92a04818fdbfa2156ec70"
        },
        "date": 1696980310547,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 735488509.0000039,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 735248220.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1651946364.4000096,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1651804490 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4111785805.599993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4110832650.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4114813365.399999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4114536540.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4106929186.799999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4106630000.0000014 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "kyle.ray.cochran@gmail.com",
            "name": "kyle-cochran",
            "username": "kyle-cochran"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "da64bbaf375a2de0017d62e0f1605b3f2df0d30e",
          "message": "feat: add State constructor with Array of Subsets (#238)\n\n* feat: add State constructor with Array of Subsets\r\n\r\n* refactor: adress MR comments",
          "timestamp": "2023-10-11T13:27:37-07:00",
          "tree_id": "8d4f3bb23b8613cc05c28d1cbfd87fdf74da62dc",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/da64bbaf375a2de0017d62e0f1605b3f2df0d30e"
        },
        "date": 1697058219849,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 697642028.100006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 697083469.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1484717112.6999853,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1484578939.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4141488179.60002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4141253600.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4142242701.699996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4142003049.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4142194365.400019,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4141949289.9999976 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "kyle.ray.cochran@gmail.com",
            "name": "kyle-cochran",
            "username": "kyle-cochran"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "b12aadb2857a6e41793754d87073f0fcf016d162",
          "message": "feat: add StateBuilder (#237)\n\n* chore: rebase conflicts\r\n\r\n* feat: add bindings (and fix typo bug\r\n\r\n* chore: remove artifact from rebase\r\n\r\n* feat: add functions to State and CoordinatesBroker to extract multiple subsets at once.\r\n\r\n* feat: add bindings (and fix typo bug\r\n\r\n* feat: initial commit to add StateBuilder hpp/cpp\r\n\r\n* test: cleanup and add test file\r\n\r\n* feat: add and test the rest of the boilerplate except for State generation function\r\n\r\n* style: formatting\r\n\r\n* feat: fully functional cpp code\r\n\r\n* feat: add python bindings\r\n\r\n* fix: several tests that started failing due to the new coordinates size constraint on State construction\r\n\r\n* refactor: rename state method in python to build_state\r\n\r\n* refactor: improve equality operator from MR comments",
          "timestamp": "2023-10-11T14:17:20-07:00",
          "tree_id": "d6a4d1a947778c6fdb3e2b97245949db0d627ccb",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b12aadb2857a6e41793754d87073f0fcf016d162"
        },
        "date": 1697061166661,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 739273186.1000016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 738990160 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1647771255.1000025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1647465730.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4105645095.599993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4104805800.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4050756573.600006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4050076789.999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4040458196.7999887,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4040174150 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "antoine.paletta@loftorbital.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "antoine.paletta@loftorbital.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "distinct": true,
          "id": "9b6e93a6d7ab1bd0b75652a51feb907b7e091391",
          "message": "add pylance ignore warnings to dev container",
          "timestamp": "2023-10-12T17:24:29Z",
          "tree_id": "9920df38ab23ac0ed1ffc75eb7487dfb417d9f6c",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/9b6e93a6d7ab1bd0b75652a51feb907b7e091391"
        },
        "date": 1697133886013,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 743675320.5999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 743571800 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1648219413.0000095,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1647671119.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4159528913.599996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4158387830.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4159048134.099987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4158085900 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4160366323.500023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4159170170 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "e93ad8562022f7fcd7cdf7677be2a622c0b065ca",
          "message": "feat: enrich StateBuilder capabilities (#239)\n\n* parent b12aadb2857a6e41793754d87073f0fcf016d162\r\nauthor Kyle Cochran <kyle.cochran@loftorbital.com> 1696962698 +0000\r\ncommitter Pau Hebrero <pau.hebrero@gmail.com> 1697119993 +0000\r\n\r\nchore: rebase\r\n\r\n* feat: StateBuilder operators, reduction and expansion\r\n\r\n---------\r\n\r\nCo-authored-by: Kyle Cochran <kyle.cochran@loftorbital.com>\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2023-10-12T13:58:18-07:00",
          "tree_id": "d8916c41d5b79bb69b637001eca6e5aacc71d561",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e93ad8562022f7fcd7cdf7677be2a622c0b065ca"
        },
        "date": 1697146387505,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 792161667.0999981,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791825260.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1785379546.099989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1784831100 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4580002477.799996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4579146840 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4628492726.000008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4623984419.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4601207011.299999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4600515240.000003 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "kyle.ray.cochran@gmail.com",
            "name": "kyle-cochran",
            "username": "kyle-cochran"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "2200a55656e853199125c7fa701f345a698dffef",
          "message": "feat: Python class meta-programming for State construction (#244)\n\n* feat: add StateTemplateType emitter in Python to create custom State types\r\n\r\n* test: add copy constructor tests\r\n\r\n* feat: add non-default copy-assignment operator\r\n\r\n* style: formatting\r\n\r\n* Update bindings/python/test/trajectory/test_state.py\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>\r\n\r\n* test: fix old naming in python test and address 1 MR comment\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2023-10-13T13:23:03-07:00",
          "tree_id": "92d7efde5a18d1aa14f7ffdbc349199590f6e1ca",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/2200a55656e853199125c7fa701f345a698dffef"
        },
        "date": 1697230823938,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 1007014359.0000101,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1006624650 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1979210368.6000075,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1978145700.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6332316508.900004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6330253680.000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6274377916.300016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6273570029.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6166116204.1,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6165362310.000003 ns\nthreads: 1"
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
          "id": "81096046a935bf2e19542361516ae3d61d8661a3",
          "message": "feat: add Brouwer-Lyddane Mean Long and Mean Short elements (#240)\n\n* wip:\r\n\r\n* feat: get it working\r\n\r\n* feat: working cut\r\n\r\n* feat: Add BrouwerLyddaneMeanShort + tests\r\n\r\n* feat: add tests for apo/periapsis calculation\r\n\r\n* feat: more tests\r\n\r\n* feat: address feedback\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* feat: start bindings and add docstrings (#242)\r\n\r\n* feat: add python bindings\r\n\r\n* docs: add docstring cpp\r\n\r\n* feat: finish tests, docstrings and bindings\r\n\r\n---------\r\n\r\nCo-authored-by: vishwa shah <vishwa2710@gmail.com>\r\n\r\n* refactor: move some stuff around\r\n\r\n* chore: cleanup\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa@loftorbital.com>\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\nCo-authored-by: Remy Derollez <43561754+Derollez@users.noreply.github.com>",
          "timestamp": "2023-10-13T21:34:21Z",
          "tree_id": "c4c6a0b29d5879ce9769e20d722642ba1ed18d24",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/81096046a935bf2e19542361516ae3d61d8661a3"
        },
        "date": 1697235165988,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 747630532.2000086,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 747379360.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1663713614.1999917,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1661071510.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4130769487.7999945,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4126641240.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4127020338.6000047,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4126721610.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4129969508.4000006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4128631700.000004 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}