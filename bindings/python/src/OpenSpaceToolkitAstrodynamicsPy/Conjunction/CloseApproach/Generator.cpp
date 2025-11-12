/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach/Generator.hpp>

/**
 * @brief Bind the C++ Generator class to Python using pybind11.
 *
 * Exposes ostk::astrodynamics::conjunction::closeapproach::Generator to the given Python module, including:
 * - Constructor accepting a reference Trajectory, a step Duration, and a tolerance Duration (defaults: 30 minutes and 1 millisecond).
 * - Query methods: is_defined, get_reference_trajectory, get_step, get_tolerance.
 * - compute_close_approaches to find close approach events over a given Interval.
 * - Mutators: set_step, set_tolerance.
 * - Static constructor: undefined.
 *
 * @param aModule The pybind11::module to which the Generator binding will be added.
 */
inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_CloseApproach_Generator(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Interval;

    using ostk::astrodynamics::conjunction::CloseApproach;
    using ostk::astrodynamics::conjunction::closeapproach::Generator;
    using ostk::astrodynamics::Trajectory;

    class_<Generator>(
        aModule,
        "Generator",
        R"doc(
            Compute close approaches to a reference trajectory.

            This class computes close approach events between a reference trajectory and other object trajectories
            over a specified time interval. It uses a temporal condition solver to identify time periods when objects
            are approaching and then determines the exact time of closest approach.
        )doc"
    )

        .def(
            init<const Trajectory&, const Duration&, const Duration&>(),
            R"doc(
                Constructor.

                Args:
                    reference_trajectory (Trajectory): The reference trajectory for which to compute close approaches (Object 1).
                    step (Duration): The step to use during the close approach search. Set it to a duration smaller than the minimum possible
                        duration between two close approaches - which is about half an orbital period. Defaults to Duration.minutes(30.0).
                    tolerance (Duration): The tolerance to use during the close approach search. Defaults to Duration.milliseconds(1.0).
            )doc",
            arg("reference_trajectory"),
            arg_v("step", Duration::Minutes(30.0), "Duration.minutes(30.0)"),
            arg_v("tolerance", Duration::Milliseconds(1.0), "Duration.milliseconds(1.0)")
        )

        .def(
            "is_defined",
            &Generator::isDefined,
            R"doc(
                Check if the generator is defined.

                Returns:
                    bool: True if generator is defined, False otherwise.
            )doc"
        )

        .def(
            "get_reference_trajectory",
            &Generator::getReferenceTrajectory,
            R"doc(
                Get the reference trajectory.

                Returns:
                    Trajectory: The reference trajectory.
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
            "compute_close_approaches",
            &Generator::computeCloseApproaches,
            R"doc(
                Compute close approaches between the reference trajectory and another object over a search interval.

                Args:
                    trajectory (Trajectory): The trajectory of the other object (Object 2).
                    search_interval (Interval): The interval over which close approaches are searched.

                Returns:
                    list[CloseApproach]: Array of close approaches over the search interval (with Object 1 being the reference trajectory).
            )doc",
            arg("trajectory"),
            arg("search_interval")
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

        .def_static(
            "undefined",
            &Generator::Undefined,
            R"doc(
                Construct an undefined generator.

                Returns:
                    Generator: An undefined generator.
            )doc"
        )

        ;
}