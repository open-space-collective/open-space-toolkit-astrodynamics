window.BENCHMARK_DATA = {
  "lastUpdate": 1697818028282,
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
      },
      {
        "commit": {
          "author": {
            "email": "kyle.ray.cochran@gmail.com",
            "name": "kyle-cochran",
            "username": "kyle-cochran"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "f964cf6b9e7dcc407d42888a4e370aab0eed73d7",
          "message": "feat: add hasSubset method to State (#259)",
          "timestamp": "2023-10-19T14:03:14-07:00",
          "tree_id": "25038016ffa36415eb251bbe699f4622ae148b2a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f964cf6b9e7dcc407d42888a4e370aab0eed73d7"
        },
        "date": 1697818026697,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 694651142.0000093,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 694492070.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1486414979.3000026,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1486290660.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4153320954.1000075,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4153053500.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4150838487.000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4150599080.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4150871723,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4150644689.9999986 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}