window.BENCHMARK_DATA = {
  "lastUpdate": 1722744276276,
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
          "id": "71601085aadea56725fb23bcb2e470ad3beb87ed",
          "message": "feat: c++ compatibility (#425)\n\n* feat: c++ 20 compatibility\r\n\r\n* chore: warnings",
          "timestamp": "2024-08-03T20:18:22-07:00",
          "tree_id": "639e6b6d18ee0ff01b45f33b5953d0efdbab007a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/71601085aadea56725fb23bcb2e470ad3beb87ed"
        },
        "date": 1722744273455,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 554623154.4000022,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 554603085 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 873054796.2000059,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 873023906.5999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3170821759.3999963,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3170713368.2000012 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3167938561.400001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3167799290.5999985 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3167590173.6999946,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3167480783.600004 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}