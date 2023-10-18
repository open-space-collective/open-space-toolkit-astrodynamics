window.BENCHMARK_DATA = {
  "lastUpdate": 1697649473921,
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
          "id": "6c2058e09d09f804fc7a59361f795142b81d87da",
          "message": "fix: fix extra state being observed in integrating with conditions (#256)\n\n* fix: fix extra state logging in integrating with conditions\r\n\r\n* chore: add test",
          "timestamp": "2023-10-18T09:39:29-07:00",
          "tree_id": "f78528d2794f03a09861d6cab45518ece6302f83",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6c2058e09d09f804fc7a59361f795142b81d87da"
        },
        "date": 1697649472588,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 700804847.3999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 700712730.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1475080339.2000136,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1474736170.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4144850291.1000047,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4144665029.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4144433202.800008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4144242720 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4148790150.700006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4148509929.9999986 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}