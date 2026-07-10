window.BENCHMARK_DATA = {
  "lastUpdate": 1783721370836,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "65405206+maxnqt@users.noreply.github.com",
            "name": "Max Niquet",
            "username": "maxnqt"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "89144ef4923718310ceb95f7d8722d4a8fdc2738",
          "message": "feat: add Alpha-5 support for TLE satellite numbers (#693)\n\n* feat: add Alpha-5 support for TLE satellite numbers\n\nSpace-Track encodes catalog IDs above 99,999 with a leading letter in\nthe 5-digit NORAD field (e.g. A5544 -> 105544). Add encode/decode for\nthe full 0-339,999 range, expose getSatelliteNumberString() for the raw\nTLE field, and substitute a numeric placeholder for libsgp4 propagation\nsince it rejects non-digit NORAD fields but does not use them in the math.\nhttps://www.space-track.org/documentation#tle-alpha5\n\n* lint: format files\n\n* fix: fix Alpha-5 TLE checksums in constructor test\n\n* test: test failure paths to fix code coverage regression\n\n* fix: fix call paths for tests (public, not private)\n\n* fix: ill-formatted test input, coverage\n\n* docs: add docstring explanation to getSatelliteNumber()\n\n* refactor: address Alpha-5 TLE review comments\n\nRename getSatelliteNumberString() -> getRawSatelliteNumber() (and the\nget_raw_satellite_number Python binding), document the Alpha-5 alphabet\n(I/O skipped, spec link), mirror the decoded-integer note into the Python\nget_satellite_number docstring, and fix a bad checksum in the Z9999 test.\n\n* refactor: tighten Alpha-5 TLE API surface per review\n\n* fix: format files\n\n* test: fix asserted types",
          "timestamp": "2026-07-10T14:40:02-07:00",
          "tree_id": "4192d5c9804a27aaf7b1c382dfa21e7e0e4a653c",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/89144ef4923718310ceb95f7d8722d4a8fdc2738"
        },
        "date": 1783721368273,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 3548697751.6999955,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3392026554.1 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1052.0932563333265,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1051.9626450000007 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 3303.269150999995,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 3302.890550333333 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 12290.635404,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 12289.362559666668 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 3242450904.8,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3241987857.499997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6075973577.599984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6074920952.299994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6079199625.499979,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6078598683.600004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6077199493.100011,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6076501878.899999 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.9708994079999229,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.970868887999984 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 4.719656400000076,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.7191134490000195 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 10.378766336000012,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.377650692999964 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 7.1655723550001085,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 7.164840480000009 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 10.153225788999976,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 10.152166227999999 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}