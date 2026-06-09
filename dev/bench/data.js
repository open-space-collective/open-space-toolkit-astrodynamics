window.BENCHMARK_DATA = {
  "lastUpdate": 1781044982976,
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
          "id": "ae99aec9dc39a2dba6b8198b0d86fa8a19447b11",
          "message": "feat: interpolators per coordinate subset for tabulated models (#683)\n\n* feat: interpolators per coordinate subset for tabulated models\n\n* feat: simplify construction\n\n* refactor: align tests\n\n* chore: style",
          "timestamp": "2026-06-09T20:58:18Z",
          "tree_id": "6166a357688560e3dc44eb1f5cae453410e65292",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ae99aec9dc39a2dba6b8198b0d86fa8a19447b11"
        },
        "date": 1781040572895,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4709568095.900004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4578056117 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4064132424.400009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4063732561.399999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7190969758.700009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7190198526.400002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7178052657.100004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7177397156.299997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7180944969.999995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7180110100.399992 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.1949668610000117,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.1948776849999945 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.426470990999974,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.424033974999986 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 10.983121690999951,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.98218024199997 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.890299833999961,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.889576427999998 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.226982134999957,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.22596946799996 s\nthreads: 1"
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
          "id": "943085773a506c9a82aaebb2b6855f2ef6a34123",
          "message": "feat: add utility methods to COE (#685)",
          "timestamp": "2026-06-09T15:13:04-07:00",
          "tree_id": "6a0354b12f7b2f1f77fafef58597aecc00e2dfe0",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/943085773a506c9a82aaebb2b6855f2ef6a34123"
        },
        "date": 1781044981651,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4683145127.4999895,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4600776836.800001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4079124697.7999945,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4078836047.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7205226331.600011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7204751640.800003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7213008720.799991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7212528949.799997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7214231801.200003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7213863529.200003 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.1999091479999606,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.1998366909999731 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.39861178000001,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.398271125000008 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.181727404999947,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.181200563999994 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.919155828000044,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.918741232000002 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.362315957999954,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.361552503000041 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}