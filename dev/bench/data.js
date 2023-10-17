window.BENCHMARK_DATA = {
  "lastUpdate": 1697582911978,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
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
          "id": "7c1420a156ec157d8a8a4a3d6d642acc6d4713e8",
          "message": "feat: make the propagator maintain the frame and size of the input State (#246)\n\n* feat: make the propagator maintain the frame and size of the input State in the output\r\n\r\n* chore: uncomment other tests\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>\r\n\r\n* refactor: make propagator StateBuilder a local variable and fix syntax errors from uncommenting code\r\n\r\n* fix: remove hard-coded gcrf usage in Propagated model\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2023-10-17T15:10:11-07:00",
          "tree_id": "1a6dd0fedc6d10dbc161e97eebe3d185f1a4a334",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/7c1420a156ec157d8a8a4a3d6d642acc6d4713e8"
        },
        "date": 1697582910343,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 873247040.8999916,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 872792190 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1965748196.6999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1964664190 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4959324205.600012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4958335809.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4993826950.599998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4993011950.000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4992825556.899993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4992034780.000006 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}