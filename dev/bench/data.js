window.BENCHMARK_DATA = {
  "lastUpdate": 1747831219769,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
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
          "id": "fc8bfdfb7c67363972a7a70a12a053c2c6484199",
          "message": "docs: include trajectory.model (#564)",
          "timestamp": "2025-05-21T14:19:57+02:00",
          "tree_id": "ad334271e9710a75660fc66ca5243e723e7af949",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/fc8bfdfb7c67363972a7a70a12a053c2c6484199"
        },
        "date": 1747831218008,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 544605601.8000035,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 544536471.6999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 882779109.6000055,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 882409148.7000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3189290037.8000035,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3189029426.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3186903413.899995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186691433.7999983 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3186861893.499997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186689348 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}