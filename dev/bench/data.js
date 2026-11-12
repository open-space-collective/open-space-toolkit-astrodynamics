window.BENCHMARK_DATA = {
  "lastUpdate": 1794524784268,
  "repoUrl": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
  "entries": {
    "Benchmark": [
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
          "id": "05612c0483d922ea27a4ff78afd2099439eeeace",
          "message": "fix: include src/ for static build (#764)\n\nThe conjunction probability-of-collision algorithms (Alfano2005,\nFoster1992, Frisbee2015) include the private Utility/String.hpp under\nsrc/. The shared library target has src/ on its include path but the\nstatic one doesn't, so BUILD_STATIC_LIBRARY=ON fails to compile. Match\nthe shared library.\n\nCo-authored-by: Kyle Cochran <kyle@samaraaerospace.com>\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-11-12T22:37:20Z",
          "tree_id": "e28154e6fffc8c5ffe8ed5bfd18890e688ec5bea",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/05612c0483d922ea27a4ff78afd2099439eeeace"
        },
        "date": 1794524782306,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 319744404.00000507,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 304789589.2 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 439.9619269999941,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 439.8797896666667 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1835.679146333329,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1835.1017323333335 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 100.42604933333148,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 100.36748899999992 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1930549778.7999969,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1930190056.9999993 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 5041695485.100001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5041215342.200002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 5034594888.799995,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5034203940.900001 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 5036517730.09999,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 5036208853.400001 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.5320685450000155,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.5320545059999802 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 1.7449701289999666,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.7448732790000179 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 1.8915867580001304,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.8914428649999877 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 2.4211449679999077,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.420999781000006 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 5.32614870600014,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.3257590570000275 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 4.838234783000075,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.837867014999972 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 4.826682542999947,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.826271665000007 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 73.500623668,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 73.49340410200003 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.018680457999948885,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.018678724000039892 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 17.83716249512693,
            "unit": "us/iter",
            "extra": "iterations: 38475\ncpu: 17.83655014944822 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}