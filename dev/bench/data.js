window.BENCHMARK_DATA = {
  "lastUpdate": 1782162241820,
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
          "id": "f0b9e53d77de2ebbd818d222aeec3ba62643e24b",
          "message": "fix: frame comparison checks (#686)",
          "timestamp": "2026-06-10T00:29:07Z",
          "tree_id": "6504c0526a09ce5a4a22c070461729808df195e4",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f0b9e53d77de2ebbd818d222aeec3ba62643e24b"
        },
        "date": 1781169483110,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 3884782534.6000037,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3803904987.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 3499157175.599993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3498614002.1000023 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6206550278.00001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6205644936.100002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6208037453.400004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6207169592.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6212118331.600021,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6211268645.600006 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.9823683339999434,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.9822739690000617 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 4.384184647999973,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.383584846000019 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 8.495304939999983,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.494077289000018 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 6.3716451049999705,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.3707170360000305 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 9.013418507999972,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 9.012343132000012 s\nthreads: 1"
          }
        ]
      },
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
          "id": "30a39924abfd25d0c08fdcc98c4da9934ffb57f6",
          "message": "feat: add Propagated to Tabulated method (#687)\n\n* feat: add propagated to tabulated method\n\n* chore: fix type",
          "timestamp": "2026-06-22T13:32:34-07:00",
          "tree_id": "0fea0935244b756d8f51dd73f257a887c1d8aa45",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/30a39924abfd25d0c08fdcc98c4da9934ffb57f6"
        },
        "date": 1782162239796,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4673108668.099991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4618130181.200001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4111900125.7999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4111538259.200001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7239785933.500008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7239233481.100001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7235983541.70001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7235387767.299997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7244998490.299986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7244413494.600008 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.223549595999998,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.2234879919999457 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.398135615000115,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.397560089999956 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.002758215000085,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.001663803999975 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 8.04454504599994,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.041622078999978 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.40465864700002,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.403711998000006 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}