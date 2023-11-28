window.BENCHMARK_DATA = {
  "lastUpdate": 1701177365043,
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
          "id": "8e56e922b49f03d66557d0790a74fe2362875a8a",
          "message": "feat: add tests for QLaw targeting from SSO (#279)",
          "timestamp": "2023-11-21T09:07:03-08:00",
          "tree_id": "39dc29690268b030e125b7e5ec444724468a4d6e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/8e56e922b49f03d66557d0790a74fe2362875a8a"
        },
        "date": 1700588258222,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 560293311.1000026,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 560261779.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 795764414.3999971,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 795713949.9999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2789083025.300002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788946870 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2780771439.2000093,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2780657560.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2782298456.4999843,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781888160.000001 ns\nthreads: 1"
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
          "id": "f63ee086e047b51306c5b240875c374b85be0d50",
          "message": "chore: cache mass flow rate (#277)\n\n* chore: cache mass flow rate\r\n\r\n* feat: fix test",
          "timestamp": "2023-11-21T10:30:13-08:00",
          "tree_id": "4a0b2f2c1635ff4343b78b8fdd49836232e29d3d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f63ee086e047b51306c5b240875c374b85be0d50"
        },
        "date": 1700593002401,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 559612743.6000074,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 559580400 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 799510882.8000014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 799476880.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2789344109.6999936,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789232969.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2786993787.9000077,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786841599.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2789037410.5999966,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788947240.0000005 ns\nthreads: 1"
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
          "id": "233b46946940ad07c597c3e4ee096821367cfca2",
          "message": "docs: update python documentation with recommendations and a notebook (#280)\n\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2023-11-22T00:11:01Z",
          "tree_id": "1af0a9e043b29bd141092fec6c07349ee53cd049",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/233b46946940ad07c597c3e4ee096821367cfca2"
        },
        "date": 1700613485334,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 552382828.3000028,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 552349579.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 789461064.2000031,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 789433840 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2786028454.9,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2785930050.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2786413423.4999967,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786027929.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2792352372.3000073,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2791945549.9999995 ns\nthreads: 1"
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
          "id": "020abdf31f46083cc766ffc627b20516ec6eeb42",
          "message": "chore: align repo with plural/singular function args (#278)",
          "timestamp": "2023-11-27T09:39:20-08:00",
          "tree_id": "284359acf7998ed3d7651067974974158450b28b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/020abdf31f46083cc766ffc627b20516ec6eeb42"
        },
        "date": 1701108390947,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 556267531.3999989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 556250330 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794699367.6000148,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794660650.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2793709235.9999895,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2793623489.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2806265346.8000007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2806188169.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2795420539.69999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2795330749.999999 ns\nthreads: 1"
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
          "id": "93ab772dfbd9fd95a4e296cf369d42a241cb5b27",
          "message": "chore: improve-makefile-for-jupyter-development (#282)\n\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2023-11-27T18:13:48Z",
          "tree_id": "85545d6a2b13bd798bd6b6f1ae4f7c4925701629",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/93ab772dfbd9fd95a4e296cf369d42a241cb5b27"
        },
        "date": 1701110472668,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 551815379.2999953,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 551784239.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 788319918.3000045,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 788277760.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2785411171.700008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784977619.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2787372703.9000072,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787114200 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2789186477.0999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788901730.000002 ns\nthreads: 1"
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
          "id": "2ffa10e7a98626dddadfd70238cc8f4973218652",
          "message": "refactor: namespace derived structs within parents for python bindings (#285)\n\n* feat: namespace derived structs within parents for python bindings\r\n\r\n* feat: fix tests and naming",
          "timestamp": "2023-11-28T13:49:08+01:00",
          "tree_id": "665a90ad7b12421321ca284333330356d377a20d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/2ffa10e7a98626dddadfd70238cc8f4973218652"
        },
        "date": 1701177363968,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 566938677.1000063,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 566913560 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 801632047.6000004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 801576430.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2785244699.600005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2785092430.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2785762990.1999984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2785673880.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2788527403.300003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788420739.999999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}