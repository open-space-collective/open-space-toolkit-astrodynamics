window.BENCHMARK_DATA = {
  "lastUpdate": 1738361863658,
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
          "id": "ce35abf5dfe8c3cd30da62ca697884b4e0bcbdeb",
          "message": "feat!: include states with Maneuver (#432)\n\n* wip:\n\n* wip:\n\n* fix: tests\n\n* feat: address remaining feedback\n\n* fix: python tests\n\n* Apply suggestions from code review\n\n* feat: address last bit of feedback\n\n* chore: style\n\n* Apply suggestions from code review\n\nCo-authored-by: coderabbitai[bot] <136622811+coderabbitai[bot]@users.noreply.github.com>\n\n* feat: use maneuver static var\n\n* fix: tests\n\n---------\n\nCo-authored-by: coderabbitai[bot] <136622811+coderabbitai[bot]@users.noreply.github.com>",
          "timestamp": "2025-01-23T17:25:44-06:00",
          "tree_id": "46f17bad0ab4ddbe3d5cdc88bb41099e439d180e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ce35abf5dfe8c3cd30da62ca697884b4e0bcbdeb"
        },
        "date": 1737823892051,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 526063160.4000082,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 525980286.3000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 866858324.2999887,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 866627963.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3195252497.6000066,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3195132053.4000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3152921840.500005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3152801861.0999985 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3129821751.199995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3129736019.2999988 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "daf48eb17f983d4ecc4212199f94c2dcf7088142",
          "message": "style: reapply clang-format (#505)\n\nstyle: run clang-format",
          "timestamp": "2025-01-27T10:25:41-06:00",
          "tree_id": "7c49289b2acbe7854af1cdb4f2a6b008d39831f8",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/daf48eb17f983d4ecc4212199f94c2dcf7088142"
        },
        "date": 1737998898742,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 537865093.4999996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 537837250.5999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 878010232.7999941,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 877787556.0999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3191232506.900002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3191064249.600001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3193463704.799996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193109292.6999965 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3192592186.1999826,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3192394786.600002 ns\nthreads: 1"
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
          "id": "df8fdb4270e86a77c0b056cc05bff998c85d14f3",
          "message": "chore: add convenience method for total size of state builder (#504)\n\n* chore: add convenience method for total size of state builder\n\n* Update test/OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.test.cpp\n\n* Update StateBuilder.cpp",
          "timestamp": "2025-01-27T23:18:06+01:00",
          "tree_id": "749b49753860603c1081367e15a7b8ce36ea6777",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/df8fdb4270e86a77c0b056cc05bff998c85d14f3"
        },
        "date": 1738017362809,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 535287271.3000124,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 535257040.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 873403976.7000014,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 873204836.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3190160986.299992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3190032882.4999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3149401269.2999945,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3149263134.1 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3148211783.499994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3148101303.300001 ns\nthreads: 1"
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
          "id": "41d44a31e61475f798f0c0d2c3ce7a898be6a243",
          "message": "docs: update index.rst to include data and provider (#503)",
          "timestamp": "2025-01-29T05:23:29Z",
          "tree_id": "4dacc4d7075697a898c1ef80bbee7365ce18c008",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/41d44a31e61475f798f0c0d2c3ce7a898be6a243"
        },
        "date": 1738129292084,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 534664246.0999988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 534551830.6999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 868277683.7999995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 868224715.0999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3188626204.2999988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188226873.500001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3188892483.800004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188836832.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3188383081.7000046,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3188276393.599996 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "dcbfdf44da86345cc871e66d632547a86f887ce4",
          "message": "fix!: stubs breaking mypy (#506)\n\n* fix: fix breaking stubs, remove py.typed\n\n* fix!: make CDM constructors kwargs-only\n\n* chore: clean up type aliases",
          "timestamp": "2025-01-31T11:11:34+01:00",
          "tree_id": "fea7a97faa66b1dd4b1b4c56e6eee1fde8bbab3a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/dcbfdf44da86345cc871e66d632547a86f887ce4"
        },
        "date": 1738319536292,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 533627038.39999884,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 533615650.2999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 875871560.4999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 875663068.4000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3194577109.999989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3194418717.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3199300528.500004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199116486.0999994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3196648045.8999968,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196454701.0000005 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "dadb18367b616baa4c19d79291bf9750f9f8dadf",
          "message": "build: loosen boost req to 1.82, bump physics to 12 (#507)\n\n* build: loosen boost version req to 1.82\n\n* build: bump physics version to 12",
          "timestamp": "2025-01-31T21:57:12Z",
          "tree_id": "2fbd4bd74ce76880531e533ba7742c1a75aad737",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/dadb18367b616baa4c19d79291bf9750f9f8dadf"
        },
        "date": 1738361862008,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 539506429.8000023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 539399600.6000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 877721382.9999937,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 877673620.5000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3195860926.800009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3195828492.399999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3191969137.599983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3191938944.900003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3193179959.500003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3193151558.999999 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}