window.BENCHMARK_DATA = {
  "lastUpdate": 1784637499405,
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
          "id": "16b3d6274cf087c14c4edbb1ce59cbc6c1548568",
          "message": "feat: add orbital element condition (#692)\n\n* feat: add generic OrbitalElementCondition event condition\n\nGeneralizes COECondition and BrouwerLyddaneMeanLongCondition into a\nsingle OrbitalElementCondition class parameterized by a Theory enum\n(Osculating, BrouwerLyddaneMeanShort, BrouwerLyddaneMeanLong), instead\nof having a separate class per orbital element theory.\n\nCOECondition and BrouwerLyddaneMeanLongCondition are marked deprecated\nin favor of the new class but kept for backwards compatibility.\n\nCo-Authored-By: Claude Sonnet 5 <noreply@anthropic.com>\n\n* refactor: remove construct functions\n\n---------\n\nCo-authored-by: Claude Sonnet 5 <noreply@anthropic.com>\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-07-21T12:06:47Z",
          "tree_id": "235e441cc71679f9f6407361d7091a9521c65720",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/16b3d6274cf087c14c4edbb1ce59cbc6c1548568"
        },
        "date": 1784637497459,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2654842124.800001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2533288825.9 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 799.7971190000044,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 799.7575000000007 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2473.880036333336,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2473.841499333332 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 9289.516941999997,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 9289.283883000002 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2443718659.7999983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2443665001.900001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4480553303.400001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4480474401.399998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4481114517.899994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4481142958.800009 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4478370027.000029,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4478418095.500001 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.7371299160000717,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.7371310579999886 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 3.7646320709999372,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 3.764654611000026 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 8.214123091000033,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.21416599700001 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 5.661183568000183,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.661189278999984 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 8.566788059000032,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.566802988999996 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}