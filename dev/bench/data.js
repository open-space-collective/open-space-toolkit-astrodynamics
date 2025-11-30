window.BENCHMARK_DATA = {
  "lastUpdate": 1764480699095,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "35860220+alex-liang3@users.noreply.github.com",
            "name": "Alex Liang",
            "username": "alex-liang3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "45ab06acbce3fb50c917a54f1a0edcb44714598c",
          "message": "docs: make script example compilable (#614)",
          "timestamp": "2025-11-30T10:38:54+05:30",
          "tree_id": "02cd8547af8be0dc1702def25b4f41107c3a42d5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/45ab06acbce3fb50c917a54f1a0edcb44714598c"
        },
        "date": 1764480697550,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 850162860.5999997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 800557068.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 894929006.2999865,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 894793459.6999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3215766943.2999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3215572098.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3201709218.700006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3201521481.2000012 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3199879244.599998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199638050.7999988 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}