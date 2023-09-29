window.BENCHMARK_DATA = {
  "lastUpdate": 1695998928589,
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
      }
    ]
  }
}