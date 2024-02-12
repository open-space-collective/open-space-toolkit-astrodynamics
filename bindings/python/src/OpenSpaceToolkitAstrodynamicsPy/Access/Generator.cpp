/// Apache License 2.0

#include <pybind11/functional.h>  // To pass anonymous functions directly

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Access_Generator(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Map;
    using ostk::core::type::Real;
    using ostk::core::type::Shared;

    using ostk::physics::Environment;
    using ostk::physics::coordinate::spherical::AER;
    using ostk::physics::time::Duration;

    using ostk::astro::Access;
    using ostk::astro::access::Generator;
    using ostk::astro::trajectory::State;

    class_<Generator, Shared<Generator>>(
        aModule,
        "Generator",
        R"doc(
            An access generator.

        )doc"
    )

        .def(
            init<
                const Environment&,
                std::function<bool(const AER&)>&,
                std::function<bool(const Access&)>&,
                std::function<bool(const State&, const State&)>&,
                const Duration&,
                const Duration&>(),
            R"doc(
                Constructor.

                Args:
                    environment (Environment): The environment.
                    aer_filter (function): The AER filter.
                    access_filter (function): The access filter.
                    state_filter (function): The state filter.
                    step (Duration): The step.
                    tolerance (Duration): The tolerance.

            )doc",
            arg("environment"),
            arg("aer_filter") = none(),
            arg("access_filter") = none(),
            arg("state_filter") = none(),
            arg("step") = DEFAULT_STEP,
            arg("tolerance") = DEFAULT_TOLERANCE
        )

        .def(
            "is_defined",
            &Generator::isDefined,
            R"doc(
                Check if the generator is defined.

                Returns:
                    bool: True if the generator is defined, False otherwise.

            )doc"
        )

        .def(
            "get_step",
            &Generator::getStep,
            R"doc(
                Get the step.

                Returns:
                    Duration: The step.

            )doc"
        )
        .def(
            "get_tolerance",
            &Generator::getTolerance,
            R"doc(
                Get the tolerance.

                Returns:
                    Duration: The tolerance.

            )doc"
        )
        .def(
            "get_aer_filter",
            &Generator::getAerFilter,
            R"doc(
                Get the AER filter.

                Returns:
                    function: The AER filter.

            )doc"
        )
        .def(
            "get_access_filter",
            &Generator::getAccessFilter,
            R"doc(
                Get the access filter.

                Returns:
                    function: The access filter.

            )doc"
        )
        .def(
            "get_state_filter",
            &Generator::getStateFilter,
            R"doc(
                Get the state filter.

                Returns:
                    function: The state filter.

            )doc"
        )

        .def(
            "get_condition_function",
            &Generator::getConditionFunction,
            R"doc(
            Get the condition function.

            Args:
                from_trajectory (State): The state at the start of the interval.
                to_trajectory (State): The state at the end of the interval.

            Returns:
                function: The condition function.

        )doc",
            arg("from_trajectory"),
            arg("to_trajectory")
        )
        .def(
            "compute_accesses",
            &Generator::computeAccesses,
            R"doc(
                Compute the accesses.

                Args:
                    interval (Interval): The interval.
                    from_trajectory (State): The state at the start of the interval.
                    to_trajectory (State): The state at the end of the interval.

                Returns:
                    Accesses: The accesses.

            )doc",
            arg("interval"),
            arg("from_trajectory"),
            arg("to_trajectory")
        )
        .def(
            "set_step",
            &Generator::setStep,
            R"doc(
            Set the step.

            Args:
                step (Duration): The step.

        )doc",
            arg("step")
        )
        .def(
            "set_tolerance",
            &Generator::setTolerance,
            R"doc(
            Set the tolerance.

            Args:
                tolerance (Duration): The tolerance.

        )doc",
            arg("tolerance")
        )
        .def(
            "set_aer_filter",
            &Generator::setAerFilter,
            R"doc(
            Set the AER filter.

            Args:
                aer_filter (function): The AER filter.

        )doc",
            arg("aer_filter")
        )
        .def(
            "set_access_filter",
            &Generator::setAccessFilter,
            R"doc(
            Set the access filter.

            Args:
                access_filter (function): The access filter.

        )doc",
            arg("access_filter")
        )
        .def(
            "set_state_filter",
            &Generator::setStateFilter,
            R"doc(
            Set the state filter.

            Args:
                state_filter (function): The state filter.

        )doc",
            arg("state_filter")
        )

        .def_static(
            "undefined",
            &Generator::Undefined,
            R"doc(
                Get an undefined generator.

                Returns:
                    Generator: An undefined generator.
            )doc"
        )
        .def_static(
            "aer_ranges",
            &Generator::AerRanges,
            R"doc(
                Create an AER generator with ranges.

                Args:
                    azimuth_range (Interval): The azimuth range.
                    elevation_range (Interval): The elevation range.
                    range_range (Interval): The range range.
                    environment (Environment): The environment.

                Returns:
                    Generator: The AER generator.
            )doc",
            arg("azimuth_range"),
            arg("elevation_range"),
            arg("range_range"),
            arg("environment")
        )
        .def_static(
            "aer_mask",
            &Generator::AerMask,
            R"doc(
                Create an AER generator with a mask.

                Args:
                    azimuth_elevation_mask (Interval): The azimuth-elevation mask.
                    range_range (Interval): The range range.
                    environment (Environment): The environment.

                Returns:
                    Generator: The AER generator.
            )doc",
            arg("azimuth_elevation_mask"),
            arg("range_range"),
            arg("environment")
        )

        ;
}
