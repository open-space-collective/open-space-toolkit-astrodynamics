window.BENCHMARK_DATA = {
  "lastUpdate": 1785774515361,
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
          "id": "b79bed30f5c481d431a2b8b3840dcd316ae693c6",
          "message": "chore: don't overwrite devcontainer venv (#704)",
          "timestamp": "2026-08-03T15:50:36Z",
          "tree_id": "9f0645bb1f9ec7efee8e045592684c37fb3a15fa",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b79bed30f5c481d431a2b8b3840dcd316ae693c6"
        },
        "date": 1785774513690,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 3437320183.8000054,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3405768245.1000004 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1062.0409946666693,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1061.9135329999997 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 3327.3384560000068,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 3326.933406666669 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 12369.238950666671,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 12368.170607 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 3240604271.100011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3240364538.799996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6033015687.099987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6032569209.399995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6032466445.100004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6031839253.5999975 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6043115385.699991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6042182215.600001 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.9799130269999523,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.9784112359999426 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 4.745263694000073,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.744703252000022 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 10.146756740000114,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.145977598000002 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.34892761399999,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.348394787000018 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 10.153961269999854,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.153080633999991 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}