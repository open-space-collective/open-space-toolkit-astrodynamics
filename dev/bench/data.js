window.BENCHMARK_DATA = {
  "lastUpdate": 1794264830863,
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
          "id": "01003e44b8f8276907c03ebf57a009b79353a2e3",
          "message": "fix: guard temporal condition solver against misuse (#743)\n\n* fix: guard temporal condition solver against misuse\n\n* fix: address pau's feedback and dont raise",
          "timestamp": "2026-10-29T12:29:47-07:00",
          "tree_id": "816bf8ff5da5b333ca561466a36543e146d10e8c",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/01003e44b8f8276907c03ebf57a009b79353a2e3"
        },
        "date": 1793304202096,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 1894134899.9000068,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1814686522 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 508.2506530000046,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 508.203989333334 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1922.071354666684,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1921.8382016666667 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 172.4460126666827,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 172.42801766666813 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 2008324948.7000103,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 2008141670.1000016 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4701597582.299991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4701179806.2 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4703608293.90001,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4703210740.599996 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4703044117.800028,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4702662200.299997 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.5606813700001112,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.5606504069999971 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 1.9153209159999278,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.9151399499999968 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 2.0614999189999708,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.061341228000032 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 2.6897152679999863,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.689457061999974 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 6.586582840999995,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.586061685999994 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 5.577909206999948,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.577078219000015 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 5.565082820000043,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 5.564610081000012 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 86.31825371599996,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 86.31033120100003 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.025949562999926457,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.02594819300003337 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 23.614250278235716,
            "unit": "us/iter",
            "extra": "iterations: 29651\ncpu: 23.612591042461464 us\nthreads: 1"
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
          "id": "2761730977ee557f02cba5a58653114d10de4ee6",
          "message": "fix: reset FSAL stepper before re-integrating event-crossing bracket (#756)\n\nintegrateTimeWithStepperImpl_ re-integrates the bracket around a detected\ncrossing starting from the pre-step state, but reused the stepper that had\nalready advanced to the post-step state. FSAL steppers (RungeKuttaDopri5)\ncache the derivative at their post-step state and reuse it as the first\nstage of the next step, so the first stage of the bracket was evaluated at\nthe wrong state, biasing the interpolated crossing state.\n\nRe-integrate on a copy of the stepper and reset() it when the stepper\nexposes reset().\n\nFixes #755\n\nCo-authored-by: Claude Sonnet 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-11-05T08:45:31Z",
          "tree_id": "55a7198bc8a6f685122cb98c0d0385eed68f6dbe",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/2761730977ee557f02cba5a58653114d10de4ee6"
        },
        "date": 1793870474346,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 2910316967.8999963,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1901870257.3 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 504.87460633333586,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 504.81380799999914 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 2072.199922666679,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 2071.988614333333 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 164.0898820000037,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 164.07829266666832 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1994236665.9999948,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1994102310.7000008 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 4688293792.599996,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4687633285.499999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 4693021721.499991,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4692288931.899998 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 4691844458.2999935,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 4691480867.0999975 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.5566585719999466,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.5566139140000246 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 1.885139487999993,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.884993138999988 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 2.0633385440000325,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.0631655949999868 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 2.6880567869999368,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.687870079000021 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 6.566993169,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 6.566455472999991 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 4.898303498000018,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.897917061000015 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 4.881984312999975,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 4.881608691999986 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 78.94885754899997,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 78.94270596500002 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.02584509899998011,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.025842536000027394 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 23.530765954589565,
            "unit": "us/iter",
            "extra": "iterations: 29772\ncpu: 23.529931512830114 us\nthreads: 1"
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
          "id": "f9eb08efe7655674f34cda033d8459da52bd2560",
          "message": "build!: migrate to nanobind and physics v15 / mathematics v5 / core v6 / io v5 (#761)\n\n* build!: adapt to open-space-toolkit-core v6, io v5, mathematics v5 and physics v15\n\nMove to open-space-toolkit-core 6, open-space-toolkit-io 5,\nopen-space-toolkit-mathematics 5 and open-space-toolkit-physics 15, and\nadapt every call site to their APIs.\n\n- CMake: FIND_PACKAGE (OpenSpaceToolkitCore 6), (OpenSpaceToolkitIO 5),\n  (OpenSpaceToolkitMathematics 5) and (OpenSpaceToolkitPhysics 15). The\n  development image installs the latest packages of those majors\n  (OSTK_*_MAJOR = 6 / 5 / 5 / 15).\n- Python: open-space-toolkit-core~=6.0, open-space-toolkit-io~=5.0,\n  open-space-toolkit-mathematics~=5.0 and open-space-toolkit-physics~=15.0\n  in pyproject.toml.in.\n- Upstream Real -> double: mathematics' Interval<Real> is Interval<double>\n  (VisibilityCriterion::AERInterval / AERMask / ElevationInterval members,\n  constructors and From* factories, their Python constructors, the Access\n  benchmark and the tests), and the Cuboid extents in the tests are\n  std::array<double, 3>. NumericalSolver passes its time step and\n  tolerances to mathematics' NumericalSolver as double (NaN is undefined\n  there too), so the NaN -> Real::Undefined() helper is gone, and\n  integrateTime hands integrateDuration an Array<double>. The upstream\n  getters that returned Real now return double: Real::toString(),\n  floor() and isNear() calls on them became utility::DoubleToString(),\n  std::floor() and std::abs() (library) or EXPECT_NEAR (tests).\n- Core's File/Directory::Undefined() and isDefined() were removed (core\n  2b2ed644): TLE::Load no longer checks aFile.isDefined(), the\n  CDM/OPM/TLE::Load(File::Undefined()) test cases are gone, and the\n  gravitational Earth models built in tests and in the validation parser\n  pass std::nullopt for their data directory (physics v15 takes a\n  std::optional<Directory>).\n- Physics' time::Time::Undefined() and isDefined() were removed (physics\n  v15): Orbit::SunSynchronous, COE::SunSynchronous and\n  COE::ComputeRaanFromLTAN no longer check their local time argument, and\n  the test cases that passed Time::Undefined() are gone.\n- Mathematics' d3::object::Composite::Undefined() was removed and an empty\n  composite is now defined. System::Undefined(), SatelliteSystem::\n  Undefined() and the SatelliteSystemBuilder default geometry use\n  Composite(Point::Undefined()) instead: a composite that holds an\n  undefined object is undefined, so System/SatelliteSystem::isDefined()\n  and getGeometry() behave as before (the tests use the same composite).\n- Const access* getters renamed or removed upstream are called by their\n  get* names: core (Array::accessFirst/accessLast, Object::accessReal/\n  accessString/accessInteger/accessDictionary/accessArray), mathematics\n  (Interval::accessLowerBound/accessUpperBound, Composite::accessObjectAt,\n  Intersection::accessComposite) and physics (Position/Velocity::\n  accessCoordinates/accessFrame, Celestial/Object::accessFrame,\n  Environment::accessCelestialObjectWithName/accessCentralCelestialObject/\n  accessObjectWithName/accessObjects, Celestial::accessGravitationalModel/\n  accessAtmosphericModel/accessEphemeris, frame::Manager::\n  accessFrameWithName, Frame::accessParent, Transform::accessTranslation/\n  accessVelocity/accessOrientation/accessAngularVelocity, Interval::\n  accessStart/accessEnd), in C++, Python (access_celestial_object_with_name,\n  access_frame, access_parent, access_name), the tools, notebooks and the\n  README. Object::getString() returns a const reference, so the OPM loader\n  copies it before replace(), and Transform::getOrientation() returns a\n  const reference, so ConstantThrust uses toNormalized().\n- NumericalSolver's deleted accessObservedStateVectors() hiding\n  declaration is gone (mathematics removed the base method), and the\n  deleted getObservedStateVectors() hides the base's new const-reference\n  getter.\n- ThirdBodyGravity.ComputeContribution: the expected Moon contribution is\n  updated to physics' SPICE TDB fix (physics f161dede, \"query SPICE at TDB\n  ephemeris time instead of TT\"), which moves the Moon by about 1.8 m and\n  the contribution by about 5e-9 relative.\nSteps 1-3 of this release listed these as the upstream boundaries.\n\nBREAKING CHANGE: requires open-space-toolkit-core 6, open-space-toolkit-io 5,\nopen-space-toolkit-mathematics 5 and open-space-toolkit-physics 15 (C++ and\nPython). VisibilityCriterion::AERInterval / AERMask / ElevationInterval and\nVisibilityCriterion::FromAERInterval / FromAERMask / FromElevationInterval\ntake Interval<double> instead of Interval<Real> (Python: RealInterval, as\nbefore). TLE::Load no longer throws ostk::core::error::runtime::Undefined\nfor an undefined File, and Orbit::SunSynchronous, COE::SunSynchronous and\nCOE::ComputeRaanFromLTAN no longer throw it for an undefined local time,\nbecause neither can be constructed any more.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>\n\n* build!: migrate python bindings from pybind11 to nanobind\n\nFollow open-space-toolkit-core v6.0.0 (ed59a510, nanobind v3.1.0) and move\nthe Python bindings from pybind11 to nanobind. The build files mirror core's,\nmathematics' and physics': nanobind is fetched with FetchContent (pinned to\nv3.1.0, as core, io, mathematics and physics, so that all the extension\nmodules share one nanobind ABI and their types resolve here), its runtime is\nbuilt once per Python version from nb_combined.cpp (NB_ADD_LIBRARY),\nPY_STRIP replaces pybind11_strip, and stubs come from nanobind's stubgen,\nwritten into the package directory so that the wheel ships them (with\npy.typed). No development image change is needed.\n\nPython 3.9 is dropped (nanobind 3 and core 6 both require Python >= 3.10):\nPYTHON_SEARCH_VERSIONS is 3.10-3.14, requires-python is \">=3.10\" and uv.lock\nis regenerated (it only loses the Python 3.9 resolution forks).\n\nMeasured on this repository's bindings (Release, Python 3.11, best of 7\ntimeit runs on a shared machine, so indicative only): ~2.5x faster on the\ngeometric mean of the call shapes below, and the extension module is 40%\nsmaller (3.48 MB -> 2.11 MB, both stripped).\n\n  call                                             pybind11   nanobind\n  State.get_size()                                   421 ns     100 ns\n  State.get_instant()                                982 ns     186 ns\n  State.get_coordinates() (VectorXd)                 846 ns     735 ns\n  State(instant, ndarray, frame, subsets)           5113 ns    1493 ns\n  State(instant, position, velocity)                1524 ns     540 ns\n  State == State                                     886 ns     579 ns\n  State + State                                     1557 ns     867 ns\n  CartesianPosition.default() (shared_ptr)           351 ns     105 ns\n  Orbit.get_state_at(instant)                       4511 ns    2269 ns\n\nThe bindings translate almost mechanically. The parts that do not:\n\n- `pybind11/operators.h`: the 54 `self == self`, `self != self` (25\n  classes) and `self + self` / `self - self` (State, Covariance)\n  definitions are explicit lambdas bound to their dunder names with\n  `is_operator()`, so that mismatched types return NotImplemented.\n- `arg_v(name, value, repr)` becomes `arg(name).sig(repr) = value` (132\n  arguments). None of the defaults is an instance of the class whose method\n  it belongs to, so no overload had to be split.\n- Trampolines: Dynamics, Thruster, EventCondition, GuidanceLaw, HardBody,\n  ProbabilityOfCollisionAlgorithm, CoordinateSubset and BrouwerLyddaneMean\n  use `NB_TRAMPOLINE` and `NB_OVERRIDE(_PURE)(_NAME)` with the same Python\n  method names as before (ProbabilityOfCollisionAlgorithm keeps its public\n  constructor, the base one being protected). Every `Shared<T>` holder\n  argument of `class_` (41 classes) is gone (nanobind/stl/shared_ptr.h).\n  Python subclass instances handed to C++ as shared pointers are now kept\n  alive by C++: a Python Dynamics stored in a Propagator keeps its Python\n  type and overrides after the last Python reference is dropped (pybind11\n  lost the Python part and handed back a plain Dynamics).\n- Boolean results (`Utility/BooleanCasting.hpp`): nanobind's bool caster\n  only accepts True and False, whereas pybind11 converted the value returned\n  by a Python callable with its truth value. Callbacks and overrides\n  commonly return numpy.bool_ (`lambda state: state.get_coordinates()[2] >\n  0.0`), which would raise std::bad_cast from inside C++. A more\n  specialized `type_caster<std::function<bool(Args...)>>` (BooleanCondition\n  evaluators, Access::Generator access and state filters,\n  TemporalConditionSolver conditions) and `OSTK_NB_OVERRIDE_PURE_NAME_BOOL`\n  (Dynamics/Thruster.is_defined, EventCondition.is_satisfied) convert the\n  returned value with bool(), as pybind11 did.\n- `GuidanceLaw.construct_ungated_guidance_law` takes self as a\n  `Shared<GuidanceLaw>`: the C++ method calls shared_from_this(), and\n  nanobind only gives a Python-created instance a shared owner when it is\n  converted to a shared pointer (pybind11's shared_ptr holder always had\n  one), so the plain member binding raised bad_weak_ptr.\n- The reference getters of Orbit (`get_model`, `get_kepler_model`,\n  `get_sgp4_model`, `get_propagated_model`, `get_tabulated_model`,\n  `get_celestial_object`) and orbit Model (`as_kepler`, `as_sgp4`,\n  `as_propagated`) use `rv_policy::reference_internal` instead of\n  `reference`, so the returned object keeps its owner alive.\n- `TLESolver.estimate`'s initial guess (`isinstance`, `.cast<T>()`) and\n  `NumericalSolver.integrate_time`'s system of equations use\n  `nanobind::cast<T>(object)`. `nanobind/stl/function.h` keeps the\n  `std::function` arguments and getters working with Python callables, and\n  `def_readonly` / `def_readwrite` become `def_ro` / `def_rw`.\n- Eigen: as in ostk.mathematics and ostk.physics, nanobind's caster only\n  accepts DLPack / buffer-protocol inputs, while `State(instant, [...],\n  frame, subsets)`, `LocalOrbitalFrameDirection([1.0, 0.0, 0.0], ...)` and\n  friends rely on lists and tuples. `Utility/EigenCasting.hpp` is\n  mathematics' header (same behaviour), extended with QLaw's Vector5d,\n  Matrix5d and 5x3 matrix: array inputs go through nanobind's caster, with a\n  `numpy.asarray` fallback and (N, 1) column inputs accepted as pybind11\n  did; returned copies are capsule-owned NumPy views.\n- pybind11 set `__hash__ = None` on classes that define `__eq__`; nanobind\n  keeps the identity hash. `Utility/Hash.hpp` (mathematics' helper) restores\n  the pybind11 rule at import for this module's classes. CoordinateSubset\n  keeps its value hash.\n- `Utility/ArrayCasting.hpp` is core's header (Array<T> list caster and the\n  STL casters).\n\nNew tests: test/test_trampoline.py (Python Dynamics, GuidanceLaw and\nEventCondition called back from a Propagator, a Thruster and a\nLogicalCondition, a Python Dynamics outliving its Python references, a\nmissing override raising, numpy.bool_ results), test/test_cross_module.py\n(physics Instant / Position / Velocity / Frame / Environment / Celestial,\nmathematics RealInterval / Composite / Quaternion and core String into and\nout of this module), test/test_eigen.py (VectorXd, Vector3d, Matrix3d,\nVector5d, Vector6d and 5x3 matrix inputs and outputs: list, tuple, int,\nfloat32, column, Fortran-order, strided and read-only inputs, copies,\nrejected shapes and types) and test/test_hash.py. Against the pybind11\nbuild of the previous commit, 40 of these 44 tests pass; the others are the\ndeliberate complex-array case, the Python Dynamics that pybind11 loses, and\ntwo cases using physics 15 getter names. No existing test changed.\n\nThe leak check (`python -X dev`, importing ostk.astrodynamics, and a script\nexercising the trampolines, callbacks, reference getters and Eigen casts)\nreports no leak. nanobind does report the objects of a cycle that runs\nthrough C++ and stays alive until interpreter shutdown, for example a\nmodule-level Propagator holding an instance of a Python Dynamics subclass\ndefined in the same module (instance -> class -> function globals ->\npropagator -> C++ shared pointer -> instance); deleting the propagator, or\ncreating it in a function, avoids it. pybind11 had the same cycle, without\na report.\n\nUser-visible behaviour changes:\n\n- Enums are Python `enum.Enum` subclasses, as in core v6, mathematics and\n  physics: `int(member)` raises TypeError (use `.value`), and `repr(member)`\n  is `Criterion.StrictlyPositive` instead of `<Criterion.StrictlyPositive:\n  0>`. `.name`, `.value`, `__members__`, equality and `Criterion(0)` are\n  unchanged.\n- bool arguments only accept True and False (pybind11 also accepted\n  numpy.bool_ and, when converting, any object with a truth value, such as\n  `1`). Values returned by Python callables and overrides keep pybind11's\n  truth-value conversion (see above).\n- None passed for a bound-class argument that does not allow it raises\n  TypeError; pybind11 passed a null pointer to C++.\n- A complex array passed for a real Eigen argument raises TypeError; pybind11\n  dropped the imaginary part with a ComplexWarning.\n- Error messages for failed argument conversions follow nanobind's wording\n  (\"incompatible function arguments\", unqualified type names); the\n  exception types are unchanged (TypeError). Calling a pure virtual method\n  that a Python subclass does not override still raises RuntimeError, with\n  nanobind's message.\n- `ostk.astrodynamics` ships `.pyi` stubs and `py.typed`; stubgen also\n  writes stubs for the package's pure-Python modules (converters,\n  pytrajectory, ...).\n\nNot addressed:\n\n- The CoordinateSubset trampoline looks up `add`, `subtract` and `inFrame`\n  (the C++ names), as with pybind11, so a Python `in_frame` override is not\n  called from C++.\n- `Utility/IterableConverter.hpp` and `Utility/MapConverter.hpp`\n  (Boost.Python, never included) are left as they were.\n- The `BUILD_WHEELS_IN_CMAKE=OFF` branch still generates no stubs (as in\n  core).\n- `EventCondition.clone()` overridden in Python hands C++ a pointer owned by\n  Python (as with pybind11).\n- Compile time not measured.\n\nBREAKING CHANGE: Python 3.9 is no longer supported; enums are enum.Enum (no\nint()); bool arguments only accept True/False; None is rejected for\nbound-class arguments that do not allow it; complex arrays are rejected for\nreal Eigen arguments.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>\n\n* refactor!: remove the Undefined / isDefined sentinels removed by physics 15 and mathematics 5\n\nopen-space-toolkit-physics 15.0.0 and open-space-toolkit-mathematics 5.0.1\nremoved Undefined() / isDefined() from Instant, Duration, Interval, Angle,\nLength, Mass, Position, Velocity, Frame, Transform, Celestial, Environment,\nPoint, Composite, Matrix3d and Interval<double>, and Earth's magnetic and\natmospheric model types no longer have an Undefined enumerator. This\nmigrates the library to those releases the way physics did: where \"no\nvalue\" is a real state it is std::optional (None in Python), guards that\nonly checked the sentinel are deleted, and the invariants the sentinel\nencoded moved into constructors.\n\n- Removed Undefined() / isDefined() (and the Python undefined / is_defined)\n  from State, StateBuilder, Propagator, NumericalSolver, Trajectory and its\n  Model, LocalOrbitalFrameFactory / Direction, Orbit and orbit::Model,\n  COE, ModifiedEquinoctial, SGP4FullPrecision, BrouwerLyddaneMean, OPM,\n  CDM, CloseApproach and its Generator, Access, access / eclipse Generator,\n  System, SatelliteSystem, Maneuver, Profile and its Model, Transform,\n  Tabulated, and Dynamics (the pure virtual isDefined of Dynamics and of\n  Trajectory / Profile / Orbit models is gone; Python subclasses no longer\n  implement is_defined).\n- Pass: the instants are std::optional<Instant>, Type is derived from them\n  (Complete only when every instant is known) and Type::Undefined is gone.\n  Access::getTimeOfClosestApproach() and\n  Generator::FindTimeOfClosestApproach() are std::optional<Instant>.\n  CDM::RelativeMetadata / Data time, span, mass and relative state\n  members are optional. ConstantThrust::FromManeuver,\n  Maneuver::toConstantLocalOrbitalFrameDirectionManeuver,\n  Orbit::Frozen and COE::FrozenOrbit take std::optional<Angle>.\n- New constructor invariants: Tabulated (trajectory, orbit and profile\n  models) and Propagated reject fewer than two / zero states; State,\n  StateBuilder, LocalOrbitalFrameFactory, Orbit, SGP4FullPrecision and\n  profile Transform reject null frames, brokers or celestial objects;\n  Thruster requires a propulsion system; SatelliteSystemBuilder::build()\n  requires a dry mass, geometry and inertia tensor.\n  ModifiedEquinoctial::Cartesian / ::COE throw for parabolic, hyperbolic\n  and 180 degree inclination input instead of returning Undefined().\n  Propagator throws RuntimeError for an empty dynamics set.\n- Earth models: a missing magnetic / atmospheric model is nullptr / None.\n- Tests, validation, benchmarks, the python docs notebook and the python\n  tests follow; tests that only exercised a removed sentinel are deleted\n  and new tests cover the new optional and constructor behaviour.\n\nBREAKING CHANGE: see the removed and changed signatures above; no\nUndefined() / isDefined() remains in the C++ and Python APIs for the\ntypes listed, Pass::Type::Undefined and the Undefined enumerator of\nLocalOrbitalFrameTransformProvider::Type are removed, and the optional\nmembers and arguments listed above take or return std::optional / None.\n\nCo-Authored-By: Claude Sonnet 5.5 <noreply@anthropic.com>\n\n* chore: remove extraneous test file\n\n---------\n\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-11-09T14:23:11-08:00",
          "tree_id": "a0f662a492c8c766418c94ef94b7aad06ca17a4e",
          "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/commit/f9eb08efe7655674f34cda033d8459da52bd2560"
        },
        "date": 1794264829312,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Access | Ground Station <> TLE/iterations:10",
            "value": 200307777.19998696,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 172085876.29999998 ns\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 1 target | 2 weeks/iterations:3",
            "value": 267.32231400001183,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 267.2999293333332 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (GCRF out) | 1 target | 2 weeks/iterations:3",
            "value": 1098.8129390000079,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 1098.4434170000002 ms\nthreads: 1"
          },
          {
            "name": "Access | Tabulated (ITRF out) | 100 targets | 1 week | Elevation/iterations:3",
            "value": 61.95508233334598,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 61.9445246666667 ms\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | Spherical/iterations:10",
            "value": 1182030219.400002,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 1181823711.3999999 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1984 {100, 100}/iterations:10",
            "value": 3271842914.200005,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3271522818.9 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM1996 {100, 100}/iterations:10",
            "value": 3267143899.699994,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3266857884.7999983 ns\nthreads: 1"
          },
          {
            "name": "Propagation | Numerical | EGM2008 {100, 100}/iterations:10",
            "value": 3251014586.299988,
            "unit": "ns/iter",
            "extra": "iterations: 10\ncpu: 3250738545.500002 ns\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_550_to_580/iterations:1",
            "value": 0.31011407599999075,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.31010967800000344 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_SMA_550_to_580/iterations:1",
            "value": 1.057961521999971,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.0578442120000062 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_FiniteDifference_SMA_550_to_580/iterations:1",
            "value": 1.1337204269999575,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.133672340000004 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_Analytical_Frozen_550_to_580/iterations:1",
            "value": 1.4593391640000277,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 1.4592415490000121 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_ConstantThrust_Intrack_DutyCycle_550_to_580/iterations:1",
            "value": 2.9712054300000545,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 2.969525720000007 s\nthreads: 1"
          },
          {
            "name": "BM_Sequence_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 3.030706974999987,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 3.030490637000028 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_SMA_AoP/iterations:1",
            "value": 3.059678004000034,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 3.0593870409999795 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_FullHorizon/iterations:1",
            "value": 45.978665561000014,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 45.972650560999995 s\nthreads: 1"
          },
          {
            "name": "BM_Segment_QLaw_SSO_540_to_550_ExtractManeuvers/iterations:1",
            "value": 0.011262854000051448,
            "unit": "s/iter",
            "extra": "iterations: 1\ncpu: 0.011254622999985031 s\nthreads: 1"
          },
          {
            "name": "BM_QLaw_CalculateThrustAccelerationAt",
            "value": 11.174683674104127,
            "unit": "us/iter",
            "extra": "iterations: 64364\ncpu: 11.17395924740526 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}