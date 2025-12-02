window.BENCHMARK_DATA = {
  "lastUpdate": 1764697078677,
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
          "id": "45ab06acbce3fb50c917a54f1a0edcb44714598c",
          "message": "docs: make script example compilable (#614)",
          "timestamp": "2025-11-30T10:38:54+05:30",
          "tree_id": "02cd8547af8be0dc1702def25b4f41107c3a42d5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/45ab06acbce3fb50c917a54f1a0edcb44714598c"
        },
        "date": 1764480697550,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 850162860.5999997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 800557068.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 894929006.2999865,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 894793459.6999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3215766943.2999907,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3215572098.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3201709218.700006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3201521481.2000012 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3199879244.599998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199638050.7999988 ns\nthreads: 1"
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
          "id": "2679d6d948343245a1d6e76217b48428f6163f98",
          "message": "feat: add COE static constructors (#615)\n\n* Add Sun-synchronous and Geosynchronous COE computations\n\n- Implemented methods to compute Sun-synchronous inclination and RAAN from LTAN in the COE class.\n- Added static methods for constructing Sun-synchronous and Geosynchronous COEs.\n- Updated Python bindings to expose new functionalities.\n- Enhanced unit tests to validate new computations and ensure proper error handling for undefined parameters.\n\n* Add Circular, Equatorial, and CircularEquatorial COE static constructors\n\n- Added COE::Circular for creating circular orbits with specified inclination\n- Added COE::Equatorial for creating equatorial orbits with specified eccentricity\n- Added COE::CircularEquatorial as convenience for circular equatorial orbits\n- Updated Orbit::Circular and Orbit::Equatorial to use new COE constructors\n- Added Python bindings and tests for new constructors\n\n🤖 Generated with [Claude Code](https://claude.com/claude-code)\n\nCo-Authored-By: Claude <noreply@anthropic.com>\n\n* fix: tests\n\n* feat: simplify\n\n* feat: improve sun sync algorithm\n\n* chore: minor cleanup\n\n* chore: style\n\n* feat: address feedback\n\n---------\n\nCo-authored-by: Vishwa <vishwa@vishwas-macbook-air.tail48d8e.ts.net>\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2025-12-02T11:53:37+05:30",
          "tree_id": "a2bafa0a8506c71725283de0b92f2968eba81ccc",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/2679d6d948343245a1d6e76217b48428f6163f98"
        },
        "date": 1764657867377,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 956845373.3000126,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 818747874.5999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 896347831.7000067,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 896291272.0999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3214378348.400004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3214137728.600001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3207987740.699997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3207757490.8000016 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3204537217.8999968,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204314789.200001 ns\nthreads: 1"
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
          "id": "98be687bd8af9048dfcf3870d618d41b953d3ca9",
          "message": "refactor: revert breaking change in orbit geosynchronous constructor (#616)\n\n* refactor: revert breaking change in orbit geosynchronous constructor\n\n* fix: docstrings",
          "timestamp": "2025-12-02T22:45:20+05:30",
          "tree_id": "8be2122e6640bdaec502337f865a5a818e433e16",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/98be687bd8af9048dfcf3870d618d41b953d3ca9"
        },
        "date": 1764697077002,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 950373068.4999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 823437507.3000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 899437490.8000053,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 899312019.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3206512563.700005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3206177488.200001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3200857601.600001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3200513929.3999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3200386220.899992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3200113952.6000004 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}