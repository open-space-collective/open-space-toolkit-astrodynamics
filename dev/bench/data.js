window.BENCHMARK_DATA = {
  "lastUpdate": 1756429502585,
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
          "id": "73437b713745ec843f3a761e8619cf3232e6a772",
          "message": "feat: add ground station method for display (#586)\n\n* feat: add ground station method for display\n\n* fix: issues\n\n* Apply suggestion from @vishwa2710\n\n* Apply suggestion from @coderabbitai[bot]\n\nCo-authored-by: coderabbitai[bot] <136622811+coderabbitai[bot]@users.noreply.github.com>\n\n* tests: add test for display\n\n* feat: address feedback\n\n* fix: tests\n\n---------\n\nCo-authored-by: coderabbitai[bot] <136622811+coderabbitai[bot]@users.noreply.github.com>",
          "timestamp": "2025-08-28T19:37:51-05:00",
          "tree_id": "1757a6597f07400976b4cf87acfde792b3204eb2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/73437b713745ec843f3a761e8619cf3232e6a772"
        },
        "date": 1756429500746,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 826860029.3999726,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 799933472.8000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 891598329.4999819,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 891538608.8000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3195799612.9999743,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3195547048.1000013 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3192672674.09997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192425542.6000037 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3194155708.400001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193972710.7000012 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}