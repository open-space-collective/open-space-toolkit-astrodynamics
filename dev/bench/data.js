window.BENCHMARK_DATA = {
  "lastUpdate": 1719352203599,
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
      }
    ]
  }
}