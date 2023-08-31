window.BENCHMARK_DATA = {
  "lastUpdate": 1693488351180,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "43561754+Derollez@users.noreply.github.com",
            "name": "Remy Derollez",
            "username": "Derollez"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "0e087f60d00d4b5d48f5ef96554d2ad9b634cccc",
          "message": "docs: update notebook for orbit propagation using propagator (#216)\n\nfeat: update notebook for propagation using numerical solver",
          "timestamp": "2023-08-31T14:50:08+02:00",
          "tree_id": "079e5a5edfa968a806555d64905a8bfc917cc397",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/0e087f60d00d4b5d48f5ef96554d2ad9b634cccc"
        },
        "date": 1693488349592,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 958234315.8000128,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 958117510 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1907768617.799991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1907491260.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4978000660.900011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4976429120 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4986565567.7999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4985432070 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5026995201.799992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5025911360.000004 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}