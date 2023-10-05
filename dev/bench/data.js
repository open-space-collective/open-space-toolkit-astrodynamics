window.BENCHMARK_DATA = {
  "lastUpdate": 1696526929722,
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
          "id": "3a708d6bf26d668631687fb8ba0df8bea8151dda",
          "message": "feat: update State printing to support extended state (#231)\n\n* feat: update State printing to support extended state\r\n\r\n* feat: explicitly print the Frame\r\n\r\n* Update src/OpenSpaceToolkit/Astrodynamics/Trajectory/State.cpp\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>\r\n\r\n* refactor: remove full coordinates print line for consistency with previous printing\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2023-10-05T09:53:38-07:00",
          "tree_id": "dc9ff5591fc49d8803fa66309e6a879d15b6385a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/3a708d6bf26d668631687fb8ba0df8bea8151dda"
        },
        "date": 1696526928697,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 698307521.5000042,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 698241649.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1480035435.700006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1479891389.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4158297952.7999954,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4158006969.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4160040780.8,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4158975040.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4159299660.1000037,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4159055809.999997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}