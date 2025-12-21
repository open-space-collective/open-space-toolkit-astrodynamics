/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach/Generator.hpp>

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
                        interval where both objects can be moving apart - which is about a quarter of an orbital period. Defaults to
                        Duration.minutes(20.0) - but it should be set lower for low velocity conjunctions as they tend to exhibit more than two
                        close approaches per orbit in a non deterministic manner.
                    tolerance (Duration): The tolerance to use during the close approach search. Defaults to Duration.milliseconds(1.0) - which
                        means that objects moving at 7km/s will be up to 7m away from their “true” position.
            )doc",
            arg("reference_trajectory"),
            arg_v("step", Duration::Minutes(20.0), "Duration.minutes(20.0)"),
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
