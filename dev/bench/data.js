window.BENCHMARK_DATA = {
  "lastUpdate": 1708417581820,
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
          "id": "09355cbb4577ff417cdefb5fd92206a0f9a835c9",
          "message": "ci: reorder benchmark and package jobs in release ci (#356)\n\n* ci: reorder benchmark and package jobs in release ci\r\n\r\n* Update release.yml",
          "timestamp": "2024-02-19T16:21:50Z",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/09355cbb4577ff417cdefb5fd92206a0f9a835c9"
        },
        "date": 1708361384215,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 556940934.499994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 556923430 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 795685935.4000016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 795664640.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2782470507.900007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782296960.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2784437259.599992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784283379.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2782463381.399998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782304100.000002 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "84833255bb2992c8e3ba4b1fc402909824b3ee2a",
          "message": "ci: re-combine built and test with benchmark and validation workflows using conditional logic (#358)\n\n* ci: re-combine built and test workflows using conditional logic\r\n\r\n* fix: dependency chain\r\n\r\n* fix: if order\r\n\r\n* fix: redo dependencies",
          "timestamp": "2024-02-20T09:10:40+01:00",
          "tree_id": "e53224ddefe9c245f8184c3bc26586bd324fee88",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/84833255bb2992c8e3ba4b1fc402909824b3ee2a"
        },
        "date": 1708417579833,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 551097039.7999871,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 550998370.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 791429238.4999954,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 791385500.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2788490587.4000025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788362059.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2788472676.799995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788340110.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2789075581.399993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788948220.000002 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}