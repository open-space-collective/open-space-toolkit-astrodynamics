window.BENCHMARK_DATA = {
  "lastUpdate": 1782854899033,
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
          "id": "72e49717c9bcee706b005953482e40677be99cac",
          "message": "feat: add output frame option for Tabulated models (#688)\n\n* feat: add output frame option to tabulated model\n\nAdd an optional output reference frame to Trajectory::Model::Tabulated (default GCRF, backward compatible). Interpolation is still performed in the frame of the provided states; only the expressed frame changes.\n\nImplemented as overloads rather than defaulted parameters to preserve ABI: the original 2-arg constructors and 1-arg Default() keep their mangled symbols, with frame-aware variants added alongside (delegating to the originals). Python bindings, C++ and Python tests included.\n\nCo-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>\n\n* fix: generator output frames\n\nExpress both positions in a common frame (ITRF) before differencing in FindTimeOfClosestApproach. It previously subtracted from/to coordinates without frame alignment, silently assuming every model returns GCRF, which gives a wrong TCA for a non-GCRF to-trajectory (e.g. an ITRF tabulated model). Range is frame-invariant, so all-GCRF results are unchanged.\n\nCo-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>\n\n* perf: hoist state filter out of the access generation loop\n\ncomputeAccessesForFixedTargets called Generator::getStateFilter() once per step (and once per target), each call copying the std::function by value and re-running isDefined(). Bind a reference to the member once before the loop instead. Behavior is unchanged.\n\nCo-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>\n\n* perf: transform only position when checking visibility\n\nThe access-generation hot paths only need the satellite/target position, but called state.inFrame(ITRF) which transforms the entire state (position, velocity and any other subsets; the velocity transform also applies the angular-velocity transport term). Extract the position first and transform that alone (position.inFrame(ITRF, instant)). Applied in the coarse grid loop, the precise-crossing conditions and CalculateElevationAt. Results are unchanged.\n\nCo-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>\n\n* perf: hoist per-evaluation work out of the trajectory-target condition\n\ngetConditionFunction returned a condition that, on every solver evaluation, performed a string-keyed Environment::accessCelestialObjectWithName(\"Earth\") lookup and copied the state filter std::function twice (once to test, once to call). Resolve the Earth pointer and bind the state filter once before building the lambda and capture them. Behavior is unchanged. (The per-evaluation NED frame in CalculateAer is inherent to a moving observer and is left as-is.)\n\nCo-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>\n\n* test: add tabulated access-generation benchmarks; keep SEZ loop\n\nAdds benchmarks comparing the tabulated model's output frame (GCRF vs ITRF) for access generation, plus a multi-target AERInterval benchmark (10/100 fixed targets) that isolates the per-step AER computation.\n\nThe latter was used to evaluate vectorizing the per-target SEZ rotation: at 10 targets the vectorized variant was ~7% slower and at 100 it was a wash, because the per-target cost is dominated by the azimuth/elevation trig, not the 3x3 matmul. The contiguous-block loop is therefore retained, with a comment recording the measurement.\n\nCo-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>\n\n* refactor: some methods\n\n* feat: add benchmark\n\n* feat: add validation\n\n* feat: address feedback\n\n---------\n\nCo-authored-by: Claude Opus 4.8 (1M context) <noreply@anthropic.com>",
          "timestamp": "2026-06-30T13:57:16-07:00",
          "tree_id": "5f5a73c7b23687e29d780dc1b3e663224134c745",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/72e49717c9bcee706b005953482e40677be99cac"
        },
        "date": 1782854896177,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4770811326.700005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4591253997.499999 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1330.7210980000075,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1330.5950743333335 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 4218.481010666665,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 4218.142840999998 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 15402.618087666668,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 15401.365264333332 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4082323898.5999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4081975397.2000046 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7207886992.600004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7207092528.099997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7210731881.100003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7209802761.599994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7210447811.900007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7209847217.200007 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.199630017000004,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.1995393830000012 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.345108440999979,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.3446825639999815 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.207679993999932,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.204907542 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.7350300419999485,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.734410996999998 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.323297362999938,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.322215344000085 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}