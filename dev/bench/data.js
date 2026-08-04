window.BENCHMARK_DATA = {
  "lastUpdate": 1785856254991,
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
          "id": "c040ed1ec0ac538b48ce159fe95b96d229766b33",
          "message": "chore: remove unneeded locale config (#705)\n\n* tmp: test new image\n\n* ci: add ref\n\n* build: remove locale override\n\n* chore: revert temp refs",
          "timestamp": "2026-08-03T11:14:40-07:00",
          "tree_id": "ed4aebf2075da4b4614ecac567cf84b9b0be0b8b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/c040ed1ec0ac538b48ce159fe95b96d229766b33"
        },
        "date": 1785856253507,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4796250597.000006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4622524687.5 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1348.8203350000088,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1348.6703120000006 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 4280.559343666671,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 4280.000952000001 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 15614.378917000011,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 15612.560988666666 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4166102026.900012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4165575056.4000044 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7276489107.300016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7275168229.599998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7364027516.600016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7363007912.70001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7270540976.4999895,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7269454261.400006 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.2238963959998728,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.223711642000012 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.408080702999996,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.407276337999974 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.656612812000049,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.654639420000024 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 8.418412169000021,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.415556234999997 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.58679255200002,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.585030324000002 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}