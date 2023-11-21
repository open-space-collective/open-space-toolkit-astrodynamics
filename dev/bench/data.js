window.BENCHMARK_DATA = {
  "lastUpdate": 1700532352274,
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
          "id": "f477d29706fd665b20da39b5dd96698149058d4e",
          "message": "feat: add periapsis scaling for QLaw and fix small bugs in oe_xx calculations (#270)\n\n* feat: add periapsis scaling for QLaw and fix some small bugs\r\n\r\n* feat: works well end to end\r\n\r\n* feat: fix style\r\n\r\n* feat: fix tests\r\n\r\n* feat: add more tests\r\n\r\n* chore: remove stdcout\r\n\r\n* feat: use Eigen map",
          "timestamp": "2023-11-13T18:21:35-08:00",
          "tree_id": "5464c02f3c5237746a62dbc3ac1e9aef6be2ce3e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f477d29706fd665b20da39b5dd96698149058d4e"
        },
        "date": 1699930573914,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 573823098.0999959,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 573769209.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 907453205.600001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 907386580 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4109037569.3,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4108166020.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4108809345.6999955,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4108231720.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4109015670.1000185,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4108319360.000003 ns\nthreads: 1"
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
          "id": "a77605d7902f166ea9492d0ab1c25301f8e52fa2",
          "message": "feat: scale mass flow rate by thrust percentage (#273)\n\n* feat: scale mass flow rate by thrust percentage\r\n\r\n* feat: fix test",
          "timestamp": "2023-11-18T11:06:13+01:00",
          "tree_id": "e8d717017456874c378718c0ce99abcb6ca6c9c7",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/a77605d7902f166ea9492d0ab1c25301f8e52fa2"
        },
        "date": 1700303672816,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 547128160.1000044,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 547112089.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792770954.3000048,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792755800 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2787819689.0000086,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787620819.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2787486525.399993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787406420.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2786871421.0999937,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786497199.9999995 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "aaed350023c011567408b62b644864bf069fa98f",
          "message": "fix: addSegment method on Sequence (#275)\n\n* fix: remove overload on addSegment for Sequence and use addSegments for array input\r\n\r\n* test: adapt cpp and python tests",
          "timestamp": "2023-11-20T12:05:34-10:00",
          "tree_id": "4c3a3de640a3c1fef8a03afb6552cba4441bd986",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/aaed350023c011567408b62b644864bf069fa98f"
        },
        "date": 1700519641789,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 563761722.000001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 563744159.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792341868.5000002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792321100.0000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2783766948.699997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783711930.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2785576161.500006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2785525870.000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2783580487.499995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783431920.000004 ns\nthreads: 1"
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
          "id": "78b367857be52de43b3945cd9fa4a703e3ac1167",
          "message": "fix: unit test for Case E (#276)\n\nchore: skip Case_E test until functional",
          "timestamp": "2023-11-20T15:38:55-10:00",
          "tree_id": "30529016133cbfa81e47b218c1de0d59d02e9832",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/78b367857be52de43b3945cd9fa4a703e3ac1167"
        },
        "date": 1700532351007,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 554972925.6999853,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 554950390 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 790533229.1999968,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 790506810 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2790085661.8000093,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789979870.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2797712965.699992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2797589910.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2790570289.8000073,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2790442749.9999986 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}