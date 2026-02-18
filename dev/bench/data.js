window.BENCHMARK_DATA = {
  "lastUpdate": 1771415900412,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "antoine.paletta@loftorbital.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "committer": {
            "email": "antoine.paletta@loftorbital.com",
            "name": "Antoine Paletta",
            "username": "apaletta3"
          },
          "distinct": true,
          "id": "ef2e2664f87cec3df5498d9fa5e34b3a33227a1c",
          "message": "chore: improve make clean command",
          "timestamp": "2026-02-18T11:26:29Z",
          "tree_id": "618348bf223b3f03e111c3d00a1b3d656a07c261",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ef2e2664f87cec3df5498d9fa5e34b3a33227a1c"
        },
        "date": 1771415898495,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4811341975.299996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4687145117.799999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4164613226.600005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4164044013.8000007 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7262048797.200015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7261372656.600006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7265657518.200005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7265107006.599998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7267949884.799986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7267342778.900002 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}