/// Apache License 2.0

#include <pybind11/functional.h>  // To pass anonymous functions directly

#include <OpenSpaceToolkit/Astrodynamics/Solvers/TemporalConditionSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Solvers_TemporalConditionSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Size;
    using ostk::core::ctnr::Array;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Interval;

    using ostk::astro::solvers::TemporalConditionSolver;

    class_<TemporalConditionSolver>(aModule, "TemporalConditionSolver")

        .def(
            init<const Duration&, const Duration&, const Size&>(),
            arg("time_step"),
            arg("tolerance"),
            arg("maximum_iteration_count") = DEFAULT_MAXIMUM_ITERATION_COUNT
        )

        .def("get_time_step", &TemporalConditionSolver::getTimeStep)
        .def("get_tolerance", &TemporalConditionSolver::getTolerance)
        .def("get_maximum_iteration_count", &TemporalConditionSolver::getMaximumIterationCount)

        .def(
            "solve",
            overload_cast<const TemporalConditionSolver::Condition&, const Interval&>(
                &TemporalConditionSolver::solve, const_
            ),
            arg("condition"),
            arg("interval")
        )

        .def(
            "solve",
            overload_cast<const Array<TemporalConditionSolver::Condition>&, const Interval&>(
                &TemporalConditionSolver::solve, const_
            ),
            arg("conditions"),
            arg("interval")
        )

        ;
}
