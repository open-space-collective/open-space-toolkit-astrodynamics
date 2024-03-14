window.BENCHMARK_DATA = {
  "lastUpdate": 1710437683925,
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
          "id": "8e3ac289afe7fb36e93c64240d2b68d9761d4566",
          "message": "refactor!:small changes to maneuver class based on vishwa's suggestions (#366)\n\n* refactor!:small changes to maneuver class based on vishwa's suggestions\r\n\r\n* test: add duplicate instant array test",
          "timestamp": "2024-03-12T14:34:05+01:00",
          "tree_id": "f137fe86250cee7365f352aff01068002a5bbad3",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/8e3ac289afe7fb36e93c64240d2b68d9761d4566"
        },
        "date": 1710251388345,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 559161655.9000045,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 559136395.8000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 800409743.5000062,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 800373955.6999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2792552692.400011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792441720.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2795197192.3000145,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2795021643.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2789444718.999988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789332111.999998 ns\nthreads: 1"
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
          "id": "572b590d3d93efb7cfd9ab122bba212c8d780ebe",
          "message": "fix: maneuver creation error message (#368)",
          "timestamp": "2024-03-14T17:16:59Z",
          "tree_id": "77fa4b8e3f9e79ca7a516ae1ff2d591b4190b936",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/572b590d3d93efb7cfd9ab122bba212c8d780ebe"
        },
        "date": 1710437681957,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 555144112.7999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 555107161.3999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796838347.0999954,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796793637.3999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2779985741.59999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2779886941.4000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2781814800.99999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781689598.0999956 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2787370005.600002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787261049.1000004 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}