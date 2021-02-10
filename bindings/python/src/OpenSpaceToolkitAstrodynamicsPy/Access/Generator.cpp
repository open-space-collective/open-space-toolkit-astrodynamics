////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Access/Generator.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <pybind11/functional.h> // To pass anonymous functions directly

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Access_Generator (        pybind11::module&         aModule                                     )
{

    using namespace pybind11 ;

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Map ;

    using ostk::physics::Environment ;
    using ostk::physics::coord::spherical::AER ;

    using ostk::astro::Access ;
    using ostk::astro::access::Generator ;

    class_<Generator, Shared<Generator>>(aModule, "Generator")

        .def(init<const Environment&>())

        .def(init<const Environment&, std::function<bool (const AER&)>&>())

        .def(init<const Environment&, std::function<bool (const AER&)>&, std::function<bool (const Access&)>&>())

        .def("is_defined", &Generator::isDefined)

        .def("compute_accesses", &Generator::computeAccesses)
        .def("set_step", &Generator::setStep)
        .def("set_tolerance", &Generator::setTolerance)
        .def("set_aer_filter", &Generator::setAerFilter)
        .def("set_access_filter", &Generator::setAccessFilter)

        .def_static("undefined", &Generator::Undefined)
        .def_static("aer_ranges", &Generator::AerRanges)
        .def_static("aer_mask", &Generator::AerMask)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
