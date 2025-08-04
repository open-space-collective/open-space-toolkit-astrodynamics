window.BENCHMARK_DATA = {
  "lastUpdate": 1754331322027,
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
          "id": "bf9b9240e3cb6c0b2e02bccc47aaf6f209721d81",
          "message": "feat: add LDTN computation to COE (#578)\n\n* feat: add LDTN computation to COE\n\n* Apply suggestion from @apaletta3\n\n* Apply suggestion from @coderabbitai[bot]\n\nCo-authored-by: coderabbitai[bot] <136622811+coderabbitai[bot]@users.noreply.github.com>\n\n---------\n\nCo-authored-by: coderabbitai[bot] <136622811+coderabbitai[bot]@users.noreply.github.com>",
          "timestamp": "2025-08-04T17:49:51Z",
          "tree_id": "bf6ed0fdbb17da7f7488611d1d0841ab08184e82",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/bf9b9240e3cb6c0b2e02bccc47aaf6f209721d81"
        },
        "date": 1754331320231,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 797813756.1999915,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 779046627.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 893324717.0000015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 893244243.2000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3199105030.5999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3198805642.7000017 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3201650053.6000023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3201256332.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3198387013.8999977,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3198144049.3999987 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}