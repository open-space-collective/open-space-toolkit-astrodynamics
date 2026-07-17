window.BENCHMARK_DATA = {
  "lastUpdate": 1784295383379,
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
          "id": "c464b24b528dc72e86a319ec9413a3fbcd34d31d",
          "message": "build: bump to ostk-physics 14 (#698)\n\n* build: bump ostk-physics~=14\n\n* chore: add missed alignments to ostk-core",
          "timestamp": "2026-07-17T13:02:17Z",
          "tree_id": "76e1c9ffa2bb90d1cdfdc018ee05e9c0075a07b1",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/c464b24b528dc72e86a319ec9413a3fbcd34d31d"
        },
        "date": 1784295381735,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4548305193.700003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4530475782.7 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1316.9462059999926,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1316.8072343333345 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 4181.222709000015,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 4180.8100686666685 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 15245.37602766666,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 15244.317844666666 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4053626709.799999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4053330662.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7174099785.499994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7173478286.299997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7172364025.000002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7171625473.700002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7179019492.8,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7177821465.399996 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.191887080000015,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.1917676379999875 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.340531184000042,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.3401926490000164 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.183078788999978,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.18246124899997 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.843327341000077,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.842789117000052 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.273424590999866,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.272633620000022 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}