window.BENCHMARK_DATA = {
  "lastUpdate": 1706092714927,
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
          "id": "142c4d0e6c3eec3624c6944802e4de8eb226a7f6",
          "message": "refactor: change the index returned by pass map to be the last index (#323)\n\nrefactor: change the index returned by pass map to be the last index",
          "timestamp": "2024-01-19T16:15:42-08:00",
          "tree_id": "c157fcd7a71ac482f717ceca0b1fc5012d0260a1",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/142c4d0e6c3eec3624c6944802e4de8eb226a7f6"
        },
        "date": 1705711441494,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 558758607.8999947,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 558642940 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796973971.0000112,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796920029.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2793163421.7000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2793042660.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2794685650.5000086,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794381589.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2791608839.6000133,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791480629.999998 ns\nthreads: 1"
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
          "id": "6be5cfa1ec280a0a8dd03e3824531c96a58702bb",
          "message": "chore: add typing, update inputs for utilities (#324)\n\n* chore: add typing, update inputs\r\n\r\n* feat: address feedback\r\n\r\n* chore: fix typo\r\n\r\n* chore: fix tetsts\r\n\r\n* chore: fix tetsts",
          "timestamp": "2024-01-22T14:02:20-08:00",
          "tree_id": "6288bac1a95e9197d0001778d1db5da8bd369f87",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6be5cfa1ec280a0a8dd03e3824531c96a58702bb"
        },
        "date": 1705962639553,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 563806458.6999917,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 563751219.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 797107061.7999942,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 797041179.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2817656485.7999835,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2817435260 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2818801435.199992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2818607950.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2817201149.999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2816998439.9999976 ns\nthreads: 1"
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
          "id": "a3f7555eb3a558eb1adcb4e7b91abab3ffc097eb",
          "message": "feat: return 0.0 when outside model bounds in TabulatedDynamics::computeContribution (#325)\n\n* feat: return 0.0 when outside model bounds in TabulatedDynamics::computeContribution\r\n\r\n* feat: fix tests\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\r\n\r\n* chore: update function arguments\r\n\r\n---------\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2024-01-23T18:44:14Z",
          "tree_id": "4540293132d3abe68debee1ccfb6e65c10525916",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/a3f7555eb3a558eb1adcb4e7b91abab3ffc097eb"
        },
        "date": 1706037177657,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 557406107.2000062,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 557385070 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794877393.7999988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794849870.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2793493343.3000026,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2793418719.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2792240094.399989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792143710 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2792122294.4999895,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792038690.000001 ns\nthreads: 1"
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
          "id": "eb183ca2330cadb7f40808cf0311e3602c293976",
          "message": "fix: update pass computation epoch to improve accuracy (#326)\n\n* fix: update pass computation epoch to improve accuracy\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* Update Orbit.hpp\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2024-01-23T13:29:14-08:00",
          "tree_id": "c37aeeb9c3f3cc891a64f0d37934b18b53cb4fb2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/eb183ca2330cadb7f40808cf0311e3602c293976"
        },
        "date": 1706047060899,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 563423164.2999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 563398839.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 795407040.6999961,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 795369370.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2788091654.9000063,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787738320 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2788805995.0000067,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788645379.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2784686751.1999903,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784596690.0000024 ns\nthreads: 1"
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
          "id": "4ddbbd201310f9798c46a0044910b8af2df8ee94",
          "message": "feat: add validation app and parser (#298)\n\n* feat: finish first full draft\r\n\r\n* chore: fixup after rebase and comments\r\n\r\n* feat: address comments from vishwa and move validation tests to its own folder\r\n\r\n* feat: add all unit tests\r\n\r\n* style: address remy's comments",
          "timestamp": "2024-01-24T10:10:28Z",
          "tree_id": "18d3f7448669c4913c6a5ac53a8f0f036d12077e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/4ddbbd201310f9798c46a0044910b8af2df8ee94"
        },
        "date": 1706092713621,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 555508168.6999984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 555497610 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792094670.1999924,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792076739.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2793685842.699992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2793633540.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2792010062.6999955,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791915610.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2791280337.900008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791106749.999997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}