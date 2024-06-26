window.BENCHMARK_DATA = {
  "lastUpdate": 1719410135905,
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
          "id": "6a2e3038afdc3504c1643c03c68eb093ad0c864e",
          "message": "build: add option to build executable (#418)\n\n* wip:\r\n\r\n* Apply suggestions from code review\r\n\r\n* feat: address feedback",
          "timestamp": "2024-06-25T14:31:24-07:00",
          "tree_id": "00b52e78c0579642721166a15165c50cc89a2fe2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6a2e3038afdc3504c1643c03c68eb093ad0c864e"
        },
        "date": 1719352201975,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 547205647.2999781,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 547189135.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 780915855.4000021,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 780890993.9000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2782920924.5000014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782868281.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2781499813.300002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781430281.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2781355906.3000073,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781263184.1 ns\nthreads: 1"
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
          "id": "6687920acf649daa1cbea6f2f82e0a1dc2719dcd",
          "message": "feat: add more pass computation methods (#414)\n\n* feat: provide initial revolution number for Propagated model\r\n\r\n* feat: fix tests\r\n\r\n* feat: address feedback\r\n\r\n* feat: all the things\r\n\r\n* feat: fix from rebase\r\n\r\n* feat: small fixes\r\n\r\n* feat: clean up tests\r\n\r\n* feat: address feedback\r\n\r\n* feat: clean up the model method\r\n\r\n* fix: tests\r\n\r\n* feat: add support for backward propagation\r\n\r\n* feat: propagate last step\r\n\r\n* feat: re-order\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\r\n\r\n* fix: typo\r\n\r\n* feat: fix tests\r\n\r\n* feat: fix tests\r\n\r\n* feat: Antoines suggestions\r\n\r\n---------\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2024-06-26T06:38:30-07:00",
          "tree_id": "e1b817ea6a6af72ca4fdd381da2ae01cb50fa436",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6687920acf649daa1cbea6f2f82e0a1dc2719dcd"
        },
        "date": 1719410134210,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 548188289.8000038,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 548146998.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 793455285.0999978,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 793420940.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2797791682.9999857,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2797632691.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2800418592.099993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2800233983.800001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2792497604.9999943,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792316072.200005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}