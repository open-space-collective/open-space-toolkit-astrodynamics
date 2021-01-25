////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Access/Generator.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>

// #include <OpenSpaceToolkitAstrodynamicsPy/Utilities/MapConverter.hpp>

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

    class_<Generator>(aModule, "Generator")

        .def(init<const Environment&>())

        .def("__init__",
                +[] (const Environment& anEnvironment, const pybind11::object& anAerFilter) -> Shared<Generator>
                {
                    return std::make_shared<Generator>(anEnvironment, anAerFilter) ;
                }
        )

        .def("__init__",
                +[] (const Environment& anEnvironment, const pybind11::object& anAerFilter, const pybind11::object& anAccessFilter) -> Shared<Generator>
                {
                    return std::make_shared<Generator>(anEnvironment, anAerFilter, anAccessFilter) ;
                }
        )

        .def("is_defined", &Generator::isDefined)

        .def("compute_accesses", &Generator::computeAccesses)
        .def("set_step", &Generator::setStep)
        .def("set_tolerance", &Generator::setTolerance)
        // .def("set_aer_filter", +[] (Generator& aGenerator, pybind11::object object) -> void { aGenerator.setAerFilter(object) ; })    // [TBR]
        // .def("set_access_filter", +[] (Generator& aGenerator, pybind11::object object) -> void { aGenerator.setAccessFilter(object) ; })   // [TBR]

        .def_static("undefined", &Generator::Undefined)
        .def_static("aer_ranges", &Generator::AerRanges)
        .def_static("aer_mask", &Generator::AerMask)

    ;

    // MapConverter()

    //     .from_python<Map<Real, Real>>()
    //     .to_python<Map<Real, Real>>()

    // ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
