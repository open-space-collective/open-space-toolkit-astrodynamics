window.BENCHMARK_DATA = {
  "lastUpdate": 1707783055069,
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
          "id": "8f54318d7092d0d1f92dfa9ef9f36f97591c75bf",
          "message": "refactor!: remove profile state (#338)\n\n* refactor!: remove profile state and use trajectory state\r\n\r\n* feat: fix tests\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa@users-MacBook-Pro.local>",
          "timestamp": "2024-02-12T10:01:14-08:00",
          "tree_id": "47e0f3287526e3fd5dd5143f2cd139c89d873199",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/8f54318d7092d0d1f92dfa9ef9f36f97591c75bf"
        },
        "date": 1707762921262,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 557508296.1000049,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 557471360 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 807520447.3999946,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 807486789.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2791665527.1000166,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791524979.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2786615703.2000046,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786512710.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2793297819.6999955,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2793197089.9999995 ns\nthreads: 1"
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
          "id": "75407430ca219ec4de370983ac73d0a05bdeaabe",
          "message": "chore: bump Core, IO, Math, and Physics (#336)\n\n* chore: bump Core, IO, and Math\r\n\r\n* chore: bump physics and align namespaces\r\n\r\n* chore: fix divergence from rebase\r\n\r\n* chore: bump math to 2.1 instead of 2.0\r\n\r\n* Revert \"chore: bump math to 2.1 instead of 2.0\"\r\n\r\nThis reverts commit 87f704ac5bfeb42c4e3e3b858e7ac463f55d135a.\r\n\r\n* chore: fix some missed imports\r\n\r\n* chore: remove debug define",
          "timestamp": "2024-02-12T15:37:19-08:00",
          "tree_id": "6fa346d340601248ffdd66b40afbc53f539d9c8f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/75407430ca219ec4de370983ac73d0a05bdeaabe"
        },
        "date": 1707783053556,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 557948861.500006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 557901210 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 801004604.7000031,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 800954020.0000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2791659212.5000024,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791326689.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2788070107.0999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787902350.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2789025667.4,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788813519.999999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}