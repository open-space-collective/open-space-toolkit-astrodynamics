window.BENCHMARK_DATA = {
  "lastUpdate": 1698265890484,
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
          "id": "30ca4aa199338787e8ce4e40e6f333126f22fefc",
          "message": "chore: align release dockerfile with ostk-data seeding paradigm (#258)\n\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2023-10-22T22:48:42Z",
          "tree_id": "269f9e3fecd2fdb8c3d07e6a19b1486bc43aebb5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/30ca4aa199338787e8ce4e40e6f333126f22fefc"
        },
        "date": 1698017112372,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 703098763.1000073,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 703004630 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1481795798.6000009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1481710180.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4157491775,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4157145200.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4158035048.4000006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4157810889.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4157610275.400009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4157397729.999999 ns\nthreads: 1"
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
          "id": "048bf6e048dbb46b3fcbaf4b450086541432b67a",
          "message": "fix: migrate ostk-data cache location to /var/cache (#260)",
          "timestamp": "2023-10-23T22:54:01+02:00",
          "tree_id": "352ab4c4924075fa31f756ac331f358c5b7e42ff",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/048bf6e048dbb46b3fcbaf4b450086541432b67a"
        },
        "date": 1698097186022,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 1098942587.7000089,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1098161710.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2078505430.8000157,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2078042509.9999988 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 5333919373.099991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5332758020 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5346472085.999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5345771689.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5307418752.599983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5306151109.999996 ns\nthreads: 1"
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
          "id": "174cba3400c9e31e711ad78505b2bc935ce638cf",
          "message": "docs: add python docstrings (#261)\n\n* chore: add docstrings for python bindings\r\n\r\n* chore: add docstrings for python bindings\r\n\r\n* chore: fix some groups\r\n\r\n* feat: cleanup the docstrings\r\n\r\n* feat: cleanup docstrings more\r\n\r\n* feat: further refine docstrings\r\n\r\n* feat: last cleanup",
          "timestamp": "2023-10-25T12:50:12-07:00",
          "tree_id": "26ccab597fa9767177e1e9673e84ae0990daef39",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/174cba3400c9e31e711ad78505b2bc935ce638cf"
        },
        "date": 1698265889650,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 741396453.5999867,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 740965860.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1659151564.6999936,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1658707680.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4034770626.5,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4033625220.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4042655340.0999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4041705639.9999976 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4139553237.799993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4138602179.9999976 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}