window.BENCHMARK_DATA = {
  "lastUpdate": 1753701290533,
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
          "id": "e0c749502ffec7032507b8190441069502aaec6a",
          "message": "perf: improve eclipse generator performance (#575)\n\n* perf: improve eclipse generator performance\n\n* fix: format",
          "timestamp": "2025-07-28T10:48:13Z",
          "tree_id": "00c0ca4fad50d7485176c11f8f87076c13ecb40b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e0c749502ffec7032507b8190441069502aaec6a"
        },
        "date": 1753701288824,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 800672115.2000182,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 775278413.1000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 885364321.9000105,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 885314338.2999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3193283941.0000157,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193129623.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3192787006.400033,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192624288.6999965 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3191732301.3000097,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3191523855.1999993 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}