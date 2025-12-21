window.BENCHMARK_DATA = {
  "lastUpdate": 1766340021467,
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
      }
    ]
  }
}