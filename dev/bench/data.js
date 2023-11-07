window.BENCHMARK_DATA = {
  "lastUpdate": 1699378726677,
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
          "id": "6c2058e09d09f804fc7a59361f795142b81d87da",
          "message": "fix: fix extra state being observed in integrating with conditions (#256)\n\n* fix: fix extra state logging in integrating with conditions\r\n\r\n* chore: add test",
          "timestamp": "2023-10-18T09:39:29-07:00",
          "tree_id": "f78528d2794f03a09861d6cab45518ece6302f83",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/6c2058e09d09f804fc7a59361f795142b81d87da"
        },
        "date": 1697649472588,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 700804847.3999964,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 700712730.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1475080339.2000136,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1474736170.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4144850291.1000047,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4144665029.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4144433202.800008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4144242720 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4148790150.700006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4148509929.9999986 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "kyle.ray.cochran@gmail.com",
            "name": "kyle-cochran",
            "username": "kyle-cochran"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "f964cf6b9e7dcc407d42888a4e370aab0eed73d7",
          "message": "feat: add hasSubset method to State (#259)",
          "timestamp": "2023-10-19T14:03:14-07:00",
          "tree_id": "25038016ffa36415eb251bbe699f4622ae148b2a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f964cf6b9e7dcc407d42888a4e370aab0eed73d7"
        },
        "date": 1697818026697,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 694651142.0000093,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 694492070.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1486414979.3000026,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1486290660.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4153320954.1000075,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4153053500.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4150838487.000005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4150599080.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4150871723,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4150644689.9999986 ns\nthreads: 1"
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
          "id": "30ca4aa199338787e8ce4e40e6f333126f22fefc",
          "message": "chore: align release dockerfile with ostk-data seeding paradigm (#258)\n\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2023-10-22T22:48:42Z",
          "tree_id": "269f9e3fecd2fdb8c3d07e6a19b1486bc43aebb5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/30ca4aa199338787e8ce4e40e6f333126f22fefc"
        },
        "date": 1698017112372,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 703098763.1000073,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 703004630 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1481795798.6000009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1481710180.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4157491775,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4157145200.0000014 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4158035048.4000006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4157810889.9999986 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4157610275.400009,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4157397729.999999 ns\nthreads: 1"
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
          "id": "048bf6e048dbb46b3fcbaf4b450086541432b67a",
          "message": "fix: migrate ostk-data cache location to /var/cache (#260)",
          "timestamp": "2023-10-23T22:54:01+02:00",
          "tree_id": "352ab4c4924075fa31f756ac331f358c5b7e42ff",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/048bf6e048dbb46b3fcbaf4b450086541432b67a"
        },
        "date": 1698097186022,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 1098942587.7000089,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1098161710.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2078505430.8000157,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2078042509.9999988 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 5333919373.099991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5332758020 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5346472085.999994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5345771689.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5307418752.599983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5306151109.999996 ns\nthreads: 1"
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
          "id": "174cba3400c9e31e711ad78505b2bc935ce638cf",
          "message": "docs: add python docstrings (#261)\n\n* chore: add docstrings for python bindings\r\n\r\n* chore: add docstrings for python bindings\r\n\r\n* chore: fix some groups\r\n\r\n* feat: cleanup the docstrings\r\n\r\n* feat: cleanup docstrings more\r\n\r\n* feat: further refine docstrings\r\n\r\n* feat: last cleanup",
          "timestamp": "2023-10-25T12:50:12-07:00",
          "tree_id": "26ccab597fa9767177e1e9673e84ae0990daef39",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/174cba3400c9e31e711ad78505b2bc935ce638cf"
        },
        "date": 1698265889650,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 741396453.5999867,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 740965860.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1659151564.6999936,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1658707680.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4034770626.5,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4033625220.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4042655340.0999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4041705639.9999976 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4139553237.799993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4138602179.9999976 ns\nthreads: 1"
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
          "id": "ab528cf717a62c1b616e35e55130f290cc606ab8",
          "message": "chore: update dev container to better enable cpp and python liniting (#243)\n\n* chore: add black formatter to dev container\r\n\r\n* chore: remove linting ignore warnings\r\n\r\n* fix: add back reportMissingModuleSource in devcontainer.json because stubs will not fix it",
          "timestamp": "2023-10-27T15:00:30+02:00",
          "tree_id": "4f04fe7f3bcb6a389e0444bfc6bd6e5e6064151a",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/ab528cf717a62c1b616e35e55130f290cc606ab8"
        },
        "date": 1698414114940,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 995377031.2999951,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 995080970.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1941376169.5000062,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1941147440 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4917954704.600004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4917400480.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4969352382.300008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4968749140 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4911006627.200004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4910453790.000006 ns\nthreads: 1"
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
          "id": "be6a9af7714f7491ec0aa513cbcf1e5c8a7e1a0d",
          "message": "feat: Event Condition termination for Sequences (#262)\n\n* feat: add event condition option to a Sequence\r\n\r\n* feat: add python bindings and test\r\n\r\n* chore: style\r\n\r\n* feat: formatting\r\n\r\n* feat: fix docs\r\n\r\n* feat: address Kyle's feedback\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa@loftorbital.com>",
          "timestamp": "2023-10-31T19:26:46Z",
          "tree_id": "143ac51b7a3ea11ed86b78414ea3f6f63a0fd682",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/be6a9af7714f7491ec0aa513cbcf1e5c8a7e1a0d"
        },
        "date": 1698782742038,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 695246790.9000007,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 694947969.9999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1480103697.6000035,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1479855249.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4144411764.099993,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4143754709.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4143399230.899991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4143203209.999996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4143605290.6999917,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4143386370 ns\nthreads: 1"
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
          "id": "62ba2dcd892839ff8782da93e1133295d460e130",
          "message": "feat: add finite difference solver (#264)\n\n* feat: add finite difference solver class\r\n\r\n* feat: add backward, forward and central differencing options\r\n\r\n* feat: get it to compile\r\n\r\n* feat: keep working on it\r\n\r\n* feat: adding test\r\n\r\n* feat: add bindings\r\n\r\n* chore: nit cleanup\r\n\r\n* chore: format\r\n\r\n* feat: address remaining feedback\r\n\r\n---------\r\n\r\nCo-authored-by: Vishwa Shah <vishwa@loftorbital.com>",
          "timestamp": "2023-11-02T12:17:22-07:00",
          "tree_id": "5229cca74642e081fb9fbafface513c9c8fff2c9",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/62ba2dcd892839ff8782da93e1133295d460e130"
        },
        "date": 1698955247103,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 664590131.4999946,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 664544610.0000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1354859042.700008,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1354778880 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2783420497.3,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783273130.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2784000144.099997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2783645030.0000024 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2786426096.49998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2786204250.0000014 ns\nthreads: 1"
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
          "id": "92babf02122dbe061a23d0fe3d85a36699a7de12",
          "message": "feat: event condition relative target (#263)\n\n* feat: add relative target for event conditions\r\n\r\n* feat: update tests\r\n\r\n* feat: add duration condition\r\n\r\n* feat: use a relativeTarget_ so as to not update the target_ field ever\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* feat: use a Target object\r\n\r\n* style: format\r\n\r\n* fix: tests\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n* feat: address remaining feedback\r\n\r\n* chore: format\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\nCo-authored-by: Vishwa Shah <vishwa@loftorbital.com>",
          "timestamp": "2023-11-02T13:20:09-07:00",
          "tree_id": "de5fb039d279d7e713432f472a198540943ce653",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/92babf02122dbe061a23d0fe3d85a36699a7de12"
        },
        "date": 1698958544646,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 736335695.300005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 736236970 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1651639716.599999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1651456590.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4174895279.1000075,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4174550810.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4175517829,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4175169220.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4175262348.1,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4174922620.000001 ns\nthreads: 1"
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
          "id": "697c95ae56cc88b04a98ace4960e9e3992c9094d",
          "message": "feat: update finite difference solver callable interface (#267)\n\n* feat: update finite difference solver callable interface\r\n\r\n* chore: add bindings and test\r\n\r\n* feat: add test for default constructor\r\n\r\n* feat: tweak interface one last ride\r\n\r\n* Apply suggestions from code review\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>\r\n\r\n---------\r\n\r\nCo-authored-by: kyle-cochran <kyle.ray.cochran@gmail.com>",
          "timestamp": "2023-11-05T19:09:13Z",
          "tree_id": "a416c8143ccb1b0520026eaaa549ca70aa55a14f",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/697c95ae56cc88b04a98ace4960e9e3992c9094d"
        },
        "date": 1699213407619,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 737412954.7000053,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 737305300.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1654337399.4999967,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1653917089.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4132944912.8999953,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4132548149.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4133212200.1000013,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4132828759.999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4133057223.6000304,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4132640080 ns\nthreads: 1"
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
          "id": "f6862fb796333eefba9346168fad8c77b1f961f2",
          "message": "feat!: guidance law (#265)\n\n* wip: first cut at guidance law\r\n\r\n* wip: feat: add tests\r\n\r\n* feat: fix a test\r\n\r\n* feat: fix remaining tests\r\n\r\n* feat: address Remy's feedback:\r\n\r\n* refactor: move guidance law around\r\n\r\n* feat: fix tests\r\n\r\n* style: format",
          "timestamp": "2023-11-06T11:41:16-08:00",
          "tree_id": "e89ed3cc8772d12fa7ec14c8adb3f797666a69d1",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f6862fb796333eefba9346168fad8c77b1f961f2"
        },
        "date": 1699302074870,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 674086456.400002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 674044100.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1349285234.299998,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1349143159.9999995 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2803307610.9000006,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2802998520.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2804261905.0999975,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2803899330.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2808146368.2999983,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2807916470.0000024 ns\nthreads: 1"
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
          "id": "c8fe8448cd30743f085481d1a4bd5408d6d48f8e",
          "message": "feat: generalize finite difference solver jacobian computation (#268)\n\n* feat: generalize finite difference solver jacobian computation\r\n\r\n* feat: fix tests",
          "timestamp": "2023-11-07T09:05:02-08:00",
          "tree_id": "325e1338729b90c28fae085dd1fbb694952e0ed4",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/c8fe8448cd30743f085481d1a4bd5408d6d48f8e"
        },
        "date": 1699378725741,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 669727332.0000021,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 669691200 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1346304171.799997,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1346229880.0000002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 2790882658.4999986,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2790763470.0000005 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 2794338357.800012,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2794203640.000001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 2795441207.3000016,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2795295439.9999995 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}