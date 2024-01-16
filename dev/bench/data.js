window.BENCHMARK_DATA = {
  "lastUpdate": 1705365414263,
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
          "id": "462bd587aa4bd913b1c4c577e920a29322af84f6",
          "message": "feat: add argument of latitude options to Orbit::SunSynchronous (#319)\n\n* feat: add ascending node and argument of latitude options to Orbit::SunSynchronous\r\n\r\n* chore: missed a spot",
          "timestamp": "2024-01-16T11:02:51+11:00",
          "tree_id": "575bf8970a38a6f83406ace6ab076752da6697ae",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/462bd587aa4bd913b1c4c577e920a29322af84f6"
        },
        "date": 1705365413573,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 555376308.6999965,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 555340540 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794273386.2999943,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794204670 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2792736234.6999927,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792560200.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2797126729.1999993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2797026470.0000024 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2791832901.2000074,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791778799.999999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}