window.BENCHMARK_DATA = {
  "lastUpdate": 1750176610958,
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
          "id": "d69d3cf4bf2d981bda08c71b3509a84f005c9a8c",
          "message": "fix: tabulated profile implementation (#563)\n\n* fix: tabulated profile implementation\n\n* Update src/OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Tabulated.cpp\n\n* test: make tests pass\n\n* test: re-add equal to test",
          "timestamp": "2025-06-10T17:54:16Z",
          "tree_id": "cd365943574b7ce053a895c80cd5f09a98ff8adf",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/d69d3cf4bf2d981bda08c71b3509a84f005c9a8c"
        },
        "date": 1749582155161,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 554903900.3999894,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 554831727.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 884718189.699987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 884526217.1999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3197469475.500003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197152573.700001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3195130257.900018,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194902572.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3197902222.9999943,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197611576.199998 ns\nthreads: 1"
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
          "id": "041d7daa544115b3d9e434e3c9875a155c527fe7",
          "message": "feat: add binding for access celestial object (#568)\n\n* feat: add binding for access celestial object\n\n* fix: tests",
          "timestamp": "2025-06-17T15:48:48Z",
          "tree_id": "336d807249da39affbad0aacbf0faacd83cfc14a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/041d7daa544115b3d9e434e3c9875a155c527fe7"
        },
        "date": 1750176609504,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 545496195.6000034,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 545419478.4999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 888038228.1999915,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 887793706.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3206617396.7000113,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3206201417.1999993 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3204473246.000009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204211533.9000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3212563288.999996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3212324401.999997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}