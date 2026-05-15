window.BENCHMARK_DATA = {
  "lastUpdate": 1778839596684,
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
          "id": "e713106c962b7c1de0686c953a9ca00a09b9aa91",
          "message": "feat: generalize condition based solving (#628)\n\n* feat: generalize condition based solving\n\n* chore: format\n\n* feat: make backwards compatible\n\n* feat: use templates to simplify\n\n* wip:\n\n* fix: tests pass\n\n* wip:\n\n* feat: adapt from rebase\n\n* feat: it works...\n\n* feat: update tests and add support for different steppers\n\n* fix: overstepping issues\n\n* wip:\n\n* feat: address Pau's feedback\n\n* feat: add missing tests\n\n* feat: simplify by using RKDP5 with dense step always\n\n* chore: minor cleanup",
          "timestamp": "2026-05-14T09:16:24-07:00",
          "tree_id": "301c2af062032fff178d32dd547e72f7a67706f2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e713106c962b7c1de0686c953a9ca00a09b9aa91"
        },
        "date": 1778777613922,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4884771208.4000025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4865121303.800001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4488926327.099995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4488374715.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7990698585.599996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7989496106.299997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7991290348.399991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7990285859.699997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7992335210.100019,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7991241528.500002 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.2414558489999763,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.2412658089999695 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.243680017000088,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.242938973000037 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 10.730084073999933,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.727720723999994 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.894118215999924,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.89304145899996 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.361675588999901,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.360187917999951 s\nthreads: 1"
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
          "id": "33b384539aadd4d7d8991a65dc0e22fcd0dd8340",
          "message": "fix: validation tests (#675)\n\n* fix: validation tests\n\n* Apply suggestions from code review\n\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2026-05-15T07:35:13Z",
          "tree_id": "74348f49d11577f5b7643f3fc5a87393c81a7838",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/33b384539aadd4d7d8991a65dc0e22fcd0dd8340"
        },
        "date": 1778839594695,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4636690030.400002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4592245176.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4082842424.300003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4082464077.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7201263586.799991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7200396722.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7202298787.699987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7201623133.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7205157324.199979,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7204441426 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.2073878399999103,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.2072525670000118 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.3950684189999265,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.39465670200002 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.184107816999926,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.183246817999986 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 8.091395940000098,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.090789223000058 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.256686592999927,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.25561039400003 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}