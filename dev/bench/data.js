window.BENCHMARK_DATA = {
  "lastUpdate": 1696354347254,
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
          "id": "b0bde4ba0d7f630a4379cabdb1486457a828c51c",
          "message": "chore: move dynamics to root of ostk astro (#229)\n\n* chore: move dynamics at root of astro folder\r\n\r\n* chore: adapt notebooks",
          "timestamp": "2023-09-29T16:12:49+02:00",
          "tree_id": "b0a77c9ad22bbcbb7d7fab61b9a4f0573f23f72b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b0bde4ba0d7f630a4379cabdb1486457a828c51c"
        },
        "date": 1695998927683,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 741826503.7999959,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 741729210 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1660303879.0999904,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1660098720.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4163677395.6999884,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4163192709.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4164507585.999996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4163770760.000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4158081878.5000076,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4156986429.999996 ns\nthreads: 1"
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
          "id": "d3581793f86b94d85159752e4282798461d828a5",
          "message": "build: update physics to 2.1.2 (#230)",
          "timestamp": "2023-10-03T18:49:27+02:00",
          "tree_id": "c13b082302ef6e7c0a4df2e7914a54b2115ab0ad",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/d3581793f86b94d85159752e4282798461d828a5"
        },
        "date": 1696354345789,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 1129456662.9999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1129137140 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2222358849.699992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2217716720.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 5634637327.199994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5632523549.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5566386992.8,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5564232080.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5563404697.000021,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5561245890.000001 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}