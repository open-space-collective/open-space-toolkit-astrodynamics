window.BENCHMARK_DATA = {
  "lastUpdate": 1693942454943,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "43561754+Derollez@users.noreply.github.com",
            "name": "Remy Derollez",
            "username": "Derollez"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "0e087f60d00d4b5d48f5ef96554d2ad9b634cccc",
          "message": "docs: update notebook for orbit propagation using propagator (#216)\n\nfeat: update notebook for propagation using numerical solver",
          "timestamp": "2023-08-31T14:50:08+02:00",
          "tree_id": "079e5a5edfa968a806555d64905a8bfc917cc397",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/0e087f60d00d4b5d48f5ef96554d2ad9b634cccc"
        },
        "date": 1693488349592,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 958234315.8000128,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 958117510 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1907768617.799991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1907491260.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4978000660.900011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4976429120 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4986565567.7999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4985432070 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5026995201.799992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5025911360.000004 ns\nthreads: 1"
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
          "id": "b609cb3b69da519b906c0795824ebc655033015e",
          "message": "feat!: add State context NumericalSolver, remove standard NumericalSolver (#214)\n\n* feat: first cut\r\n\r\n* feat: fix bugs and use NumericalSolver from math\r\n\r\n* feat: add bindings\r\n\r\n* feat: fix python test imports\r\n\r\n* feat: clean up a bit\r\n\r\n* feat: fix a test\r\n\r\n* feat: fix a test for reals\r\n\r\n* feat: add missing tests",
          "timestamp": "2023-08-31T16:08:36-07:00",
          "tree_id": "5d15d9cd9ef59c7eae144ec881ca545520ce5de3",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b609cb3b69da519b906c0795824ebc655033015e"
        },
        "date": 1693525406255,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 902211255.0999963,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 902013680 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1743437553.700005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1743232489.9999993 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4542259044.1,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4541700000 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4520712641.200009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4520119029.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4522767036.099993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4521895310.000002 ns\nthreads: 1"
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
          "id": "8b45943bca57d7667c7171f4e581673f335e6255",
          "message": "refactor: clean-up event condition tree (#217)\n\n* refactor: clean-up event condition tree\r\n\r\n* feat: rebase, add bindings, tests, python tests\r\n\r\n* test: tidy up testing\r\n\r\n* chore: rename criteria to criterion\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>\r\nCo-authored-by: vishwa shah <vishwa2710@gmail.com>",
          "timestamp": "2023-09-01T08:20:30-07:00",
          "tree_id": "989c2552b2b9727a99dd661d000f89cbdded19de",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/8b45943bca57d7667c7171f4e581673f335e6255"
        },
        "date": 1693583628238,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 724430277.8000019,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 724356990 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1591405016.3000014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1591195579.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4155218663.7000093,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4154777210 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4154098526.7,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4153808369.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4153234966.599996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4152175950 ns\nthreads: 1"
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
          "id": "dec31d6b0056d88609a9d80a887389faacff8fb9",
          "message": "feat: make EventCondition state aware (#218)\n\n* refactor: clean-up event condition tree\r\n\r\n* feat: finishing tests, add bindings\r\n\r\n* test: fix python tests\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>\r\nCo-authored-by: vishwa shah <vishwa2710@gmail.com>",
          "timestamp": "2023-09-05T16:19:26Z",
          "tree_id": "fc2367d28f3d94b62346b0ca3a7d304515f10d54",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/dec31d6b0056d88609a9d80a887389faacff8fb9"
        },
        "date": 1693932693686,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 674653524.5000075,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 674084369.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1431606025.0999897,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1431501480 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4132936613.5999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4132440830 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4133553011.2999945,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4133324709.9999976 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4133106191.5000024,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4132924420.000003 ns\nthreads: 1"
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
          "id": "38062e55211599a6b485857db3ad62a8468bb6b4",
          "message": "chore: bump OSTk Physics to 2.1.0 (#219)",
          "timestamp": "2023-09-05T18:58:54Z",
          "tree_id": "228a1fe15ac4c436ed98dd99c28095d762de7626",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/38062e55211599a6b485857db3ad62a8468bb6b4"
        },
        "date": 1693942453539,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 798280576.299993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 798129629.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1800553868.099996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1800302909.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4682616677.600004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4681942950 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4670879440.900001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4670299319.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4659257750.800009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4658189349.999994 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}