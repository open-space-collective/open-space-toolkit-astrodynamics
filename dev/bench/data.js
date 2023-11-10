window.BENCHMARK_DATA = {
  "lastUpdate": 1699625801986,
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
          "id": "79d7ac2377439a1b140df7d5c068dc55d845c3e7",
          "message": "fix: fix relative logical conditions usage with Sequences (#269)\n\n* fix: fix relative logical conditions usage with Sequences\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* feat: fix last test\r\n\r\n* feat: add print test\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2023-11-08T10:42:32-08:00",
          "tree_id": "09b1f0fc5838ee72c8d129a0a41ed8b9933afa2f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/79d7ac2377439a1b140df7d5c068dc55d845c3e7"
        },
        "date": 1699472169119,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 877665144.9999803,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 877295540.0000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1965459416.4999936,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1964771810.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4976028503.800013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4974863820 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5000121275.700007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4998870669.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4988577794.799982,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4987416520.000002 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "92c1251696352b37d43ce28342a9da47a7f3d6ea",
          "message": "build: update OSTk physics to 3.0.0 (#271)\n\n* build: update to ostk physics 3.0.0\r\n\r\n* chore: update EarthAtmosphericModel full constructor",
          "timestamp": "2023-11-10T14:34:04+01:00",
          "tree_id": "e6c8965bc384159a46c3e5064270b4f0b2c55cf0",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/92c1251696352b37d43ce28342a9da47a7f3d6ea"
        },
        "date": 1699625800680,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 859490909.3000069,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 859375640 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1166764955.5999675,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1166567370.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6137577905.000001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6136733310 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6238394245.700022,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6237593299.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6066629517.000002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6065935860.000002 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}