window.BENCHMARK_DATA = {
  "lastUpdate": 1780648882110,
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
          "id": "4bce26b7d9d4f9592f23d94c64931169686eee9f",
          "message": "fix: add frame property to residual to keep it backwards compatible (#682)",
          "timestamp": "2026-06-01T18:03:29Z",
          "tree_id": "af4db3b34ff5d665ea0ba347aa09c80dc5bcb352",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/4bce26b7d9d4f9592f23d94c64931169686eee9f"
        },
        "date": 1780338812623,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4704088876.600002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4568472720.600001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4059370896.100006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4059010632.1 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7179365618.200018,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7178706693.600004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7178063618.300018,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7177406695.299998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7183670645.199992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7183154320.600001 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.19188932499992,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.191754336000031 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.400817613999948,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.40022746599999 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 10.969223022000051,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.968043292000004 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.828721814000005,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.828057194000053 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.224640569999906,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.223760818000017 s\nthreads: 1"
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
          "id": "5537491ad2813d425cd76af04c8ad9f210f2a398",
          "message": "fix: qlaw targeting without sma (#684)\n\n* fix: qlaw targeting without sma should pass\n\n* style: apply linter\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-06-05T08:13:10Z",
          "tree_id": "ebfde8f979ec94e8d3db87d8f9421e0099a65186",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/5537491ad2813d425cd76af04c8ad9f210f2a398"
        },
        "date": 1780648880295,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 3556747402.799994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3407926007.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 3238816341.7000213,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3238584066.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6037450086.000007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6037001879.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6037476627.199999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6037066927.699998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6033366738.499984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6032782682.999996 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.971395068999982,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.9713103029999672 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 4.755488268000136,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.755179104000007 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 10.185298325000076,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.184498861000037 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.05271737299995,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.0521679440000185 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 10.05016247900005,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.049206314999992 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}