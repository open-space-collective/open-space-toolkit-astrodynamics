window.BENCHMARK_DATA = {
  "lastUpdate": 1761574656480,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "65550121+phc1990@users.noreply.github.com",
            "name": "Pau Hebrero",
            "username": "phc1990"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "26fe93fcf51a1d123a4792de5c87cd84a3dad413",
          "message": "feat: add constant-local-orbital-frame-direction Maneuvering Segments (#591)\n\n* feat: add maneuvering segments with constant local orbital frame direction\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2025-10-21T14:26:11Z",
          "tree_id": "8aa03523b59c47d22aa8dcd8b816bf5324a737f3",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/26fe93fcf51a1d123a4792de5c87cd84a3dad413"
        },
        "date": 1761063284319,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 883478336.4000032,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 808977992 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 895235277.0000004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 895066287.6000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3204580287.4999933,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204260451.9000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3203231282.0999894,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3202971396.2000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3202889047.9999857,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3202624813.999999 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "65550121+phc1990@users.noreply.github.com",
            "name": "Pau Hebrero",
            "username": "phc1990"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "6703a3f04c9f1a859ea2e0b468d128212a3e3715",
          "message": "feat: add Close Approach module (#593)\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2025-10-27T13:55:40Z",
          "tree_id": "38c06ec579c77b6b0024daf59db810f580aa39bb",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6703a3f04c9f1a859ea2e0b468d128212a3e3715"
        },
        "date": 1761574654003,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 964571426.6000084,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 811320158.5999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 897772990.0999918,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 897570572.0000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3212752077.5999985,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3212347147.2999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3214202489.7000055,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3213852876.1999984 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3214134149.8000145,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3213805192.499997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}