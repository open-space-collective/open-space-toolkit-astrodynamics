window.BENCHMARK_DATA = {
  "lastUpdate": 1715363886539,
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
          "id": "94fab231ca2808fd9a475dd9068e12f413db7973",
          "message": "chore: change dependabot to ignore major releases (#395)\n\n* chore: disable dependabot\r\n\r\n* Update dependabot.yml",
          "timestamp": "2024-05-10T17:38:05Z",
          "tree_id": "aa732903748696569972fd7f12a6fcbfb8f7e395",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/94fab231ca2808fd9a475dd9068e12f413db7973"
        },
        "date": 1715363885129,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 549849687.0000054,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 549829796.3000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 795381852.8999989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 795357208.5000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2780043700.4999876,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2779956865.3999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2774511000.3000093,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2774414064.300003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2777001973.199992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2776936511.8999944 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}