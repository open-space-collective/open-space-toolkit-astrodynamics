window.BENCHMARK_DATA = {
  "lastUpdate": 1788204647119,
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
          "id": "0f86f2be4055e51caa495a3653cfa10c3be9fb53",
          "message": "feat: propagate SGP4 from mean elements, on Vallado's reference implementation (#709)\n\n* feat: propagate SGP4 from mean elements, on Vallado's reference implementation\n\nA TLE is a lossy serialization of an SGP4 mean element set: writing one rounds\nthe eccentricity to 1e-7 and the angles to 1e-4 deg. That is harmless for\npropagation but fatal for differentiation. On a near-circular orbit a\nfinite-difference step in eccentricity can be smaller than the quantum, so the\npropagator does not respond at all and the Jacobian silently loses rank --\nTLESolver then drives the orbit onto a hyperbola and throws \"Algorithm error.\"\n\nAdd MeanElements, which holds the eight elements at full precision, and fit\nagainst it. A TLE is written once, from the converged state.\n\nReaching a propagator that takes elements rather than 69 columns of text meant\nreplacing libsgp4 (dnwrnr/sgp4, installed as a system library) with Vallado's\nreference SGP4, the companion code to AIAA 2006-6753, whose sgp4init() takes the\nelements as doubles. It is vendored rather than installed: two self-contained\nfiles depending on nothing outside the standard library, byte-identical to\nupstream but for line endings, three degree signs in comments, and internal\nlinkage on two debug globals. See thirdparty/vallado-sgp4/README.md.\n\norbit::model::SGP4 now decodes its TLE into a MeanElements and propagates that,\nso there is one propagator behind both entry points, and the Alpha-5 satellite\nnumber laundering that libsgp4's parser required is gone with it.\n\nMeasured against the outgoing libsgp4 over 201 samples per window, the two agree\nto 0.28 um (ISS, +/-1 d), 6.5 um (SSO, +/-3 d) and 52 um (Molniya, +/-1 d) --\nthe size of the numerical step this costs. Propagation errors now surface as\nostk RuntimeError rather than libsgp4's exception types.\n\nCo-Authored-By: Claude Opus 5 (1M context) <noreply@anthropic.com>\n\n* docs: prescribe when to use SGP4 versus MeanElements\n\nBoth propagate SGP4 and, since orbit::model::SGP4 decodes its TLE into a\nMeanElements, both run the same code. What separates them is what precision can\nreach that code, and neither class said so.\n\nGive each a class doc that names the other: SGP4 takes a TLE and therefore flies\nelements quantized by the text format (eccentricity 1e-7, angles 1e-4 deg, mean\nmotion 1e-8 rev/day, epoch 1e-8 day), which is free when the elements were\npublished at that precision and fatal when something differentiates the\npropagator; MeanElements takes the eight elements as continuous values and is\nwhat estimators want. C++ and Python both.\n\nCo-Authored-By: Claude Opus 5 (1M context) <noreply@anthropic.com>\n\n* feat: add one more test\n\n* feat: address feedback\n\n* refactor: rename to FullPrecision\n\n* chore: style\n\n* wip:\n\n* feat: simplify and make thread safe and performant\n\n* feat: address Alex's feedback\n\n* fix: repair the SGP4FullPrecision python test, and document the vendored SGP4\n\nThe python test file did not parse: a duplicated `test_calculate_state_at`\ndefinition carried a fragment of `test_from_tle`'s body with it. pytest failed\nat collection, taking all 839 tests in the suite down with it, so neither this\nfeature's python tests nor any other had run. `test_calculate_states_at` also\nreferenced an `instants` that was never defined; it now builds the same three\ninstants the C++ test uses. Dropped the unused numpy and SGP4 imports and ran\nblack over the result.\n\nWrote the `thirdparty/vallado-sgp4/README.md` that CMakeLists.txt and\nSGP4FullPrecision.cpp both point at: upstream, version, the fact that the files\nare verbatim, why it is vendored rather than installed, why it compiles with\n-w, and how to update it. Put SGP4 back in README.md's dependency table too --\nvendoring moves a dependency, it does not remove it, and that table is the\nlicense inventory.\n\nAlso \"precision that what can be encoded\" -> \"than\", in the header and in the\npython docstring that mirrors it.\n\nCo-Authored-By: Claude Opus 5 (1M context) <noreply@anthropic.com>\n\n---------\n\nCo-authored-by: Claude Opus 5 (1M context) <noreply@anthropic.com>",
          "timestamp": "2026-08-26T15:17:50-07:00",
          "tree_id": "57ce1c2a07dd5265e3ea175211d5e21068c8d4c5",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/0f86f2be4055e51caa495a3653cfa10c3be9fb53"
        },
        "date": 1787784708588,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2483352020.4000483,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2447251284.7 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 836.3702076666565,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 836.2332846666665 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2621.683720999954,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2621.289850666667 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 9793.453304666627,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 9792.359800333334 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2572915724.5000353,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2572636325.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6173754295.199956,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6173129994.099999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6695968609.599981,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6695286107.199996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6061172281.199969,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6060058146.699984 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.7921381629998905,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.7920165730000122 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 4.2909721629998785,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.2905145850000395 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 9.737896097999965,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 9.736883512999952 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 6.43934142199987,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.438660357999936 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 9.072960273999797,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 9.071867794000013 s\nthreads: 1"
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
          "id": "a0791f6225a3e98a88a78bc8336939de75cb94be",
          "message": "perf: simplify third body gravity calculation (#707)\n\n* perf: reduce per-step allocation overhead in the propagation loop\n\nTwo hot-loop cleanups on the astrodynamics side of numerical propagation,\nwith no public API change and bit-identical numerical results:\n\n- Dynamics::DynamicalEquations: reuse a per-context scratch buffer for the\n  reduced read state instead of heap-allocating a fresh VectorXd for every\n  dynamics at every derivative evaluation. The buffer lives in the\n  Dynamics::Context copies bound into the system-of-equations wrapper\n  (per-propagation), not in the shared Dynamics objects.\n\n- NumericalSolver::integrateTime(State, Instant, ...): defer construction\n  of the observed State objects (one per accepted integration step) until\n  accessObservedStates()/getObservedStates() is actually called, and build\n  them from a const reference to the underlying solver's observed state\n  vectors instead of a by-value copy. Propagator::calculateStateAt no\n  longer pays for building thousands of State objects nobody reads;\n  Segment and other consumers get identical states, built lazily.\n\nMeasured (Release, EGM96 10x10 + PositionDerivative, RK4 fixed 5 s):\n- loop overhead (PositionDerivative-only, 17,280 steps): 1.05 -> 0.78 us/step\n- derivative-evaluation glue with 5 dynamics: 0.222 -> 0.173 us/eval\n- 1-day propagation: ~991 -> ~950 ms, 7-day: ~4.76 -> ~4.65 s (medians of\n  3 interleaved A/B runs); final states bit-identical to baseline\n\nCo-Authored-By: Claude Fable 5 <noreply@anthropic.com>\n\n* perf: avoid body-frame rotation round trip in ThirdBodyGravity\n\nThirdBodyGravity::computeContribution evaluated the third body's\ngravitational field twice via Celestial::getGravitationalFieldAt, each\ncall rotating the query position into the body's own ephemeris-attached\nframe (a full SPICE sxform_c lookup for SPICE-backed bodies) and then\nrotating the resulting acceleration back out via inFrame. For a\npoint-mass (spherical) gravitational model, the field does not depend\non the body's orientation, so this rotate-in/rotate-out round trip is\npure overhead: the same result can be obtained from the body's position\nalone (Celestial::getPositionIn, a pure translation lookup) combined\nwith mu (Celestial::getGravitationalParameter) via the closed-form\ntwo-term third-body formula, with no per-call orientation lookups.\n\nSince ThirdBodyGravity accepts an arbitrary Shared<const Celestial>,\nnothing guarantees the attached model is point-mass (e.g. a\nhigher-fidelity spherical-harmonics model could be attached). A new\nprivate static helper, IsPointMassGravitationalModel, dynamic_casts the\nCelestial's gravitational model to Spherical (the generic point-mass\nmodel) or to Sun/Moon/Earth with their respective Type::Spherical\n(their only non-orientation-dependent type); this is computed once at\nconstruction and cached. The fast path is only taken when this\npositively identifies a point mass; otherwise computeContribution falls\nback unchanged to the original two-call getGravitationalFieldAt +\ninFrame path, so behavior is unaffected for any other configuration.\n\nStandalone micro-benchmark (Release, unique instants per call):\n  Sun (SPICE):                17.5 -> 15.5 us/call (~12% faster)\n  Moon (SPICE):               19.3 -> 17.1 us/call (~12% faster)\n  Sun (analytical ephemeris):  9.9 ->  7.4 us/call (~25% faster)\n  Moon (analytical ephemeris): 10.6 ->  8.2 us/call (~22% faster)\n\nAdded tests verifying the fast path numerically matches the generic\npath for Sun::Default/Moon::Default/Sun::Analytical/Moon::Analytical,\nand a test with a synthetic non-spherical (WGS84) third body proving\nthe generic fallback path is genuinely exercised.\n\nCo-Authored-By: Claude Fable 5 <noreply@anthropic.com>\n\n* feat: leverage new celestial method\n\n---------\n\nCo-authored-by: Claude Fable 5 <noreply@anthropic.com>",
          "timestamp": "2026-08-31T11:58:50-07:00",
          "tree_id": "c0af55a5f6b16d96af613d8c774d911ef53a61ff",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/a0791f6225a3e98a88a78bc8336939de75cb94be"
        },
        "date": 1788204645618,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2521631115.8999815,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2449648530.7999997 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 832.9022846666779,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 832.8102636666669 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2594.595138666667,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2594.3673599999984 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 9738.00668233334,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 9737.097828666667 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2564147665.2,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2563911075.100002 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 6038464379.500005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6037896449.4 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 6036600866.400004,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6036134787.800003 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 6036887322.599989,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 6036378160.900008 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.7918004500000961,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.7917882400000167 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 4.28455090500006,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.283186610999962 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 9.731540369000072,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 9.730424775000017 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 6.4221513530001175,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.420019477000039 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 8.863445354999953,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 8.862389829999984 s\nthreads: 1"
          }
        ]
      }
    ]
  }
}