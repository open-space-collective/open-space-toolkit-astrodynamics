window.BENCHMARK_DATA = {
  "lastUpdate": 1701196820261,
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
          "id": "e42637d9af2a1fa1da84dd025e5b735603223961",
          "message": "feat: add convergence threshold for q law (#287)\n\n* feat: add convergence threshold criteria for QLaw\r\n\r\n* feat: set thrust direction to null based on deltaCOE and threshold\r\n\r\n* feat: successssss\r\n\r\n* feat: fix tests\r\n\r\n* refactor: generalize compute deltaCOE",
          "timestamp": "2023-11-28T18:12:29Z",
          "tree_id": "b94b79244759d3468cb2c80e3917c4e16b9cd37d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e42637d9af2a1fa1da84dd025e5b735603223961"
        },
        "date": 1701196819355,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 554064696.0999993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 554036110 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 799335620.0999982,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 799300450.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2787950957.3000064,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787644300.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2792224501.6000147,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792071339.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2788524078.8000145,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788206320.0000005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}