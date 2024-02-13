window.BENCHMARK_DATA = {
  "lastUpdate": 1707850037769,
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
          "id": "7e8443130dc1c93f63b93634b9713ed8cf75457f",
          "message": "fix: temporarily remove ostk.astrodynamics.solvers from documentation (#343)",
          "timestamp": "2024-02-13T09:09:52Z",
          "tree_id": "d9c2b337b16bd38e3e369f0c78be3cb780316abf",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/7e8443130dc1c93f63b93634b9713ed8cf75457f"
        },
        "date": 1707817305502,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 556502708.4000008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 556467739.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796270342.8999987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796219079.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2818994621.300021,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2818812030.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2820665247.500011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2820356929.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2818194196.5999956,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2818051550.000001 ns\nthreads: 1"
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
          "id": "6b65f18a98bf5697f742066c7b2a0dc649485a5d",
          "message": "refactor!: align namespaces to singular (#341)\n\n* chore: bump math to 2.1 instead of 2.0\r\n\r\n* Revert \"chore: bump math to 2.1 instead of 2.0\"\r\n\r\nThis reverts commit 87f704ac5bfeb42c4e3e3b858e7ac463f55d135a.\r\n\r\n* chore: fix some missed imports\r\n\r\n* chore: remove debug define\r\n\r\n* feat: first pass\r\n\r\n* chore: second pass of renaming\r\n\r\n* fix: cpp tests\r\n\r\n* fix: python tests\r\n\r\n* refactor: change coordinatesbroker\r\n\r\n* fix: CoordinatesBroker python bindings\r\n\r\n* Apply suggestions from code review\r\n\r\nself review\r\n\r\n* refactor: coordinatessubsets namespace\r\n\r\n* fix: documentation build\r\n\r\n* feat: align namespaces to singular suggestions (#342)\r\n\r\nfeat: suggestions\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2024-02-13T10:14:00-08:00",
          "tree_id": "104e41655feebfe5389d1d5bb7c6ef2d40d86f12",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6b65f18a98bf5697f742066c7b2a0dc649485a5d"
        },
        "date": 1707850036236,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 557354938.8999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 557314319.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 803662296.0000045,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 803602269.9999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2796567226.7000003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2796425439.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2791574899.600016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791468739.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2792601919.9000053,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792409050.0000005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}