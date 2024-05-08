window.BENCHMARK_DATA = {
  "lastUpdate": 1715165330195,
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
          "id": "f48b10712c48b55aee40b3f216abaf729e075a9a",
          "message": "chore: update build-documentation command to use ostk-build-docs (#394)",
          "timestamp": "2024-05-08T11:55:13+02:00",
          "tree_id": "78df59aefd466226699c4a70d11d5b57ee23e84e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f48b10712c48b55aee40b3f216abaf729e075a9a"
        },
        "date": 1715165328003,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 554529183.5999933,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 554505085.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 797915399.9000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 797896763.4000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2780419535.79999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2780322326.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2778616896.099999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2778486329.6999993 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2778546756.90001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2778418385.4000006 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}