window.BENCHMARK_DATA = {
  "lastUpdate": 1704402812725,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "98616558+apaletta3@users.noreply.github.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e25d7a63afb8d6ff24356b93cbf8ee3f6e8ee48c",
          "message": "fix: update Propulsion System scalar constructor (#307)\n\n* fix: update Propulsion System scalar constructor \r\n\r\nIt seems there was an error before, and anyways, I think all three mem vars `thrust_`, `specificImpulse_`, and `massFlowRate_` should remain undefined (as defaulted in the .hpp) if the input scalars are not defined.\r\n\r\n* docs: add cpp docstring to SatelliteSystemBuilder\r\n\r\n* docs: add python docstring\r\n\r\n* docs: update according to Pau's feedback",
          "timestamp": "2024-01-03T11:03:54+11:00",
          "tree_id": "db699874c0a47e6e64346532b1a461a9beabd1ff",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e25d7a63afb8d6ff24356b93cbf8ee3f6e8ee48c"
        },
        "date": 1704241956201,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 555013844.5999949,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 554977440 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791728333.1000022,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791701390.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4672724424.500001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4672573339.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4674263091.300003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4674144589.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4679122302.100012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4678866240.000001 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "359caef7865539fe2cbafa3abf266b490606475a",
          "message": "fix: satellite system python docs (#310)\n\nchore: satellite system python docs fix",
          "timestamp": "2024-01-03T09:48:15Z",
          "tree_id": "3f5447447cab47608b5e15e99317bc45ae3c9ee4",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/359caef7865539fe2cbafa3abf266b490606475a"
        },
        "date": 1704276957820,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 554436194.0000101,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 554421070 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791974243.6000035,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791957959.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2792416194.2999887,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792331030.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789392542.6999884,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789319199.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2789630181.3999853,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789576149.999999 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "98616558+apaletta3@users.noreply.github.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "376e4da24941bb426b1322f1896407c1e80f2ffd",
          "message": "fix: py unit test packaging and enhance dev container formatting (#296)\n\n* chore: add pyproject file in bindings directory\r\n\r\n* fix: remove lines that exclude unit tests from setup.cfg.in\r\n\r\n* fix: change setup.cfg packages setting from find: to find_namespace:\r\n\r\n* fix: make test-unit-python command\r\n\r\n* refactor: remove find_namespace from setup.cfg",
          "timestamp": "2024-01-03T13:58:21+01:00",
          "tree_id": "6651505857ab3da2aa506961d822940f0898e2b3",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/376e4da24941bb426b1322f1896407c1e80f2ffd"
        },
        "date": 1704288411848,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 555687755.6000131,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 555420859.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 793371779.5000007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 793336149.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2786747598.1999917,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786644689.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2790643601.499994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2790362540.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2784675636.5000143,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784574330.000001 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "98616558+apaletta3@users.noreply.github.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "ef5c48643ed69aaaab9a93749668a0f1414835ea",
          "message": "docs: align sequence api docs with function definition (#311)",
          "timestamp": "2024-01-03T16:04:06+01:00",
          "tree_id": "605618b82cfb5791edf552bf586ac8309968b4e0",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ef5c48643ed69aaaab9a93749668a0f1414835ea"
        },
        "date": 1704295923368,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 550052204.4999912,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 550027029.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 793176627.0000026,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 793137909.9999994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2792031427.5000067,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791903670.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2787992248.6,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787862190.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2788998041.0000024,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788917470.000001 ns\nthreads: 1"
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
          "id": "4daa7f949e20af5449552d0bb941502f24499d67",
          "message": "refactor: unify profile and trajectory States (#305)\n\n* feat: add Attitude Quaternion and Angular Velocity Coordinate Subsets\r\n\r\n* feat: add State pos/vel/attitude constructor\r\n\r\n* tests: add AttitudeQuaternion and AngularVelocity test\r\n\r\n* test: add attitude and angular velocity tests to State\r\n\r\n* fix: angular velocity subset size\r\n\r\n* feat: add Attitude Quaternion and Angular Velocity Coordinate Subsets\r\n\r\n* tests: add AttitudeQuaternion and AngularVelocity test\r\n\r\n* chore: apply suggestions\r\n\r\n* build: add trajectory state attitude-related bindings\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2024-01-04T17:08:20+01:00",
          "tree_id": "b0076e764cbef41e1737d3ad4cad869c19c93fe3",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/4daa7f949e20af5449552d0bb941502f24499d67"
        },
        "date": 1704386193721,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 562413444.6000028,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 562400449.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 797908484.1999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 797865089.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2789112848.700006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788950740.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2785000989.09999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784663600.000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2784339099.9000063,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784245500 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "98616558+apaletta3@users.noreply.github.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "bf8bd6d678eca988ab2644151e5b6bbba0316597",
          "message": "fix: non-root dev container ostk data issue (#312)\n\nfix: non-root dev container issues",
          "timestamp": "2024-01-04T21:45:32+01:00",
          "tree_id": "93b6b04e22300497cfde28aa8b21eb83d1b39b8f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/bf8bd6d678eca988ab2644151e5b6bbba0316597"
        },
        "date": 1704402811864,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 553146214.9999869,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 553108160 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796512621.599993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796431990 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2787885785.100002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787772960.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2786444432.5000024,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786331089.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2783621738.699998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783464380.000001 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}