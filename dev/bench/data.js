window.BENCHMARK_DATA = {
  "lastUpdate": 1786476947765,
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
          "id": "8d62f4169ea912b02c89eae9f19886f7dee314e0",
          "message": "test: make frame-sensitive expectations robust to time conversion resolution (#706)\n\n* test: make frame-sensitive expectations robust to time conversion resolution\n\nostk-physics #378 computes Julian dates arithmetically from Instant's internal\nnanosecond count rather than round-tripping through a calendar date and a\n~2.4e6-magnitude double, and has the CIRF/TIRF/ITRF providers use those accessors\ndirectly. It had to be reverted in #390 because it broke tests here.\n\nThat change removes ~20-40 us of quantization noise from the Modified Julian Date\nfeeding the Earth Rotation Angle, which shifts every Earth-fixed frame\ntransformation by up to a few millimeters and re-rolls the floating-point noise of\npropagations whose force model is evaluated in the Earth-fixed frame. Four\nexpectations were pinned tightly enough, or conditioned poorly enough, to notice:\n\n- Orbit.GeoSynchronous compares against reference states aligned with a geodetic\n  longitude, so they move with the Earth Rotation Angle (3.9 mm). The tolerance was\n  the print precision of those values; it is now 1 cm, which at geosynchronous\n  radius is still a 2.4e-10 rad longitude error.\n- CrossValidation.ForceModel_TwoBody bounds the GCRF difference with the STK\n  reference on the round-off floor of the propagation rather than on any physical\n  difference. Observed maxima are now 6.8e-7 m and 7.2e-10 m/s, so the bounds are\n  raised an order of magnitude to 5e-6 m and 5e-9 m/s.\n- test_cartesian_position and test_cartesian_velocity hard-coded the ITRF\n  coordinates at J2000 with rel=1e-14. They now obtain the expected coordinates\n  through the equivalent Position / Velocity transformation, as the C++ tests do.\n- test_tle_solver asserted termination on the RMS update criterion while fitting an\n  11-observation, 14.5-minute arc from the orbit determination dataset. Over an arc\n  that short the fit reaches the accuracy floor imposed by the quantization of the\n  TLE strings after 5 iterations and then limit-cycles around it with an RMS update\n  of 1.0 to 5.3 m, so meeting the 1 m threshold was a coincidence. The fixture now\n  uses the same dataset and 12-hour fit span as the C++ tests, where the RMS update\n  drops to 1e-3 m.\n\nVerified against both the re-landed physics change (C++ 1104 passed / 2 pre-existing\nskips, Python 839 passed) and the released ostk-physics 14.1.1.\n\nCo-Authored-By: Claude Opus 5 (1M context) <noreply@anthropic.com>\n\n* chore: clean up comments\n\n* chore: format\n\n---------\n\nCo-authored-by: Claude Opus 5 (1M context) <noreply@anthropic.com>",
          "timestamp": "2026-08-11T18:56:26Z",
          "tree_id": "02b9eb58a842dabd37eb6e1cf8ffaccd1be5e79f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/8d62f4169ea912b02c89eae9f19886f7dee314e0"
        },
        "date": 1786476945907,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 3139111128.2999873,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3043541540.299999 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 957.7542553333274,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 957.6796106666662 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2950.5198690000047,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2950.285750666666 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 11098.910467666676,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 11098.135549333332 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2908546149.39999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2908353420.399997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 5338262127.599989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5337928220.400003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5390771577.500005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5390347721.800001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5384494491.999999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5383722171.700001 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.8892882999999756,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.8891945780000015 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 4.5117450370000824,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.511228436999943 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 9.811082297999974,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 9.810185443000023 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 6.828759563000062,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.828119569000023 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 10.195877306000057,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.195028147999949 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}