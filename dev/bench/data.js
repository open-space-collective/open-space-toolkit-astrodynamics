window.BENCHMARK_DATA = {
  "lastUpdate": 1701274733783,
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
          "id": "809be19392c2cfdc81d2c3e761cf5a859854732d",
          "message": "feat: use analytical derivatives for QLaw (#272)\n\n* feat: use analytical derivatives for QLaw\r\n\r\n* feat: add enum for gradient computation\r\n\r\n* feat: fix python bindings and tests\r\n\r\n* feat: fix remaining few nans\r\n\r\n* feat: cleanup from rebase",
          "timestamp": "2023-11-28T16:19:48-08:00",
          "tree_id": "7db09c23160052d85105fcc4a10d23366311bca8",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/809be19392c2cfdc81d2c3e761cf5a859854732d"
        },
        "date": 1701218850702,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 561516357.7999965,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 561484340.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792775215.9000022,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792730609.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2783440610.7999997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783322749.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2781593233.4999957,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781468249.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2782091291.0000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781966949.999999 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "pau@Paus-MacBook-Pro.local",
            "name": "Pau Hebrero"
          },
          "committer": {
            "email": "pau@Paus-MacBook-Pro.local",
            "name": "Pau Hebrero"
          },
          "distinct": true,
          "id": "15bce140a7e6dd12a4a9756e737be52178fd5791",
          "message": "Revert \"build: update OSTk physics to 4.0.2\"\n\nThis reverts commit 939215672294d407d89309c0b6bb9e409292af19.",
          "timestamp": "2023-11-29T15:13:03+01:00",
          "tree_id": "7db09c23160052d85105fcc4a10d23366311bca8",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/15bce140a7e6dd12a4a9756e737be52178fd5791"
        },
        "date": 1701268979197,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 545450955.2999923,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 545374759.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 793902037.9000056,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 793864760.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2811363018.5000033,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2811050160 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2804270923.0000014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2804062330.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2801904267.6000085,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2801796970.0000024 ns\nthreads: 1"
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
          "id": "ab8fa7368575a3fe8b95bbf5bbdd87b1a0e18e95",
          "message": "build: update OSTk physics to 4.0.2 (#288)\n\n* build: update OSTk physics to 4.0.2\r\n\r\n* chore: remove wrong licencing\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau@Paus-MacBook-Pro.local>",
          "timestamp": "2023-11-29T16:50:28+01:00",
          "tree_id": "30c35000cdebd9fed61f58a691bcf14ef482c866",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ab8fa7368575a3fe8b95bbf5bbdd87b1a0e18e95"
        },
        "date": 1701274732694,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 554709989.0000083,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 554691689.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794493160.0000075,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794467839.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2786109073.7999973,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2785963790.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2785192270.199991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784932600.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2784428195.000004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784277439.9999995 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}