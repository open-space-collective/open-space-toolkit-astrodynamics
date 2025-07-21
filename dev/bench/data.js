window.BENCHMARK_DATA = {
  "lastUpdate": 1753121697323,
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
          "id": "403931799fb9c19575cc37d9e836ee417a411fcc",
          "message": "feat: Modified Equinoctial Orbital Elements (MEOE) (#566)\n\n* Add Modified Equinoctial Orbital Elements (MEOE)\n\nThis commit introduces the Modified Equinoctial Orbital Elements (MEOE) class, providing an alternative to Classical Orbital Elements (COE) for representing satellite orbits, particularly useful for orbits with small eccentricities or inclinations.\n\nThe MEOE set includes:\n- p: semi-latus rectum\n- f: x-component of eccentricity vector (e * cos(RAAN + AOP))\n- g: y-component of eccentricity vector (e * sin(RAAN + AOP))\n- h: x-component of node vector (tan(i/2) * cos(RAAN))\n- k: y-component of node vector (tan(i/2) * sin(RAAN))\n- L: true longitude (RAAN + AOP + True Anomaly)\n\nKey features implemented:\n1.  **MEOE Class:**\n    *   Defined in `OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/MEOE.hpp` and implemented in `MEOE.cpp`.\n    *   Includes constructors, getters for each element, equality operators, and print/string representations.\n2.  **Cartesian Conversions:**\n    *   `MEOE::Cartesian()`: Converts Cartesian state (position, velocity) to MEOE. This is achieved by first converting to COE and then to MEOE, handling singularities like 180-degree inclination.\n    *   `MEOE::getCartesianState()`: Converts MEOE to Cartesian state using direct formulas from JPL documentation.\n3.  **Unit Tests (C++):**\n    *   Added `MEOE.test.cpp` with comprehensive tests for constructors, getters, conversions (including round-trip), and edge cases (circular, equatorial, 180-deg inclination orbits, p<=0, q=0 singularities).\n4.  **CMake Integration:**\n    *   The new files are automatically included in the build process due to existing `FILE(GLOB_RECURSE)` patterns in CMakeLists.txt.\n5.  **Python Bindings:**\n    *   Exposed the `MEOE` class and its methods to Python via Pybind11.\n    *   Created `bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model/MEOE.cpp` for the binding code.\n    *   Updated Python binding CMakeLists to correctly include `.cpp` source files.\n6.  **Python Tests:**\n    *   Added `bindings/python/test/trajectory/orbit/models/test_meoe.py` with a comprehensive suite of tests mirroring the C++ tests, ensuring the Python interface is robust and correct.\n\nThe implementation aims to be consistent with the existing COE class structure and provides a valuable new tool for orbital mechanics analysis within OpenSpace Toolkit.\n\n* fix: tests\n\n* fix: tests\n\n* chore: style\n\n* feat: address remaining feedback\n\n* Apply suggestions from code review\n\nCo-authored-by: Pau Hebrero <65550121+phc1990@users.noreply.github.com>\n\n* feat: address remaining feedback\n\n---------\n\nCo-authored-by: google-labs-jules[bot] <161369871+google-labs-jules[bot]@users.noreply.github.com>\nCo-authored-by: Pau Hebrero <65550121+phc1990@users.noreply.github.com>",
          "timestamp": "2025-06-24T07:44:08-05:00",
          "tree_id": "50e05f7a0512814dbe4cdbd72a94cc5084584879",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/403931799fb9c19575cc37d9e836ee417a411fcc"
        },
        "date": 1750770247638,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 559342110.7999859,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 559270574.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 887594581.4000032,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 887511680.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3204631761.699994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204375442 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3204415811.8000154,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3203924668.399999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3206589707.1999984,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3206231950 ns\nthreads: 1"
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
          "id": "578d85b045d62fa62c99429a482bd92a9454bfdf",
          "message": "feat: add COE getCartesianState frame check (#571)\n\n* feat: add COE getCartesianState frame check\n\n* Update COE.cpp\n\n* Update ModifiedEquinoctial.cpp\n\n* Update ModifiedEquinoctial.test.cpp\n\n* Update COE.test.cpp\n\n* Update ModifiedEquinoctial.test.cpp\n\n* Update COE.test.cpp\n\n* fix: format\n\n* fix: compilation\n\n* fix: format",
          "timestamp": "2025-07-16T13:32:07Z",
          "tree_id": "463a7a8aa647c8e3b7a06a3b54684af1366ffacb",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/578d85b045d62fa62c99429a482bd92a9454bfdf"
        },
        "date": 1752674315674,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 560687687.4999983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 560534319.1000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 894225445.6000057,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 894160387.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3199233972.999997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199055625.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3200229137.799994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3200043039.900001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3198133568.500015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197954794.0999975 ns\nthreads: 1"
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
          "id": "6fdd836250803f52081abc8e48906973cb0e9fd9",
          "message": "test: remove unnecessary use of TEME frame in tests (#572)",
          "timestamp": "2025-07-17T13:30:49-05:00",
          "tree_id": "931a31230efe31a6926a89c87bc10ca77344d775",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6fdd836250803f52081abc8e48906973cb0e9fd9"
        },
        "date": 1752778597163,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 558552658.900004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 558400180.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 892609470.1000124,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 892441467.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3203876036.09999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3203412193.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3202456329.5999937,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3202184671.900001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3202840742.4999895,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3202412963.500001 ns\nthreads: 1"
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
          "id": "ec7db90cc36ce23e8f1c0873ce25cfc93e006d51",
          "message": "feat: add eclipse generator (#573)\n\n* feat: add eclipse generator\n\n* Apply suggestions from code review\n\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>\nCo-authored-by: Vishwa Shah <vishwa2710@gmail.com>",
          "timestamp": "2025-07-21T17:36:45Z",
          "tree_id": "8ab8bcd201e1decdb11e20185e84b6a7c8d59d5d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ec7db90cc36ce23e8f1c0873ce25cfc93e006d51"
        },
        "date": 1753121695338,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 551478150.4999859,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 551352752.7999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 891423239.3000021,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 891352029.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3194481556.200003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194254641.7000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3193152919.700009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192785597.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3193194771.000003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193027134.2999988 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}