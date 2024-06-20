window.BENCHMARK_DATA = {
  "lastUpdate": 1718845972846,
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
          "id": "6155a31ba76509b53b9a3067ecfc8899cbce3ede",
          "message": "docs: cross reference ostk core, mathematics (#416)",
          "timestamp": "2024-06-19T12:06:37-07:00",
          "tree_id": "fca68e4066cc2f7ed06d18939915fc9b5c68f3ca",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6155a31ba76509b53b9a3067ecfc8899cbce3ede"
        },
        "date": 1718845970924,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 539695103.8999987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 539661818.4000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791042373.099998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 790993480.5000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2789702250.100015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789522631.9000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789229636.800008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789046874.399998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2788875289.3999934,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788752904.2000012 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}