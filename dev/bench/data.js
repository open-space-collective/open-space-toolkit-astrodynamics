window.BENCHMARK_DATA = {
  "lastUpdate": 1765799972566,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "35860220+alex-liang3@users.noreply.github.com",
            "name": "Alex Liang",
            "username": "alex-liang3"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "883d9ecdff14b7ae1337b32f24134b317b6d9c60",
          "message": "fix: handle round fractional days in TLE (#618)",
          "timestamp": "2025-12-11T10:45:54Z",
          "tree_id": "907dbbf92f97e32d93dd93a1f6e0ef66d6743363",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/883d9ecdff14b7ae1337b32f24134b317b6d9c60"
        },
        "date": 1765451746945,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4704428429.999996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4670559451.800001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4078555942.000003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4078242554.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7192503040.899992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7191977516.499995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7193687368.799998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7193002951.699999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7199592018.20002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7198888578.100004 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "a58e6a0bcd452fd005a39e469e2bdc2087fa9596",
          "message": "fix: deal with single-point maneuvers during segment convergence (#619)\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2025-12-15T10:19:41Z",
          "tree_id": "f3044904ebf23f82aecb49eefa6f1c6a1eb7733f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/a58e6a0bcd452fd005a39e469e2bdc2087fa9596"
        },
        "date": 1765795693856,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4715543366.100008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4699133951.500001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4109596086.100009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4108934754.6999984 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7217664083.299991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7216885377.400002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7225893307.799993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7225168561.800006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7224156222.399996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7223359458.200005 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "732972a7c64ffa2ff71f52e4671af0d12967d5eb",
          "message": "refactor: apply MR suggestions in https://github.com/open-space-collective/open-space-toolkit-astrodynamics/pull/619 (#621)\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2025-12-15T12:33:17+01:00",
          "tree_id": "9a1f428bcfdfcda923508ef2c37c4dff191dbf86",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/732972a7c64ffa2ff71f52e4671af0d12967d5eb"
        },
        "date": 1765799970297,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4708135628.999998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4695319324.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4106710200.4999986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4106328758 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7218857604.499998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7218189796.199997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7221777324.499998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7220828649.799993 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7219826457.399996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7218909289.899997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}