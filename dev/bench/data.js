window.BENCHMARK_DATA = {
  "lastUpdate": 1784318926673,
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
          "id": "759866c365316dff19c319872d7e91e7a2c71acf",
          "message": "build: fix requires-python to permit >=3.9 (#699)",
          "timestamp": "2026-07-17T16:51:44Z",
          "tree_id": "d0f78b39dba95cf59331c67c5c09342c0db8c6a0",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/759866c365316dff19c319872d7e91e7a2c71acf"
        },
        "date": 1784318925411,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4644494132.899996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4610194133.6 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1337.5613290000008,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1337.393534666667 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 4240.156931000001,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 4239.688481333334 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 15450.680499333335,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 15448.233808333332 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4108975263.6999946,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4107997404.499995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7239423648.1,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7238507119.699997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7235374966.899984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7234494889.70001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7229343694.100032,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7228615061.4 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 1.2137304699999731,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.213674477999973 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 5.4179868140000735,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.417565945000035 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 11.425640208999994,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.424825458000043 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.942974910999965,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.9423155000000065 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 11.465691160000006,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 11.46456691200001 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}