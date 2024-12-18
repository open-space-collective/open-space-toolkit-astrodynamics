window.BENCHMARK_DATA = {
  "lastUpdate": 1734518766922,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
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
          "id": "827f75f77688336e62bbc540216388797cd01e02",
          "message": "fix: startup script for jupyter (#471)",
          "timestamp": "2024-12-08T19:04:43-07:00",
          "tree_id": "0eedb04296dcfccf31bfc6f28cffa4034c5f4421",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/827f75f77688336e62bbc540216388797cd01e02"
        },
        "date": 1733710993304,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 589627758.3000085,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 589592527.6000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 884427258.1999916,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 884369169.7000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3188399030.800008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188162319.5999994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3188446686.599997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188298265.7000017 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3196785188.400008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196688772.7999988 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "35860220+alex-liang3@users.noreply.github.com",
            "name": "Alex Liang",
            "username": "alex-liang3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "6956a4e18fabf2a52c0b3aafc7efc7dbc5ef3eb2",
          "message": "feat: frozen orbit conditions (#473)\n\n* feat: add method to get coe for frozen orbit\r\n\r\n* feat: add frozen Orbit constructor\r\n\r\n* test: add coe gtest\r\n\r\n* chore: improve error handling\r\n\r\n* refactor: make re, j2, and j3 args\r\n\r\n* test: add tests for orbit\r\n\r\n* chore: bump ostk-physics dependency version\r\n\r\n* test: add python tests\r\n\r\n* perf: loosen criticality requirement\r\n\r\n* style: use camelCase\r\n\r\n* style: add units and long for to var names\r\n\r\n* style: make lambda captures explicit\r\n\r\n* perf: use ostk Arrays\r\n\r\n* test: add more tests\r\n\r\n* doc: add more documentation\r\n\r\n* test: improve tests\r\n\r\n* doc: add docs for Orbit constructor\r\n\r\n* feat: add overloaded ctor for COE\r\n\r\n* test: add test for ctor with Celestial\r\n\r\n* test: add python test\r\n\r\n* perf: use reference\r\n\r\n* chore: fix typos\r\n\r\n* doc: add doc to orbit.hpp\r\n\r\n* test: add negative tests for celestial ctor",
          "timestamp": "2024-12-17T13:50:49-03:00",
          "tree_id": "b9713c32b18cb97e6bdc8dc001a4df72f7762185",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6956a4e18fabf2a52c0b3aafc7efc7dbc5ef3eb2"
        },
        "date": 1734455480354,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 584720371.7999946,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 584694399.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 881762585.0999889,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 881722342.2000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3175390347.0000067,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3175295313.600001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3174495148.300002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3174368380.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3175764644.300011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3175667149.8999968 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "35860220+alex-liang3@users.noreply.github.com",
            "name": "Alex Liang",
            "username": "alex-liang3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "84816150be89cca9f52929306466e147990db9b3",
          "message": "docs: clarify altitude definition in Orbit constructors (#475)\n\ndoc: clarify altitude definition in Orbit ctors",
          "timestamp": "2024-12-18T11:28:38+01:00",
          "tree_id": "db8e00e55ad6fe506767b9457b42a5dd467304c2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/84816150be89cca9f52929306466e147990db9b3"
        },
        "date": 1734518764637,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 583768593.700006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 583730880.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 888723954.0999985,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 888686518.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3184520978.800026,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3184384983.400001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3181799061.9000173,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3181680421.199998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3186331568.2999883,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186055682.699998 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}