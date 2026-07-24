window.BENCHMARK_DATA = {
  "lastUpdate": 1784925656639,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "49699333+dependabot[bot]@users.noreply.github.com",
            "name": "dependabot[bot]",
            "username": "dependabot[bot]"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "fc4a940f80a1668d8d08f492d3f9b3b351b813a4",
          "message": "build(deps): bump pygments from 2.19.2 to 2.20.0 in /bindings/python (#700)\n\nBumps [pygments](https://github.com/pygments/pygments) from 2.19.2 to 2.20.0.\n- [Release notes](https://github.com/pygments/pygments/releases)\n- [Changelog](https://github.com/pygments/pygments/blob/master/CHANGES)\n- [Commits](https://github.com/pygments/pygments/compare/2.19.2...2.20.0)\n\n---\nupdated-dependencies:\n- dependency-name: pygments\n  dependency-version: 2.20.0\n  dependency-type: indirect\n...\n\nSigned-off-by: dependabot[bot] <support@github.com>\nCo-authored-by: dependabot[bot] <49699333+dependabot[bot]@users.noreply.github.com>",
          "timestamp": "2026-07-24T20:09:31Z",
          "tree_id": "93a78e2c8e12a5129ac18c5827e733bb49df2632",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/fc4a940f80a1668d8d08f492d3f9b3b351b813a4"
        },
        "date": 1784925655057,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2606344184.7000093,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2570957717.3 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 814.2919573333339,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 814.2499903333338 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2503.632486000015,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2503.399604666666 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 9435.129491666676,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 9434.420282666668 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2479826980.1999936,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2479677589.499998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4516223156.20001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4515796286.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4510564421.899988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4510152871.499994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4517155543.80001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4517012314.300001 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.7608286930000077,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.7607773130000055 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 3.8325553339999487,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 3.832379675000027 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 8.349661175000051,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.349398375999982 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 5.766758537999976,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.766536402000014 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 8.71857361800005,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.71809993299999 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}