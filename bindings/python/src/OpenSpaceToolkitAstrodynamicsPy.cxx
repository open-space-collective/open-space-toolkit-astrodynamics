// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkitAstrodynamicsPy/Utilities/ArrayCasting.hpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Utilities/ShiftToString.hpp>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <OpenSpaceToolkitAstrodynamicsPy/Access.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/NumericalSolver.cpp>
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

    // Add python submodules to OpenSpaceToolkitAstrodynamicsPy
    OpenSpaceToolkitAstrodynamicsPy_Trajectory(m);
    OpenSpaceToolkitAstrodynamicsPy_Flight(m);
    OpenSpaceToolkitAstrodynamicsPy_Access(m);
    OpenSpaceToolkitAstrodynamicsPy_Conjunction(m);
    OpenSpaceToolkitAstrodynamicsPy_NumericalSolver(m
    );  // [TBI] This module will likely be moved to ostk-mathemetics in a future version
}
