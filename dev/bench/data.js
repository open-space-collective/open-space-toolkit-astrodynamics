window.BENCHMARK_DATA = {
  "lastUpdate": 1717430415340,
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
          "id": "b57043e58528e5ba7847ee591e5ac44637e5425f",
          "message": "refactor: remove unused forward declaration COE (#401)",
          "timestamp": "2024-06-03T15:43:17Z",
          "tree_id": "b20f60333d4be0a762b1032ca46e513ece3c6752",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b57043e58528e5ba7847ee591e5ac44637e5425f"
        },
        "date": 1717430413515,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 555991874.2000036,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 555968157 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796271938.2000046,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796242111.7999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2823842698.399983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2823759369.299999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2819185813.400003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2819100382.4000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2818511635.3000126,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2818453765.200003 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}