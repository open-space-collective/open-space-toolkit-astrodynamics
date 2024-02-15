/// Apache License 2.0

#include <pybind11/functional.h>  // To pass anonymous functions directly

#include <OpenSpaceToolkit/Astrodynamics/Solver/TemporalConditionSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Solver_TemporalConditionSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Size;
    using ostk::core::container::Array;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Interval;

    using ostk::astrodynamics::solver::TemporalConditionSolver;

    class_<TemporalConditionSolver>(
        aModule,
        "TemporalConditionSolver",
        R"doc(
            Given a set of conditions and a time interval, the solver computes all sub-intervals over which conditions are met.

        )doc"
    )

        .def(
            init<const Duration&, const Duration&, const Size&>(),
            R"doc(
                Constructor.

                Args:
                    time_step (Duration): The time step.
                    tolerance (Duration): The tolerance of the solver.
                    maximum_iteration_count (int): The maximum number of iterations allowed.

            )doc",
            arg("time_step"),
            arg("tolerance"),
            arg("maximum_iteration_count") = DEFAULT_MAXIMUM_ITERATION_COUNT
        )

        .def(
            "get_time_step",
            &TemporalConditionSolver::getTimeStep,
            R"doc(
                Get the time step.

                Returns:
                    Duration: The time step.
            )doc"
        )

        .def(
            "get_tolerance",
            &TemporalConditionSolver::getTolerance,
            R"doc(
                Get the tolerance.

                Returns:
                    Duration: The tolerance.
            )doc"
        )

        .def(
            "get_maximum_iteration_count",
            &TemporalConditionSolver::getMaximumIterationCount,
            R"doc(
                Get the maximum number of iterations allowed.

                Returns:
                    int: The maximum number of iterations allowed.
            )doc"
        )

        .def(
            "solve",
            overload_cast<const TemporalConditionSolver::Condition&, const Interval&>(
                &TemporalConditionSolver::solve, const_
            ),
            R"doc(
                Solve a temporal condition.

                Args:
                    condition (function): The condition to solve.
                    interval (Interval): The interval to solve the condition over.

                Returns:
                    Duration: The time at which the condition is satisfied.
            )doc",
            arg("condition"),
            arg("interval")
        )

        .def(
            "solve",
            overload_cast<const Array<TemporalConditionSolver::Condition>&, const Interval&>(
                &TemporalConditionSolver::solve, const_
            ),
            R"doc(
                Solve an array of temporal conditions.

                Args:
                    conditions (list): The conditions to solve.
                    interval (Interval): The interval to solve the conditions over.

                Returns:
                    list: The times at which the conditions are satisfied.
            )doc",
            arg("conditions"),
            arg("interval")
        )

        ;
}
