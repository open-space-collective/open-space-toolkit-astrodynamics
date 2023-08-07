/// Apache License 2.0

#include <pybind11/functional.h>  // To pass anonymous functions directly

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Access_Generator(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Map;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;

    using ostk::physics::Environment;
    using ostk::physics::coord::spherical::AER;
    using ostk::physics::time::Duration;

    using ostk::astro::Access;
    using ostk::astro::access::Generator;
    using ostk::astro::trajectory::State;

    class_<Generator, Shared<Generator>>(aModule, "Generator")

        .def(
            init<
                const Environment&,
                std::function<bool(const AER&)>&,
                std::function<bool(const Access&)>&,
                std::function<bool(const State&, const State&)>&,
                const Duration&,
                const Duration&>(),
            arg("environment"),
            arg("aer_filter") = none(),
            arg("access_filter") = none(),
            arg("state_filter") = none(),
            arg("step") = DEFAULT_STEP,
            arg("tolerance") = DEFAULT_TOLERANCE
        )

        .def("is_defined", &Generator::isDefined)

        .def("get_step", &Generator::getStep)
        .def("get_tolerance", &Generator::getTolerance)
        .def("get_aer_filter", &Generator::getAerFilter)
        .def("get_access_filter", &Generator::getAccessFilter)
        .def("get_state_filter", &Generator::getStateFilter)

        .def("get_condition_function", &Generator::getConditionFunction, arg("from_trajectory"), arg("to_trajectory"))
        .def(
            "compute_accesses",
            &Generator::computeAccesses,
            arg("interval"),
            arg("from_trajectory"),
            arg("to_trajectory")
        )
        .def("set_step", &Generator::setStep, arg("step"))
        .def("set_tolerance", &Generator::setTolerance, arg("tolerance"))
        .def("set_aer_filter", &Generator::setAerFilter, arg("aer_filter"))
        .def("set_access_filter", &Generator::setAccessFilter, arg("access_filter"))
        .def("set_state_filter", &Generator::setStateFilter, arg("state_filter"))

        .def_static("undefined", &Generator::Undefined)
        .def_static(
            "aer_ranges",
            &Generator::AerRanges,
            arg("azimuth_range"),
            arg("elevation_range"),
            arg("range_range"),
            arg("environment")
        )
        .def_static(
            "aer_mask", &Generator::AerMask, arg("azimuth_elevation_mask"), arg("range_range"), arg("environment")
        )

        ;
}
