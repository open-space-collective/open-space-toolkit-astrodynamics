window.BENCHMARK_DATA = {
  "lastUpdate": 1707506557673,
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
          "id": "d512b77fb9cedad0cfb6e87f1b9d37d6d2abc8eb",
          "message": "feat: add xyz axes plotting option to viewer (#337)",
          "timestamp": "2024-02-09T10:49:06-08:00",
          "tree_id": "9bf5a9c2f02f36ae20f138ec2e4232bc127e3b26",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/d512b77fb9cedad0cfb6e87f1b9d37d6d2abc8eb"
        },
        "date": 1707506556336,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 549222998.0999696,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 549194519.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 790349095.6000155,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 790307510.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2766682829.3999968,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2766571960 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2747434622.200001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2747323899.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2754556481.2999945,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2754436660.0000005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}