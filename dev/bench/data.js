window.BENCHMARK_DATA = {
  "lastUpdate": 1708007582229,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "98616558+apaletta3@users.noreply.github.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e4621238a9f9602319a3be07668b124592ce8acb",
          "message": "fix: incomplete solve namespace update and OSTk physics bump (#352)\n\nfix: solve namespace update",
          "timestamp": "2024-02-15T14:09:00Z",
          "tree_id": "af5a7a01f5b287582215de862de3e5d3ae526daf",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e4621238a9f9602319a3be07668b124592ce8acb"
        },
        "date": 1708007580556,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 540362762.9999931,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 540337019.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 787264528.1000018,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 787225210 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2783158566.399996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783041820.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2781266346.900008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781169669.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2786455380.5000014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786331259.9999986 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}