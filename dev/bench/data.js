window.BENCHMARK_DATA = {
  "lastUpdate": 1710154073028,
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
          "id": "956bb7b4145517238720c7414ff1efd9aa8f7217",
          "message": "docs: add release badge (#363)",
          "timestamp": "2024-02-21T18:10:14Z",
          "tree_id": "dc94435a4787adac5641f4df13feb9d1c1d2b60e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/956bb7b4145517238720c7414ff1efd9aa8f7217"
        },
        "date": 1708539969586,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 543975561.3999921,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 543959930 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 800654686.0000014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 800500569.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2797121159.800008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2797031900 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2796374510.299989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2796281160.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2794647824.099991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794294109.9999986 ns\nthreads: 1"
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
          "id": "94eeb094ac6420abf78d0c717983ae77c2b750c3",
          "message": "refactor: add dynamics validation to propagator (#364)\n\n* refactor: add dynamics validate to propagator\r\n\r\n* fix: remove tabulateddynamics and thruster dynamics incompatibility throw statement\r\n\r\n* fix: address pau's feedback\r\n\r\n* fix: validate dynamics set to use map.find",
          "timestamp": "2024-02-22T22:16:39+01:00",
          "tree_id": "f27cd2533aae9ccd93fac941d911e774eb4586d3",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/94eeb094ac6420abf78d0c717983ae77c2b750c3"
        },
        "date": 1708637527157,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 549297620.7999988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 549288170.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792126816.5999948,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792105799.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2782317377.5000157,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782145629.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2779414077.7000055,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2779309759.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2785052541.500005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784873150 ns\nthreads: 1"
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
          "id": "85bb6e143ae6e437417a432dc0b4d986a931cea0",
          "message": "feat: connect maneuver class with segments and propagator (#340)\n\n* feat: connect maneuver class with segments and propagator\r\n\r\n* feat: add bindings and python tests\r\n\r\n* fix: add pau's suggestion",
          "timestamp": "2024-03-11T11:25:47+01:00",
          "tree_id": "bdb5d201c0994e8890ce5672493815736fb57935",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/85bb6e143ae6e437417a432dc0b4d986a931cea0"
        },
        "date": 1710154070958,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 557298327.9000084,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 557259264.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 801478976.8000014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 801420015.8000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2788954383.100003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788842471.4000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2788529841.000002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788487822.6000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2784247926.499984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784174097.899999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}