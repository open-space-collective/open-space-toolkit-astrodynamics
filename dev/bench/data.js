window.BENCHMARK_DATA = {
  "lastUpdate": 1769596962459,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "65550121+phc1990@users.noreply.github.com",
            "name": "Pau Hebrero",
            "username": "phc1990"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "2654e6d7327fc05ab676fb98303c0c45b8191388",
          "message": "[draft] perf: improve Segment Chunk strategy performance (#644)\n\n* perf: improve Segment Chunk strategy performance\n\n* refactor: apply PR suggestions\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-01-28T10:14:41Z",
          "tree_id": "bb6b7c207cdbd69cddd926603a81a1ecf870d090",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/2654e6d7327fc05ab676fb98303c0c45b8191388"
        },
        "date": 1769596960570,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4846900020.29999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4659304964.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4068209611.3,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4067925233.2000017 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7175623082.200007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7175028138.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7173380255.399991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7172921485.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7188011804.6,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7187592786.2000065 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}