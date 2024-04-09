window.BENCHMARK_DATA = {
  "lastUpdate": 1712666169446,
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
          "id": "f42bf9d22b1612d02dae7b340facdad4d23e9569",
          "message": "fix: variable naming error (#383)",
          "timestamp": "2024-04-05T15:25:12+02:00",
          "tree_id": "21b81f11b15ac2dd42161ec13410facff39a2eb6",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f42bf9d22b1612d02dae7b340facdad4d23e9569"
        },
        "date": 1712324505434,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 543407408.0999892,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 543375436.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 790716240.9999955,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 790687734.7000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2780535312.4000077,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2780392977.7000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2781236262.000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781013166.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2780675557.7000103,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2780544881.8000016 ns\nthreads: 1"
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
          "id": "5ed7804ac3a632d6d46a0b74246ad54ba83709be",
          "message": "fix: converters coerce_to_iso typing (#385)\n\nCo-authored-by: Pau Hebrero <pau@Paus-MacBook-Pro.local>",
          "timestamp": "2024-04-05T18:44:48+02:00",
          "tree_id": "1f800a3e9515650e9e0ac876e30ccd363f8821db",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/5ed7804ac3a632d6d46a0b74246ad54ba83709be"
        },
        "date": 1712336472813,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 549181927.8999856,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 549136921.5000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794544392.6999986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794477471.2999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2787698509.400002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787554856.600001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2797559223.199994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2797360514.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2787328714.299997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787191387 ns\nthreads: 1"
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
          "id": "fd49513a9fb299687152976e3ae59414297add5b",
          "message": "refactor!: remove local orbital frame factory parent frame getter and fix accessor (#384)\n\n* fix: local orbital frame factory paren frame accessor\r\n\r\n* refactor: remove getter for parent frame\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau@Paus-MacBook-Pro.local>\r\nCo-authored-by: Antoine Paletta <antoine.paletta@loftorbital.com>",
          "timestamp": "2024-04-05T19:30:30+02:00",
          "tree_id": "1616628365954af60673b2d58226680964ff5093",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/fd49513a9fb299687152976e3ae59414297add5b"
        },
        "date": 1712339312610,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 544326505.1999958,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 544310116.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 789773146.600004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 789748369.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2789297784.400003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789185751.800002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2787694321.2000074,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787503540.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2787428592.200007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787391908.4000025 ns\nthreads: 1"
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
          "id": "f576105d2c6e1e85452d2c2d2d1840fc59233820",
          "message": "fix: coordinate subset unsafe frame conversion (#387)",
          "timestamp": "2024-04-08T17:52:09+02:00",
          "tree_id": "e47b5d7f50f525fabac28abc676d0eea81b7edc9",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f576105d2c6e1e85452d2c2d2d1840fc59233820"
        },
        "date": 1712592929728,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 551762355.9000185,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 551719771.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791385985.199986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791333271.6999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2784557501.2999916,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784380567.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2780835092.999996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2780598121.7000012 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2782642142.099985,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782458884.799999 ns\nthreads: 1"
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
          "id": "582ed2bd28232191028d2fb939229cae2915de8a",
          "message": "feat: add maneuver cross and self validation (#373)",
          "timestamp": "2024-04-09T14:20:06+02:00",
          "tree_id": "980afb2b5577b3e38a11c8d1d096c397409657c4",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/582ed2bd28232191028d2fb939229cae2915de8a"
        },
        "date": 1712666167638,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 551786664.0000023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 551742301.5000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 797259726.2000022,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 797203907.4999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2780395950.2999988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2780210971.399999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2780143931.100008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2780020766.9000015 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2778906020.1999917,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2778772797.800005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}