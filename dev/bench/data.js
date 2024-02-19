window.BENCHMARK_DATA = {
  "lastUpdate": 1708352756977,
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
          "id": "419c50ef072dd9c03ce2dd69da170b9e7d0520e3",
          "message": "refactor: propulsion system to use Reals instead of Scalar (#353)\n\n* refactor: propulsion system to use Reals instead of Scalar\r\n\r\n* fix: shift to string issue",
          "timestamp": "2024-02-16T14:05:26+01:00",
          "tree_id": "3420506dfebf68e27d5891cdcd46bd760014e82d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/419c50ef072dd9c03ce2dd69da170b9e7d0520e3"
        },
        "date": 1708118553364,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 557120489.4999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 557085510 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796572825.100003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796528500.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2792682421.8999996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2792435759.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789578146.3000107,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789422630.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2795467527.200003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2795372179.999998 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "Antoine Paletta",
            "username": "apaletta3",
            "email": "98616558+apaletta3@users.noreply.github.com"
          },
          "committer": {
            "name": "GitHub",
            "username": "web-flow",
            "email": "noreply@github.com"
          },
          "id": "b6ce0cadc982521ff4ed78e11f123314b651a6fb",
          "message": "chore: move benchmark and validation tests to separate CI (#355)\n\n* chore: move benchmark and validation tests to separate CI\r\n\r\n* ci: add test branch for full run\r\n\r\n* ci: make benchmark no longer dependant on build-test and remove concurrency\r\n\r\n* ci: remove test branch\r\n\r\n* ci: redo ordering of benchmark and validate workflow to come after build-test",
          "timestamp": "2024-02-19T14:02:03Z",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b6ce0cadc982521ff4ed78e11f123314b651a6fb"
        },
        "date": 1708352754966,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 549019900.5999955,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 548996709.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 797796542.4000046,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 797759160.0000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2786405342.400002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786319920 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2784283266.8000026,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784228970.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2783796066.700006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783709609.9999995 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}