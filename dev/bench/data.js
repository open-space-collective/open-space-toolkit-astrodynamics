window.BENCHMARK_DATA = {
  "lastUpdate": 1793304203811,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
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
          "id": "01003e44b8f8276907c03ebf57a009b79353a2e3",
          "message": "fix: guard temporal condition solver against misuse (#743)\n\n* fix: guard temporal condition solver against misuse\n\n* fix: address pau's feedback and dont raise",
          "timestamp": "2026-10-29T12:29:47-07:00",
          "tree_id": "816bf8ff5da5b333ca561466a36543e146d10e8c",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/01003e44b8f8276907c03ebf57a009b79353a2e3"
        },
        "date": 1793304202096,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 1894134899.9000068,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1814686522 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 508.2506530000046,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 508.203989333334 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1922.071354666684,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1921.8382016666667 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 172.4460126666827,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 172.42801766666813 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2008324948.7000103,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2008141670.1000016 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4701597582.299991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4701179806.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4703608293.90001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4703210740.599996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4703044117.800028,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4702662200.299997 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.5606813700001112,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.5606504069999971 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 1.9153209159999278,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.9151399499999968 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 2.0614999189999708,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.061341228000032 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 2.6897152679999863,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.689457061999974 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 6.586582840999995,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.586061685999994 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 5.577909206999948,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.577078219000015 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 5.565082820000043,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.564610081000012 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 86.31825371599996,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 86.31033120100003 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.025949562999926457,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.02594819300003337 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 23.614250278235716,
            "unit": "us/iter",
            "extra": "iterations: 29651\ncpu: 23.612591042461464 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}