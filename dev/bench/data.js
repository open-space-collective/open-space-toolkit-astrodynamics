window.BENCHMARK_DATA = {
  "lastUpdate": 1784921713618,
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
          "id": "1cbd75ef1c5b8ca7268e4d4f20d017293008b8ac",
          "message": "build: support Python 3.14 (#701)",
          "timestamp": "2026-07-24T19:00:14Z",
          "tree_id": "7f5d1241efeada708b0cef4277e6a4cb8dc06e1c",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/1cbd75ef1c5b8ca7268e4d4f20d017293008b8ac"
        },
        "date": 1784921712317,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4622127866.600004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4604550473.800001 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1337.2972893333401,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1337.2068709999994 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 4231.213885999997,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 4230.964725333334 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 15498.864776999995,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 15497.866847000003 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4112037150.6000137,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4111753396.0999975 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7249114354.899996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7248707000.800005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7231447663.200003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7230840210.199997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7235301130.699997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7234898861.999994 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.2123075089999702,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.2122197630000073 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.396712665999985,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.396279676000006 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.397335542000064,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.396799127000008 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.929777167999873,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.929351752000002 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.416086396999958,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.415315827999962 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}