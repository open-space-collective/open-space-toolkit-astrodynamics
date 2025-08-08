window.BENCHMARK_DATA = {
  "lastUpdate": 1754644174590,
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
          "id": "6746b6d8652092647892c39c81952dc85c4fe7a1",
          "message": "fix: access generator stepping outside analysis interval (#580)\n\n* fix: access generator stepping outside analysis interval\n\n* fix: last step logic",
          "timestamp": "2025-08-06T18:55:41Z",
          "tree_id": "5572eda9a8c1617e338fb6b5da6e7d5471b78675",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6746b6d8652092647892c39c81952dc85c4fe7a1"
        },
        "date": 1754508111559,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 789634707.100015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 773818380.5000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 891851612.4000008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 891796702.7999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3205102871.700001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204925157.6000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3208807346.7000017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3208487385.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3208406162.7000093,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3208092519.3 ns\nthreads: 1"
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
          "id": "caff45ecb252984addc552cada77323b14b25fbe",
          "message": "fix: improve logging for TLE constructor (#581)",
          "timestamp": "2025-08-06T21:11:30Z",
          "tree_id": "655374db9cad8b9fb6a20386c2b28be80a4cb159",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/caff45ecb252984addc552cada77323b14b25fbe"
        },
        "date": 1754516256413,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 795780332.3000008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 780140323.6000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 890594240.0999947,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 890479670.9999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3198211040.4999957,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197961569.7999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3200910437.099992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3200692626.600001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3195162412.799999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194954397.6000032 ns\nthreads: 1"
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
          "id": "ce9b0a7f31e01f471b01468930362ba7848e7f3a",
          "message": "fix: make FiniteDifferenceSolver gradient behaviour consistent (#579)\n\n* fix: make finitedifferencesolver gradient behaviour consistent\n\n* test: clarify test elements",
          "timestamp": "2025-08-08T08:42:35Z",
          "tree_id": "116dbdff761a15ab642f7511cd8daf3ee0f8334f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ce9b0a7f31e01f471b01468930362ba7848e7f3a"
        },
        "date": 1754644172829,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 927092180.1000042,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 797795697.4000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 893692544.5000042,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 893516903.8999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3195509874.6999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3195261359.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3194753225.300008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194534666.700003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3193520981.199987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193304053.2999997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}