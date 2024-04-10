window.BENCHMARK_DATA = {
  "lastUpdate": 1712786048533,
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
          "id": "eaf3e9f35a3f4f3b3bb7da37d04816032ea382e9",
          "message": "fix: throw error in Profile::getBodyFrame if frame already exists (#388)",
          "timestamp": "2024-04-10T21:00:51Z",
          "tree_id": "9a1bef093721ea27cea2766e02f5bfdba0b7ef93",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/eaf3e9f35a3f4f3b3bb7da37d04816032ea382e9"
        },
        "date": 1712786046686,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 546410753.8000121,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 546392313.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 789874698.6000049,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 789835716.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2774646890.2000003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2774559044 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2779420983.1000047,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2779356707.1000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2773335119.6999936,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2773264472.4000025 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}