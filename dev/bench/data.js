window.BENCHMARK_DATA = {
  "lastUpdate": 1768384421377,
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
          "id": "419fbe94986955601f0e41115552c81a297d76e2",
          "message": "feat: support state builder expansion and reduction for any reference frame (#623)\n\n* fix: state builder reduce should work for different frames\n\n* feat: address feedback\n\n* feat: address feedback",
          "timestamp": "2025-12-19T01:15:14Z",
          "tree_id": "504e0b05cc598659bc3ec08e16b87aec8c544a0d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/419fbe94986955601f0e41115552c81a297d76e2"
        },
        "date": 1766108511987,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 3479005126.2999954,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3445086941.3000007 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 3241337005.600002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3241061443.8999987 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6034200178.099991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6033625934.199999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6035972234.599996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6035468267.599999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6030945330.7000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6030459979.8 ns\nthreads: 1"
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
          "id": "cfdbcac4d3b1c507c552f3e42810c3d220671607",
          "message": "docs: update close approach timestep recommendation (#626)\n\n* docs: update close approach timestep recommendation\n\n* fix: apply suggestions\n\n* fix: apply suggestions",
          "timestamp": "2025-12-21T17:34:44Z",
          "tree_id": "d5bcff7bddfe8552b49128f0b59274f7a4f37516",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/cfdbcac4d3b1c507c552f3e42810c3d220671607"
        },
        "date": 1766340018874,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4729154104.399959,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4673446344.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4097602912.799971,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4097218197.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7208867384.500025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7208086328 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7207280741.800003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7206661296.600001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7198578813.600023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7198014534.500002 ns\nthreads: 1"
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
          "id": "b79dc651cab97fb533ca0edc68ee1d763c7b3307",
          "message": "fix: maneuver segments not solved precisely at provided intervals (#629)",
          "timestamp": "2026-01-09T07:20:44Z",
          "tree_id": "76bede80c8cf90392e5d0e6a828ac66961f2c738",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b79dc651cab97fb533ca0edc68ee1d763c7b3307"
        },
        "date": 1767944983085,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4697066850.599998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4672829537.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4071187192.8999987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4070872960.8000016 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7182159216.099996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7181704848.900004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7180572903.899997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7180023378.000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7179748699.099991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7179232809.400006 ns\nthreads: 1"
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
          "id": "950bbf2453557a0883343d18d6e4838bb99d9ac7",
          "message": "feat: expose QLaw effectivity computation and convergence thresholds (#631)\n\n* feat: add public method to compute QLaw sensitivity\n\n* Apply suggestions from code review\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n* fix: tests\n\n* chore: format\n\n* refactor: apply MR suggestion\n\n* feat: expose control weight\n\n* docs: address typo\n\n* refactor: remove unnecessary import\n\n---------\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-01-14T09:26:17Z",
          "tree_id": "700b922f5aaabc5a832fc256c3694e7633bcb6ac",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/950bbf2453557a0883343d18d6e4838bb99d9ac7"
        },
        "date": 1768384419849,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4701812488.7,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4672880667.199999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4077822087.4,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4077286470.3000007 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7196459948.6,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7195742586.799997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7195631642.299997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7194565025 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7190262815.80002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7189685892.100004 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}