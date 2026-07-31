window.BENCHMARK_DATA = {
  "lastUpdate": 1785505637675,
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
      },
      {
        "commit": {
          "author": {
            "email": "98616558+apaletta3@users.noreply.github.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e12c6b67461de2834e4ea6b82ff15480fe217031",
          "message": "feat: add output frame option for tabulated orbit models (#703)",
          "timestamp": "2026-07-31T07:12:30-06:00",
          "tree_id": "18e62c18c344d246c91104dcfe9fd657fe6027cc",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e12c6b67461de2834e4ea6b82ff15480fe217031"
        },
        "date": 1785505635786,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4602914388.300008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4551890659.999999 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1318.0906746666778,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1317.9109913333325 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 4181.411871333353,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 4180.894000333335 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 15223.893560666664,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 15222.388178333333 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4062930673.799997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4062547438.8000016 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7175116163.000029,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7174399021.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7171068905.899983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7170281985.700002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7179272120.7,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7178577423.599996 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.1976912650000031,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.1976266990000681 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.402810815000066,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.402247160999991 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.117209295000066,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.116104508000035 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.830930558999967,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.8303245860000175 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.234403114999964,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.233377511999947 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}