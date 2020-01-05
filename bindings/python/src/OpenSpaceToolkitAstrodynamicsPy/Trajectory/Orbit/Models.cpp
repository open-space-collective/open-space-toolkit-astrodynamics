////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Tabulated.cpp>
#include <bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/SGP4.cpp>
#include <bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Kepler.cpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models ( )
{

    boost::python::object module(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("ostk.astrodynamics.trajectory.orbit.models")))) ;

    boost::python::scope().attr("models") = module ;

    boost::python::scope scope = module ;

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler() ;
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_SGP4() ;
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Tabulated() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
