window.BENCHMARK_DATA = {
  "lastUpdate": 1712336475035,
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
      }
    ]
  }
}