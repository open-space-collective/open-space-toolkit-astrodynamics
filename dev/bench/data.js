window.BENCHMARK_DATA = {
  "lastUpdate": 1697061167837,
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
      }
    ]
  }
}