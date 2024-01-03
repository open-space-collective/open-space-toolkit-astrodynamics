window.BENCHMARK_DATA = {
  "lastUpdate": 1704276958961,
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
      }
    ]
  }
}