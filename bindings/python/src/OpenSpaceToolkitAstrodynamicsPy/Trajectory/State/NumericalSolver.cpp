/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_NumericalSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;

    using ostk::physics::time::Instant;

    using MathNumericalSolver = ostk::astro::NumericalSolver;
    using ostk::astro::EventCondition;
    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::state::NumericalSolver;

    typedef std::function<MathNumericalSolver::StateVector(
        const MathNumericalSolver::StateVector& x, MathNumericalSolver::StateVector& dxdt, const double t
    )>
        pythonSystemOfEquationsSignature;

    class_<NumericalSolver::ConditionSolution>(aModule, "ConditionSolution")

        .def_readonly("state", &NumericalSolver::ConditionSolution::state)
        .def_readonly("condition_is_satisfied", &NumericalSolver::ConditionSolution::conditionIsSatisfied)
        .def_readonly("iteration_count", &NumericalSolver::ConditionSolution::iterationCount)
        .def_readonly("root_solver_has_converged", &NumericalSolver::ConditionSolution::rootSolverHasConverged)

        ;

    {
        class_<NumericalSolver, MathNumericalSolver>(aModule, "NumericalSolver")

            .def(
                init<
                    const NumericalSolver::LogType&,
                    const NumericalSolver::StepperType&,
                    const Real&,
                    const Real&,
                    const Real&>(),
                arg("log_type"),
                arg("stepper_type"),
                arg("time_step"),
                arg("relative_tolerance"),
                arg("absolute_tolerance")
            )

            .def("get_observed_states", &NumericalSolver::getObservedStates)

            .def(
                "integrate_time",
                +[](NumericalSolver& aNumericalSolver,
                    const State& aState,
                    const Instant& anInstant,
                    const object& aSystemOfEquationsObject)
                {
                    const auto pythonDynamicsEquation =
                        pybind11::cast<pythonSystemOfEquationsSignature>(aSystemOfEquationsObject);

                    const NumericalSolver::SystemOfEquationsWrapper& systemOfEquations =
                        [&](const NumericalSolver::StateVector& x, NumericalSolver::StateVector& dxdt, const double t
                        ) -> void
                    {
                        dxdt = pythonDynamicsEquation(x, dxdt, t);
                    };

                    return aNumericalSolver.integrateTime(aState, anInstant, systemOfEquations);
                },
                arg("state"),
                arg("instant"),
                arg("system_of_equations")
            )

            .def(
                "integrate_time",
                +[](NumericalSolver& aNumericalSolver,
                    const State& aState,
                    const Array<Instant>& instants,
                    const object& aSystemOfEquationsObject)
                {
                    const auto pythonDynamicsEquation =
                        pybind11::cast<pythonSystemOfEquationsSignature>(aSystemOfEquationsObject);

                    const NumericalSolver::SystemOfEquationsWrapper& systemOfEquations =
                        [&](const NumericalSolver::StateVector& x, NumericalSolver::StateVector& dxdt, const double t
                        ) -> void
                    {
                        dxdt = pythonDynamicsEquation(x, dxdt, t);
                    };

                    return aNumericalSolver.integrateTime(aState, instants, systemOfEquations);
                },
                arg("state"),
                arg("instants"),
                arg("system_of_equations")
            )

            .def(
                "integrate_time",
                +[](NumericalSolver& aNumericalSolver,
                    const State& aState,
                    const Instant& anInstant,
                    const object& aSystemOfEquationsObject,
                    const EventCondition& anEventCondition)
                {
                    const auto pythonDynamicsEquation =
                        pybind11::cast<pythonSystemOfEquationsSignature>(aSystemOfEquationsObject);

                    const NumericalSolver::SystemOfEquationsWrapper& systemOfEquations =
                        [&](const NumericalSolver::StateVector& x, NumericalSolver::StateVector& dxdt, const double t
                        ) -> void
                    {
                        dxdt = pythonDynamicsEquation(x, dxdt, t);
                    };

                    return aNumericalSolver.integrateTime(aState, anInstant, systemOfEquations, anEventCondition);
                },
                arg("state"),
                arg("instant"),
                arg("system_of_equations"),
                arg("event_condition")
            )

            ;
    }
}
