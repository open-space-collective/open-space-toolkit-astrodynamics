window.BENCHMARK_DATA = {
  "lastUpdate": 1702982722140,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
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
          "id": "2675b54ffa75537e4ead881a9bf06a613bb441ae",
          "message": "fix: Satellite System Builder test import (#302)\n\n* fix: Satellite System Builder test import\r\n\r\n* fix: Satellite System Builder bindings tests\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2023-12-14T20:12:50+01:00",
          "tree_id": "ac37c8c630e074113e7cc0dc6c580d20a5daab94",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/2675b54ffa75537e4ead881a9bf06a613bb441ae"
        },
        "date": 1702582826896,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 549825633.599994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 549793190 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792504831.6000016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792461530.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2815131032.399995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2814982020.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2812866139.200014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2812733500.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2809423762.699987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2809350869.999999 ns\nthreads: 1"
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
          "id": "51ffce32ed41114273240b86d35dfed820cf04b9",
          "message": "chore: improve dev container with non-root access (#300)\n\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2023-12-15T12:28:28+01:00",
          "tree_id": "eed051a38cfb686b4949eec171e849960504d587",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/51ffce32ed41114273240b86d35dfed820cf04b9"
        },
        "date": 1702641357979,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 548547032.4000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 548526210 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794251930.6999998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794219670.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2785003007.700004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2784857420 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2783967906.100003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783859279.999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2782541853.5999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782385360.000001 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "43561754+Derollez@users.noreply.github.com",
            "name": "Remy Derollez",
            "username": "Derollez"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "9f026667b95fffbf620865addf42ce7b6f6d7efe",
          "message": "feat: add segment solution constructor (#304)\n\n* build: support space separated paths in Makefile\r\n\r\n* feat: add bindings for segment solution constructor\r\n\r\n* test: add test and fix old test\r\n\r\n* chore: style",
          "timestamp": "2023-12-19T11:17:40+01:00",
          "tree_id": "20af3d447cadcff65bccd7a3cd35a328bcd2131b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/9f026667b95fffbf620865addf42ce7b6f6d7efe"
        },
        "date": 1702982721406,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 547279630.5000031,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 547248860.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 798667561.0000077,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 798627540.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2787820351.300013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2787670910 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2789531030.3999906,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2789140340.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2788398550.1999917,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2788208759.999999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}