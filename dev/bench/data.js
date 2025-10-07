window.BENCHMARK_DATA = {
  "lastUpdate": 1759862094821,
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
      }
    ]
  }
}