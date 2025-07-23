window.BENCHMARK_DATA = {
  "lastUpdate": 1753312775906,
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
          "id": "73ba1bd1cf77260cb197528542cb93c6765b4c4a",
          "message": "fix: negative eccentricity (#569)\n\n* wip:\n\n* feat: fix\n\n* fix: tests\n\n* fix: tests\n\n* feat: use ModifiedEquinoctial in conversions\n\n* feat: address remaining feedback\n\n* fix: teme frame\n\n* feat: address remaining feedback\n\n* chore: style",
          "timestamp": "2025-07-22T18:32:13-05:00",
          "tree_id": "bd80043ab30c03ec517a2f6c7735cd37caa0aee5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/73ba1bd1cf77260cb197528542cb93c6765b4c4a"
        },
        "date": 1753228648422,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 791369022.7999837,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 774300613.1 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 885739608.3000027,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 885561568.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3193120668.6000166,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192862995.7999983 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3198160073.200006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197934110.000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3192217174.6000004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192019867.8 ns\nthreads: 1"
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
          "id": "cf5772afdf62f06d7be8493a564ec507ad83205a",
          "message": "revert: \"feat: add COE getCartesianState frame check (#571)\" (#574)\n\nfeat: Revert \"feat: add COE getCartesianState frame check (#571)\"\n\nThis reverts commit 578d85b045d62fa62c99429a482bd92a9454bfdf.",
          "timestamp": "2025-07-23T17:54:23-05:00",
          "tree_id": "f43f61956bf699f82f55bb97ab1befc59ddfbc92",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/cf5772afdf62f06d7be8493a564ec507ad83205a"
        },
        "date": 1753312774288,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 795780940.6999956,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 769183909.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 885144009.1000086,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 885073147.7000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3193267017.7000093,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192966134.9000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3194458238.7000027,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194233288.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3197792174.300002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197456814.400003 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}