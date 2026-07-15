window.BENCHMARK_DATA = {
  "lastUpdate": 1784139708097,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "name": "open-space-collective",
            "username": "open-space-collective"
          },
          "committer": {
            "name": "open-space-collective",
            "username": "open-space-collective"
          },
          "id": "9072bde8bff2340073d4f40d0249903fbb1e6888",
          "message": "ci(tmp): fix benchmark",
          "timestamp": "2026-07-15T12:54:11Z",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/pull/695/commits/9072bde8bff2340073d4f40d0249903fbb1e6888"
        },
        "date": 1784139706191,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4547117901.599996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4531493199.000001 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1314.5038806666964,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1314.4100310000013 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 4167.740779999993,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 4167.465177666668 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 15194.976844000013,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 15194.014992666665 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4051500685.0999956,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4051234645.5000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7174745517.800023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7174310540.299995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7171332259.199984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7170683072.200003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7169104092.600014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7168656629.900004 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.188766337000061,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.1886551349999763 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.334021641999925,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.333707957999991 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.118947761999834,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.118182201000025 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.829758285999787,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.82923944199996 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.152484440999842,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.151777623000044 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}