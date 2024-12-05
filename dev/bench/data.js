window.BENCHMARK_DATA = {
  "lastUpdate": 1733442713388,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "44425077+tapiab@users.noreply.github.com",
            "name": "Benoit Tapia",
            "username": "tapiab"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "298e57e92afcd13829c9e1fa8d545e1ac429aeeb",
          "message": "chore: allow build without Boost Stacktrace (#466)",
          "timestamp": "2024-12-02T09:33:42-08:00",
          "tree_id": "990404b639c56fdb0db7055aa3baafcfebf5fab2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/298e57e92afcd13829c9e1fa8d545e1ac429aeeb"
        },
        "date": 1733162065113,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 581983448.5999991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 581955621.6999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 879448204.0999867,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 879420719.9999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3186210979.7000017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186072879.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3187003595.7999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186860106.100003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3185987643.0999804,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3185884707.400001 ns\nthreads: 1"
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
          "id": "fba3e5eed67cb804f04815c5daf720002cbb3581",
          "message": "refactor: remove extra quaternion conversion methods (#469)",
          "timestamp": "2024-12-05T16:33:55-07:00",
          "tree_id": "0b9e0290c1c64d7f4488b7dba82763176d2a5f26",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/fba3e5eed67cb804f04815c5daf720002cbb3581"
        },
        "date": 1733442710672,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 587148304.2000023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 587131377.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 878405508.3000113,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 878365609.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3188679965.299991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188543807.4000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3186477175.800013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186183228 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3188205019.800006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188075262.3999996 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}