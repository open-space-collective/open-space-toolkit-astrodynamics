window.BENCHMARK_DATA = {
  "lastUpdate": 1784146006823,
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
          "id": "14aaa742cb17fcf444913dca5c3fa3a8e15694bf",
          "message": "build: switch to uv, upgrade project (#659)\n\n* build: switch to uv\n\nbuild: use new base image\n\nstyle: fix formatting\n\nfix: skip building docs, don't blow up machine\n\nci: use test branch\n\nwip\n\nci: try build artifacts\n\nbuild: update ref\n\nUpdate Makefile\n\nRevert \"ci: try build artifacts\"\n\nThis reverts commit 947ba749a3a6c1df19f48a25dd9f809c7fd8531b.\n\nbuild: test removing ci-test\n\nci: update actions versions\n\nbuild: add specialized benchmark and validation builds\n\nci: split c++ and python tests\n\nCo-authored-by: Copilot <copilot@github.com>\n\n* build: update dockerignore, split package builds\n\nCo-authored-by: Copilot <copilot@github.com>\n\n* chore: clean up *-standalone targets\n\nCo-authored-by: Copilot <copilot@github.com>\n\n* Apply suggestion from @alex-liang3\n\n* Apply suggestion from @alex-liang3\n\n* build: remove cesiumpy as dependency\n\n* chore: update base image ref\n\n* chore: align with ostk-core, bump cesiumpy\n\n* ci: undo temp change\n\n* chore: fix makefile\n\n* Apply suggestion from @alex-liang3\n\n---------\n\nCo-authored-by: Copilot <copilot@github.com>",
          "timestamp": "2026-07-15T12:53:29Z",
          "tree_id": "39149c8e708c3b681b4633483ad0b01ec230d0c5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/14aaa742cb17fcf444913dca5c3fa3a8e15694bf"
        },
        "date": 1784146004937,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4879013814.900003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4860931826.1 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1395.9765379999756,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1395.7619893333324 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 4516.387639666694,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 4515.638816333331 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 16587.353990333326,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 16585.041025333336 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4504817450.599989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4504296925.599999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 8002825672.399979,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 8001910472.099997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 8004989170.799978,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 8004284222.499985 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 8010777514.700032,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 8010039833.100001 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.2586327010000105,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.258480148999979 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.587545728999999,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.587044451000054 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 10.835983069999997,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.835226179000017 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 8.132400978000078,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.131269062000001 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.566602548999981,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.565023058999998 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}