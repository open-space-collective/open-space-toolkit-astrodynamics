window.BENCHMARK_DATA = {
  "lastUpdate": 1759868827428,
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
          "id": "8d9201a9a6cdf96aae597ee951f27a11ecc53ece",
          "message": "fix: allow overriding of 'calculate_thrust_acceleration_at' function … (#590)\n\n* fix: allow overriding of 'calculate_thrust_acceleration_at' function in Guidance Law trampoline class\n\n* test: add test coverage\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2025-10-07T18:00:18Z",
          "tree_id": "9386c911d358ddf0a42465dfeabc95f2ee4e00d7",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/8d9201a9a6cdf96aae597ee951f27a11ecc53ece"
        },
        "date": 1759862093174,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 807583262.7999886,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 790922972.5999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 891833768.2999947,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 891579834.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3202062395.200005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3201857138.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3202589088.199994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3202331351.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3200147621.2,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199906462.3999987 ns\nthreads: 1"
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
          "id": "c9bdd47da87574f48a2ff60ac44ad00cbb981377",
          "message": "feat: add constructBodyFrame method to Profile (#589)\n\n* feat add constructBodyFrame method to Profile\n\n* feat: address feedback",
          "timestamp": "2025-10-07T20:05:48Z",
          "tree_id": "b7dbaf995e6e21fcd578a52475888f7462ef97e3",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/c9bdd47da87574f48a2ff60ac44ad00cbb981377"
        },
        "date": 1759868825828,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 805163346.5000066,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791024841.6999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 892988578.9000025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 892904435.3999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3207245232.0999956,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3206985431.6999993 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3206565169.4999986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3206186045.299999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3204153165.199983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3203960125.3999977 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}