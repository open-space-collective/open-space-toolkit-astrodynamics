////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Access/Generator.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Utilities/MapConverter.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Access_Generator ( )
{

    using namespace boost::python ;

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Map ;

    using ostk::physics::Environment ;
    using ostk::physics::coord::spherical::AER ;

    using ostk::astro::Access ;
    using ostk::astro::access::Generator ;

    scope in_Generator = class_<Generator>("Generator", init<const Environment&>())

        .def
        (
            "__init__",
            make_constructor
            (
                +[] (const Environment& anEnvironment, const boost::python::object& anAerFilter) -> Shared<Generator>
                {
                    return std::make_shared<Generator>(anEnvironment, anAerFilter) ;
                }
            )
        )

        .def
        (
            "__init__",
            make_constructor
            (
                +[] (const Environment& anEnvironment, const boost::python::object& anAerFilter, const boost::python::object& anAccessFilter) -> Shared<Generator>
                {
                    return std::make_shared<Generator>(anEnvironment, anAerFilter, anAccessFilter) ;
                }
            )
        )

        .def("is_defined", &Generator::isDefined)

        .def("compute_accesses", &Generator::computeAccesses)
        .def("set_step", &Generator::setStep)
        .def("set_tolerance", &Generator::setTolerance)
        .def("set_aer_filter", +[] (Generator& aGenerator, boost::python::object object) -> void { aGenerator.setAerFilter(object) ; })
        .def("set_access_filter", +[] (Generator& aGenerator, boost::python::object object) -> void { aGenerator.setAccessFilter(object) ; })

        .def("undefined", &Generator::Undefined).staticmethod("undefined")
        .def("aer_ranges", &Generator::AerRanges).staticmethod("aer_ranges")
        .def("aer_mask", &Generator::AerMask).staticmethod("aer_mask")

    ;

    MapConverter()

        .from_python<Map<Real, Real>>()
        .to_python<Map<Real, Real>>()

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
