window.BENCHMARK_DATA = {
  "lastUpdate": 1764074834458,
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
          "id": "684be293e910c5e61578f2912451ed17e35379a9",
          "message": "refactor: use const for heterogeneous guidance law (#598)\n\n* refactor: use const for heterogenous guidance law\n\n* fix: format",
          "timestamp": "2025-11-11T19:07:34Z",
          "tree_id": "4327536dae020e9cb50e35f0ddcf6deb80aa6c4e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/684be293e910c5e61578f2912451ed17e35379a9"
        },
        "date": 1762889355832,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 996021766.0000069,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 816953350.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 894244376.6999872,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 894171655.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3199544404.3000135,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199287632.299999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3197077294.3000154,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196852021.900001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3197201177.7999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196975737.2000017 ns\nthreads: 1"
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
          "id": "9baf153348dda16d123e93eace10b58103be8337",
          "message": "feat: allow Q-Law COE domain selection (#599)\n\n* feat: allow Q-Law COE domain selection\n\n* feat: clean it up a bit\n\n* feat; add some tests\n\n* Apply suggestions from code review\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n* feat: address feedback\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2025-11-11T12:23:17-08:00",
          "tree_id": "c242dde84a7a9f9abc5121d2aa2e2035df034a38",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/9baf153348dda16d123e93eace10b58103be8337"
        },
        "date": 1762893923356,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 865900384.2000004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 828158189.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 914691188.8000034,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 914593862.7000004 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3205039482.4000025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204634836.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3203337438.099993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3202795189.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3203751381.400008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3203292850.1000013 ns\nthreads: 1"
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
          "id": "6ef063aee28a37ad76c0e82ed2cf02cff4676aa7",
          "message": "fix: NumericalSolver incorrect observed states when condition not met (#603)\n\n* fix: NumericalSolver incorrect observed states when condition not met\n\n* chore: style\n\n* Apply suggestion from @apaletta3\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\n\n---------\n\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2025-11-18T18:23:54Z",
          "tree_id": "8835f492f8118dbd931f001641aeb17279522839",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6ef063aee28a37ad76c0e82ed2cf02cff4676aa7"
        },
        "date": 1763491487980,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 897700586.1000122,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 817886147.7 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 896898282.6999991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 896809505.0999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3204394074.2000017,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3204047361.7000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3202066005.499995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3201674384.199998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3202430604.099999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3202011194.499998 ns\nthreads: 1"
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
          "id": "0d1880600169fc478d14678e9a039d62ac8375e3",
          "message": "docs: clarify mltan input (#605)\n\n* docs: clarify mltan input\n\n* refactor: address vishwa's feedback",
          "timestamp": "2025-11-20T13:42:49-08:00",
          "tree_id": "2317da939da4a75ddf80c84df46983d3aa3012c5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/0d1880600169fc478d14678e9a039d62ac8375e3"
        },
        "date": 1763729941638,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 861879399.3999987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 812112395.8000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 891044631.099993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 890949161.9000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3200901988.4999986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3200584540.099999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3197637384.200004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197402561.1000037 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3199387525.8,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199109701.4000034 ns\nthreads: 1"
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
          "id": "e80d587f1455998dd14066c82d4570368b365d1c",
          "message": "feat: add utility methods to compare orbits and states (#596)\n\n* fix: update docstrings and tests for compare functions\n\nUpdated docstrings for compare_orbit_to_states and compare_states_to_states\nto accurately reflect the actual return values. The functions return\ndictionaries with detailed position and velocity residuals including both\nL2 norms (dr, dv) and component-wise differences (dr_x, dr_y, dr_z, dv_x,\ndv_y, dv_z), along with timestamps.\n\nUpdated all related tests to check for the correct dictionary keys and\nstructure instead of the previously incorrect return types.\n\n🤖 Generated with [Claude Code](https://claude.com/claude-code)\n\nCo-Authored-By: Claude <noreply@anthropic.com>\n\n* fix: tests\n\n* fix: tests\n\n* Apply suggestion from @vishwa2710\n\n* feat: address remaining feedback\n\n* chore: fix typing\n\n* feat: address remaining feedback\n\n* chore: can't use strict with zip, to support python3.9\n\n---------\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2025-11-24T17:13:30Z",
          "tree_id": "f34485d8635e030f9618d12443676264daf172c9",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e80d587f1455998dd14066c82d4570368b365d1c"
        },
        "date": 1764005874390,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 810711890.500005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796663651.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 895778360.0000026,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 895666860.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3201495801.8000073,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3201239675.4999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3199380351.500008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3199148506.299999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3200810583.699979,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3200538217.0000005 ns\nthreads: 1"
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
          "id": "b058ffffc332d018724658dd55795691ea263733",
          "message": "refactor: update naming for compute_residuals_for_orbit (#612)",
          "timestamp": "2025-11-25T12:26:37Z",
          "tree_id": "5fae99defb17179f2f0ce8f9429a797d87daff84",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b058ffffc332d018724658dd55795691ea263733"
        },
        "date": 1764074831544,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 837879338.100015,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 800721668.8999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 892913206.9999924,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 892638809.2999992 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3197252556.9000025,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3197071751.5999994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3195731091.399989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3195554890.6000013 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3196648675.8999963,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3196474011.1999974 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}