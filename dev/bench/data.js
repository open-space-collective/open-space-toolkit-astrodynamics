window.BENCHMARK_DATA = {
  "lastUpdate": 1745013119041,
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
          "id": "d718fd9a0a9503dcb2be5b6f11d366914321b143",
          "message": "feat: update velocity computation ground strip (#540)\n\n* feat: add geodetic nadir trajectory ground strip\n\n* feat: update tests and python\n\n* Apply suggestions from code review\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n* feat: address remaining feedback\n\n---------\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2025-04-16T19:14:45Z",
          "tree_id": "1bf057728a2263a60a933fd055a05e1be9ab8a8b",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/d718fd9a0a9503dcb2be5b6f11d366914321b143"
        },
        "date": 1744832322469,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 541338369.4999993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 541312855.4000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 872910227.6000049,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 872730571.7999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3188373073.699995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188268256.1000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3186797245.4000154,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186689798.1 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3187483976.999999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3187379444.1 ns\nthreads: 1"
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
          "id": "f32a854bb17e55883c06a7a1a95f15a121af705a",
          "message": "feat!: add minimum maneuver duration constraint (#541)\n\n* feat: add minimum maneuver duration constraint\n\n* fix: print test\n\n* feat: address feedback",
          "timestamp": "2025-04-18T16:04:23-05:00",
          "tree_id": "4adeaab16808a9e2758eb834947902af356b2b33",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f32a854bb17e55883c06a7a1a95f15a121af705a"
        },
        "date": 1745011444765,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 538237378.600013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 538188626.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 881556438.1999934,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 881424365.4999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3197978050.0999937,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197764425.700001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3193580834.399989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193419216.0999985 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3193173737.199993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193069849.999999 ns\nthreads: 1"
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
          "id": "dcfd0dee197966acf3ffe1828001d092861bc4ce",
          "message": "fix: within range angular condition, wrapping boundaries (#542)\n\n* fix: angular condition within range across boundaries\n\n* chore: style",
          "timestamp": "2025-04-18T16:32:30-05:00",
          "tree_id": "77b9d6325d6087ee1fb7aaee5e51c8dd131aef96",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/dcfd0dee197966acf3ffe1828001d092861bc4ce"
        },
        "date": 1745013117606,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 533242687.49999535,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 533232528.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 871149684.8000082,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 870919388.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3187028409.00001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186930968.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3186584739.7999916,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3186486139.4000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3185932891.599998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3185859409.1000013 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}