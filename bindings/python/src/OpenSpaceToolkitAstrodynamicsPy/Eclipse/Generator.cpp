/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Eclipse/Generator.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Eclipse_Generator(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;
    using ostk::core::type::String;

    using ostk::physics::Environment;
    using ostk::physics::environment::utilities::Eclipse;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Interval;

    using ostk::astrodynamics::eclipse::Generator;
    using ostk::astrodynamics::Trajectory;

    class_<Generator, Shared<Generator>>(
        aModule,
        "Generator",
        R"doc(
            An eclipse generator.            
        )doc"
    )

        .def(
            init<const Environment&, const Duration&, const Duration&>(),
            R"doc(
                Constructor.

                Args:
                    environment (Environment): The Environment to use during the search. Defaults to the default environment.
                    search_step_size (Duration): The step size to use during the search. Defaults to 60 seconds.
                    search_tolerance (Duration): The tolerance to use during the search. Defaults to 1 millisecond.
            )doc",
            arg("environment") = Environment::Default(),
            arg("search_step_size") = Duration::Seconds(60.0),
            arg("search_tolerance") = Duration::Milliseconds(1.0)
        )

        .def(
            init<>(),
            R"doc(
                Default constructor with default parameters.
            )doc"
        )

        .def(
            "is_defined",
            &Generator::isDefined,
            R"doc(
                Check if eclipse generator is defined.

                Returns:
                    bool: True if eclipse generator is defined, False otherwise.
            )doc"
        )

        .def(
            "get_environment",
            &Generator::getEnvironment,
            R"doc(
                Get the environment.

                Returns:
                    Environment: The environment used during the search.
            )doc"
        )

        .def(
            "get_search_step_size",
            &Generator::getSearchStepSize,
            R"doc(
                Get the search step size.

                Returns:
                    Duration: The step size used during the search.
            )doc"
        )

        .def(
            "get_search_tolerance",
            &Generator::getSearchTolerance,
            R"doc(
                Get the search tolerance.

                Returns:
                    Duration: The tolerance used during the search.
            )doc"
        )

        .def(
            "generate",
            &Generator::generate,
            R"doc(
                Generate eclipses for a given trajectory over the provided analysis interval.

                Args:
                    trajectory (Trajectory): The trajectory to search for eclipses.
                    analysis_interval (Interval): The analysis interval.
                    occulted_celestial_object_name (str): The name of the occulted celestial object. Defaults to "Sun".
                    occulting_celestial_object_name (str): The name of the occulting celestial object. Defaults to "Earth".

                Returns:
                    Array[Eclipse]: Array of eclipses found within the analysis interval.
            )doc",
            arg("trajectory"),
            arg("analysis_interval"),
            arg("occulted_celestial_object_name") = "Sun",
            arg("occulting_celestial_object_name") = "Earth"
        )

        ;
}
