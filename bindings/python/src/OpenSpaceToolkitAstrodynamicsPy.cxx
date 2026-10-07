/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Utility/ArrayCasting.hpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Utility/BooleanFunctionCasting.hpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Utility/EigenSequenceCasting.hpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Utility/ShiftToString.hpp>
#include <nanobind/eigen/dense.h>
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/complex.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

#include <OpenSpaceToolkitAstrodynamicsPy/Access.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Data.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Dynamics.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Eclipse.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Estimator.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/GuidanceLaw.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/RootSolver.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Solver.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory.cpp>

NB_MODULE(OpenSpaceToolkitAstrodynamicsPy, m)
{
    // Add optional docstring for package OpenSpaceToolkitAstrodynamicsPy
    m.doc() = "Orbit, attitude, access for OpenSpaceToolkit";

    // Change attribute __name__ to make OpenSpaceToolkitAstrodynamicsPy invisible in modules path
    m.attr("__name__") = "ostk.astrodynamics";

// Package version information
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif

    // [TBI] These modules will likely be moved to ostk-mathematics in a future version
    OpenSpaceToolkitAstrodynamicsPy_Solver(m);
    OpenSpaceToolkitAstrodynamicsPy_RootSolver(m);

    // Add python submodules to OpenSpaceToolkitAstrodynamicsPy
    OpenSpaceToolkitAstrodynamicsPy_Trajectory(m);
    OpenSpaceToolkitAstrodynamicsPy_Flight(m);
    OpenSpaceToolkitAstrodynamicsPy_Dynamics(m);
    OpenSpaceToolkitAstrodynamicsPy_Access(m);
    OpenSpaceToolkitAstrodynamicsPy_Conjunction(m);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition(m);
    OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw(m);
    OpenSpaceToolkitAstrodynamicsPy_Estimator(m);
    OpenSpaceToolkitAstrodynamicsPy_Data(m);
    OpenSpaceToolkitAstrodynamicsPy_Eclipse(m);
}
