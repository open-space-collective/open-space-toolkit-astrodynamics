window.BENCHMARK_DATA = {
  "lastUpdate": 1718131643012,
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
          "id": "b57043e58528e5ba7847ee591e5ac44637e5425f",
          "message": "refactor: remove unused forward declaration COE (#401)",
          "timestamp": "2024-06-03T15:43:17Z",
          "tree_id": "b20f60333d4be0a762b1032ca46e513ece3c6752",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/b57043e58528e5ba7847ee591e5ac44637e5425f"
        },
        "date": 1717430413515,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 555991874.2000036,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 555968157 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 796271938.2000046,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 796242111.7999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2823842698.399983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2823759369.299999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2819185813.400003,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2819100382.4000006 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2818511635.3000126,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2818453765.200003 ns\nthreads: 1"
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
          "id": "d5a301ae72d7e104d22ce374848492415dc2850c",
          "message": "chore: sort 'using' declarations lexographically (#404)",
          "timestamp": "2024-06-04T12:32:10-07:00",
          "tree_id": "f49f18ba87201af08d371e635706abb6b57f4e34",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/d5a301ae72d7e104d22ce374848492415dc2850c"
        },
        "date": 1717530559991,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 554890747.799999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 554856277.5 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 797821388.3000023,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 797772786.1999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2782605411.1000075,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782489029.6 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2781957598.9999976,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2781839705.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2782593601.399992,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782487935.3 ns\nthreads: 1"
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
          "id": "285dd0227779d86e1a81aa400305426a9df2299d",
          "message": "feat: add duration step parameter to Orbit.getPassWithRevolutionNumber (#403)\n\n* feat: add duration step parameter to Orbit.getPassWithRevolutionNumber\r\n\r\n* feat: address feedback\r\n\r\n* feat: improve algorithm logic\r\n\r\n* feat: address remaining feedback\r\n\r\n* feat: uncomment tests\r\n\r\n* Apply suggestions from code review\r\n\r\n* feat: address remaining feedback\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>\r\n\r\n---------\r\n\r\nCo-authored-by: Antoine Paletta <98616558+apaletta3@users.noreply.github.com>",
          "timestamp": "2024-06-05T00:16:16Z",
          "tree_id": "46745feb8d86014a89165e4b147f0efa92c7cd36",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/285dd0227779d86e1a81aa400305426a9df2299d"
        },
        "date": 1717547636173,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 542625732.0000104,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 542612924.2999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 787112621.099999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 787093892.4000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2776360251.3999953,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2776293362.0999994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2775799229.100005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2775739568.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2777642112.7999983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2777554426.600003 ns\nthreads: 1"
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
          "id": "86c9d2e79dd10b273557b0eabfea045a886f7934",
          "message": "chore: update README.md (#406)",
          "timestamp": "2024-06-05T20:10:49+02:00",
          "tree_id": "81452e2f6c0b82e76a61c3219869918f30073695",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/86c9d2e79dd10b273557b0eabfea045a886f7934"
        },
        "date": 1717612077634,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 550032266.5000055,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 550000744.5000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 794253163.1000065,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 794222471.3 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2782835247.0999904,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2782689732.5999994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2779459359.3,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2779331155 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2778363563.099998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2778223889.399997 ns\nthreads: 1"
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
          "id": "7406328b0c34e1c8eccf57344cee652e1a15cee3",
          "message": "ci: fix python unit tests (#409)\n\n* ci: fix python unit tests\r\n\r\n* Update Makefile",
          "timestamp": "2024-06-11T08:31:40-07:00",
          "tree_id": "e279516c54b388bb4f18f6c33219198fb765b4ee",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/7406328b0c34e1c8eccf57344cee652e1a15cee3"
        },
        "date": 1718123124077,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 547650810.8000076,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 547609773.6999997 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 799930000.5999885,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 799894543.5999998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2780966149.300008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2780883081.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2786113281.4000087,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786021356.8999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2778601050.4999976,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2778509527.5999975 ns\nthreads: 1"
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
          "id": "8986e16b7cbe7be1fa8e3050dda04fd14a2f8227",
          "message": "fix: prevent State addition and subtraction based on coordinates broker not being equal (#407)\n\n* fix: prevent State addition and subtraction based on coordinates broker not being equal\r\n\r\n* test: fix State addition and subtraction operation tests\r\n\r\n---------\r\n\r\nCo-authored-by: Pau Hebrero <pau.hebrero@gmail.com>",
          "timestamp": "2024-06-11T18:27:04Z",
          "tree_id": "6609f4572464df0a0cb5cce0ddb6c6538845505f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/8986e16b7cbe7be1fa8e3050dda04fd14a2f8227"
        },
        "date": 1718131640290,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 549449759.7000077,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 549407530.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 792196889.9000036,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 792139396.3000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2804948753.899993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2804757896.5999994 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2797525403.4000044,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2797319780.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2798235957.8999987,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2798049417.400003 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}