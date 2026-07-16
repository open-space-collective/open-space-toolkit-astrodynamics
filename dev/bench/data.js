window.BENCHMARK_DATA = {
  "lastUpdate": 1784220151910,
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
          "id": "6e2a3b78b760fd77ad1db43346a66bbe485040e7",
          "message": "build: fix packaging (#696)",
          "timestamp": "2026-07-16T16:06:43Z",
          "tree_id": "babcf08fdfed8a21a457266c0958e5158d9d539f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6e2a3b78b760fd77ad1db43346a66bbe485040e7"
        },
        "date": 1784220150245,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4643064203.100027,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4629654067.699999 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1330.0614866666745,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1329.8409576666686 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 4260.85541400001,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 4259.5296953333345 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 15533.915228999982,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 15532.52321733333 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4112728466.399995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4112131919.0000076 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7266290428.499974,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7265330048.700003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7271917740.000015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7270936201.400004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7256837519.500005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7256113765.700007 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.2142113470001732,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.2139695399999368 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.433598608000125,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.433038417000034 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.290424348999977,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.289051279999967 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.952795624000032,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.952045611000017 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.363907437999956,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.362638067999967 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}