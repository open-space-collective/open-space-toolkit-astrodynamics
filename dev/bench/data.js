window.BENCHMARK_DATA = {
  "lastUpdate": 1693525407656,
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
      }
    ]
  }
}