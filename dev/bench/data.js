window.BENCHMARK_DATA = {
  "lastUpdate": 1722861878777,
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
          "id": "06195e2e20d45729d95836cf05358ca2eecbfbde",
          "message": "feat: add dataframe <-> OSTk object utilities (#426)\n\n* feat: add dataframe <-> OSTk object utilities\r\n\r\n* fix: remove unused import\r\n\r\n---------\r\n\r\nCo-authored-by: Kyle Cochran <kyle.cochran@loftorbital.com>",
          "timestamp": "2024-08-05T14:24:31+02:00",
          "tree_id": "c183de5bc476d0d54cb62eff4bd4724838831f61",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/06195e2e20d45729d95836cf05358ca2eecbfbde"
        },
        "date": 1722861876561,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 557502643.8999999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 557457932.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 873201036.5000064,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 873160569.7999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3165168760.6999984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3165043035.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3164782736.6999936,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3164589232.6000013 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3163550828.0000124,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3163339659.3000016 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}