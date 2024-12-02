window.BENCHMARK_DATA = {
  "lastUpdate": 1733162068274,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "44425077+tapiab@users.noreply.github.com",
            "name": "Benoit Tapia",
            "username": "tapiab"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "298e57e92afcd13829c9e1fa8d545e1ac429aeeb",
          "message": "chore: allow build without Boost Stacktrace (#466)",
          "timestamp": "2024-12-02T09:33:42-08:00",
          "tree_id": "990404b639c56fdb0db7055aa3baafcfebf5fab2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/298e57e92afcd13829c9e1fa8d545e1ac429aeeb"
        },
        "date": 1733162065113,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 581983448.5999991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 581955621.6999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 879448204.0999867,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 879420719.9999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3186210979.7000017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186072879.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3187003595.7999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186860106.100003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3185987643.0999804,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3185884707.400001 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}