/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_NumericalSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;

    using ostk::physics::time::Instant;

    using MathNumericalSolver = ostk::math::solvers::NumericalSolver;
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
        class_<NumericalSolver> numericalSolver(aModule, "NumericalSolver");

        numericalSolver

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

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<NumericalSolver>))
            .def("__repr__", &(shiftToString<NumericalSolver>))

            .def("is_defined", &NumericalSolver::isDefined)

            .def("get_stepper_type", &NumericalSolver::getStepperType)
            .def("get_log_type", &NumericalSolver::getLogType)
            .def("get_time_step", &NumericalSolver::getTimeStep)
            .def("get_relative_tolerance", &NumericalSolver::getRelativeTolerance)
            .def("get_absolute_tolerance", &NumericalSolver::getAbsoluteTolerance)
            .def("get_observed_states", &NumericalSolver::getObservedStates)
            .def("get_root_solver", &NumericalSolver::getRootSolver)

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

            .def_static("string_from_stepper_type", &NumericalSolver::StringFromStepperType, arg("stepper_type"))
            .def_static("string_from_log_type", &NumericalSolver::StringFromLogType, arg("log_type"))
            .def_static("default", &NumericalSolver::Default)
            .def_static("undefined", &NumericalSolver::Undefined)
            .def_static("default_conditional", &NumericalSolver::DefaultConditional)

            ;

        enum_<NumericalSolver::StepperType>(numericalSolver, "StepperType")

            .value("RungeKutta4", NumericalSolver::StepperType::RungeKutta4)
            .value("RungeKuttaCashKarp54", NumericalSolver::StepperType::RungeKuttaCashKarp54)
            .value("RungeKuttaFehlberg78", NumericalSolver::StepperType::RungeKuttaFehlberg78)
            .value("RungeKuttaDopri5", NumericalSolver::StepperType::RungeKuttaDopri5)

            ;

        enum_<NumericalSolver::LogType>(numericalSolver, "LogType")

            .value("NoLog", NumericalSolver::LogType::NoLog)
            .value("LogConstant", NumericalSolver::LogType::LogConstant)
            .value("LogAdaptive", NumericalSolver::LogType::LogAdaptive)

            ;
    }
}
