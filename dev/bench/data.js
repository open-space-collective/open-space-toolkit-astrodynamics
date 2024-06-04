window.BENCHMARK_DATA = {
  "lastUpdate": 1717530562381,
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
          "id": "d5a301ae72d7e104d22ce374848492415dc2850c",
          "message": "chore: sort 'using' declarations lexographically (#404)",
          "timestamp": "2024-06-04T12:32:10-07:00",
          "tree_id": "f49f18ba87201af08d371e635706abb6b57f4e34",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/d5a301ae72d7e104d22ce374848492415dc2850c"
        },
        "date": 1717530559991,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 554890747.799999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 554856277.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 797821388.3000023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 797772786.1999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2782605411.1000075,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782489029.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2781957598.9999976,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781839705.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2782593601.399992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782487935.3 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}