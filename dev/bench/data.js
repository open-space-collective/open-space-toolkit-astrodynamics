window.BENCHMARK_DATA = {
  "lastUpdate": 1780013867487,
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
          "id": "c1bcf9f0ec561eb6753a0bba81d7f3ead91e7d39",
          "message": "feat: improve residual computation utilities (#681)",
          "timestamp": "2026-05-28T23:46:08Z",
          "tree_id": "8f752f06cd925a860595cb4cd57568d017a85c7b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/c1bcf9f0ec561eb6753a0bba81d7f3ead91e7d39"
        },
        "date": 1780013864739,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4951987083.499994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4909680301.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4524013554.500005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4523380189.300001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 8026794437.399984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 8025395440.399997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 8027001474.2000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 8025850250.100001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 8022096296.899997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 8021009802.500006 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.26595614200005,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.2658350660000224 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.638902704999964,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.638123047000022 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.029842450999922,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.02832153899999 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 8.167823301999874,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.166546729000004 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.611194201999979,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.609472947000029 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}