/// Apache License 2.0

#include <OpenSpaceToolkitAstrodynamicsPy/Utilities/ArrayCasting.hpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Utilities/ShiftToString.hpp>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <OpenSpaceToolkitAstrodynamicsPy/Access.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/NumericalSolver.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/RootSolver.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Solvers.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory.cpp>

PYBIND11_MODULE(OpenSpaceToolkitAstrodynamicsPy, m)
{
    // Add optional docstring for package OpenSpaceToolkitAstrodynamicsPy
    m.doc() = "Orbit, attitude, access for OpenSpaceToolkit";

    // Add __path__ attribute to python package
    m.attr("__path__") = "ostk.astrodynamics";

    // Change attribute __name__ to make OpenSpaceToolkitAstrodynamicsPy invisible in modules path
    m.attr("__name__") = "ostk.astrodynamics";

// Package version information
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif

    // [TBI] These modules will likely be moved to ostk-mathematics in a future version
    OpenSpaceToolkitAstrodynamicsPy_Solvers(m);
    OpenSpaceToolkitAstrodynamicsPy_NumericalSolver(m);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition(m);
    OpenSpaceToolkitAstrodynamicsPy_RootSolver(m);

    // Add python submodules to OpenSpaceToolkitAstrodynamicsPy
    OpenSpaceToolkitAstrodynamicsPy_Flight(m);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory(m);
    OpenSpaceToolkitAstrodynamicsPy_Access(m);
    OpenSpaceToolkitAstrodynamicsPy_Conjunction(m);
}
