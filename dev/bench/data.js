window.BENCHMARK_DATA = {
  "lastUpdate": 1739443816062,
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
          "id": "7af3fb66c44339d705365bb4c477b6231f138b97",
          "message": "fix: skip importing coordinates that are none in State.from_dict (#520)\n\nfeat: skip importing coordinates that are none in State.from_dict",
          "timestamp": "2025-02-13T11:29:42+01:00",
          "tree_id": "3f05f6211cb36d7d4279581feadb00b83d6a1610",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/7af3fb66c44339d705365bb4c477b6231f138b97"
        },
        "date": 1739443813690,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 521251046.3000058,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 521230528.7000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 853049594.4999983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 853006335.5000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3092566845.1000032,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3092455513.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3095176608.0999956,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3094970270.100001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3087759828.6000023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3087653098.5000005 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}