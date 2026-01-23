window.BENCHMARK_DATA = {
  "lastUpdate": 1769179848760,
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
          "id": "01afa3e4724b40c1844b50235223eb60587c39da",
          "message": "feat: add lower/upper bound for RootSolver Solution (#635)\n\n* feat: add lower/upper bound for RootSolver Solution\n\n* chore: format",
          "timestamp": "2026-01-16T08:22:20-08:00",
          "tree_id": "8245342675e095a9843999fecca07a128e9a077c",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/01afa3e4724b40c1844b50235223eb60587c39da"
        },
        "date": 1768582215486,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4830618545.799996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4714571518.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4077924329.7999983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4077453226.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7206130220.099998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7205377649.100001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7197363584.799995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7196506983.100001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7192529107.7,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7191712059.900002 ns\nthreads: 1"
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
          "id": "fe9563bdeb896ea9cc08b4b0b0ef32f48a9e9655",
          "message": "fix: numerical solver root bounding (#638)\n\n* fix: improve InstantCondition and NumericalSolver crossing detection\n\n- Fix InstantCondition to calculate time relative to target instant instead of J2000\n- Simplify Array concatenation in Segment.cpp\n- Update NumericalSolver tests for new InstantCondition API\n\nCo-Authored-By: Claude Opus 4.5 <noreply@anthropic.com>\n\n* feat: add Pau's tests\n\n* chore: fix typos\n\n---------\n\nCo-authored-by: Claude Opus 4.5 <noreply@anthropic.com>",
          "timestamp": "2026-01-16T15:43:56-08:00",
          "tree_id": "bc6e06b70a152699dca9a3dd4dc99edb73a5cad2",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/fe9563bdeb896ea9cc08b4b0b0ef32f48a9e9655"
        },
        "date": 1768608666935,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4850904535.800004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4712051380.800001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4084930702.3000054,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4084472081.199998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7194077278.000003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7193258634.200002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7193935977.800005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7193247059.1 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7193815304.300016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7192960314.600003 ns\nthreads: 1"
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
          "id": "f938847ca0dfb2db445e6228213f906d97c71f4c",
          "message": "feat: maneuver by maneuver (#633)\n\n* feat: maneuver by maneuver\n\n* wip:\n\n* feat: works!\n\n* test: add Solve_MultipleManeuvers_WithManeuverConstraintsThatHaveNoImpact test\n\n* test: add OpenSpaceToolkit_Astrodynamics_Trajectory_Segment_Solve_QLawHangingScenariosWhenReachingThreshold test\n\n* test: add 'HangingScenariosWhenReachingTarget' test\n\n* test: remove hanging scenario tests, reason is understood\n\n* wip:\n\n* feat: works!\n\n* chore: style\n\n* tests: clean up tests\n\n* feat: cleanup\n\n* Apply suggestion from @vishwa2710\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-01-21T17:59:28-08:00",
          "tree_id": "ec9f926ad0f4de108b952506a3e227c9bf6f4477",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f938847ca0dfb2db445e6228213f906d97c71f4c"
        },
        "date": 1769048800622,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4807421270.100008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4698564049.200001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4083236829.399999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4082907072.6000047 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7204297986.099981,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7203723531.100001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7208943942.599989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7208443322.899999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7202967220.499977,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7202348604.900009 ns\nthreads: 1"
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
          "id": "e4b1d3e14814a349db8a64c3b507001f2d4722e0",
          "message": "feat: add Chunk strategy for maximum maneuver duration violation (#632)\n\n* feat: add Chunk strategy for maximum maneuver duration violation\n\nAdd a new Chunk strategy to MaximumManeuverDurationViolationStrategy that\nsplits maneuvers exceeding the maximum duration into multiple chunks from\nthe leading edge, rather than truncating or centering.\n\nCo-Authored-By: Claude Opus 4.5 <noreply@anthropic.com>\n\n* chore: fix coverage\n\n* feat: rebase and fix tests\n\n* refactor: apply MR suggestions\n\n---------\n\nCo-authored-by: Claude Opus 4.5 <noreply@anthropic.com>\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-01-22T11:35:43Z",
          "tree_id": "a19c984e2febb8b171a86cbb016101d29896fd4d",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/e4b1d3e14814a349db8a64c3b507001f2d4722e0"
        },
        "date": 1769083364729,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4759938235.599993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4680752510.700001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4092249745.3999877,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4091899547.8 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7202652933.499979,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7201686602.599998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7198527326.300006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7197731500.500006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7205283018.700027,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7204432335.999994 ns\nthreads: 1"
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
          "id": "65c7f3b306d7c3fd20de5bfe6b3a691017968804",
          "message": "fix: maximum maneuver duration violation strategies (#639)\n\n* fix: maximum maneuver duration violation strategies\n\n* test: harmonize test\n\n* test: amend test\n\n* test: address comments\n\n---------\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-01-23T12:11:48Z",
          "tree_id": "95c48f861734ce427eb2d22f5f3d560df73c77c5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/65c7f3b306d7c3fd20de5bfe6b3a691017968804"
        },
        "date": 1769171938378,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4775520489.400003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4675709657 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4080922731.4,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4080548280.2000017 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7186025589.20001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7185371294.000003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7187053935.399991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7186212386.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7184736383.599966,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7184107819.200007 ns\nthreads: 1"
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
          "id": "92f2334c3728fd494a34a81608ca04cb563483bf",
          "message": "fix: build packages in main branch environment (#640)\n\nfix: explicitely include optional in Segment.cpp\n\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2026-01-23T14:23:46Z",
          "tree_id": "4745af8ee43a381af3ca4b2647b14c9ed4f54430",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/92f2334c3728fd494a34a81608ca04cb563483bf"
        },
        "date": 1769179846999,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 4717124060.199995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4661735831.199999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 4078528060.100007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4078161926.0000024 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 7185534736.799968,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7184885928.400003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 7185420868.600034,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7184804481.000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 7188233653.999998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 7187488437.699997 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}