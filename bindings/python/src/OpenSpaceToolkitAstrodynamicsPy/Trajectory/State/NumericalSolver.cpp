/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_NumericalSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;

    using ostk::physics::time::Instant;

    using MathNumericalSolver = ostk::mathematics::solver::NumericalSolver;
    using ostk::astrodynamics::EventCondition;
    using ostk::astrodynamics::RootSolver;
    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::state::NumericalSolver;

    typedef std::function<MathNumericalSolver::StateVector(
        const MathNumericalSolver::StateVector& x, MathNumericalSolver::StateVector& dxdt, const double t
    )>
        pythonSystemOfEquationsSignature;

    class_<NumericalSolver, MathNumericalSolver> numericalSolver(
        aModule,
        "NumericalSolver",
        R"doc(
                A numerical solver is used to integrate the trajectory of a dynamical system.

                The numerical solver can be used to integrate the trajectory of a dynamical system to a given instant,
                or to a set of instants, or until an `Event Condition` is met.

            )doc"
    );

    enum_<NumericalSolver::RootFindingStrategy>(
        numericalSolver,
        "RootFindingStrategy",
        R"doc(
            The strategy for finding the exact event crossing time during conditional integration.
        )doc"
    )
        .value("Linear", NumericalSolver::RootFindingStrategy::Linear, "Linear interpolation between step endpoints")
        .value(
            "Propagated",
            NumericalSolver::RootFindingStrategy::Propagated,
            "Re-integrate with smaller steps during root finding"
        )
        .value(
            "Boundary",
            NumericalSolver::RootFindingStrategy::Boundary,
            "Return first step boundary where condition is satisfied"
        );

    class_<NumericalSolver::ConditionSolution>(
        numericalSolver,
        "ConditionSolution",
        R"doc(
            The solution to an event condition.

        )doc"
    )
        .def_readonly(
            "state",
            &NumericalSolver::ConditionSolution::state,
            R"doc(
                The state of the trajectory.

                Type:
                    State
            )doc"
        )
        .def_readonly(
            "condition_is_satisfied",
            &NumericalSolver::ConditionSolution::conditionIsSatisfied,
            R"doc(
                Whether the event condition is satisfied.

                Type:
                    bool
            )doc"
        )
        .def_readonly(
            "iteration_count",
            &NumericalSolver::ConditionSolution::iterationCount,
            R"doc(
                The number of iterations required to find the solution.

                Type:
                    int
            )doc"
        )
        .def_readonly(
            "root_solver_has_converged",
            &NumericalSolver::ConditionSolution::rootSolverHasConverged,
            R"doc(
                Whether the root solver has converged.

                Type:
                    bool
            )doc"
        )

        ;

    {
        numericalSolver

            .def(
                init<
                    const NumericalSolver::LogType&,
                    const NumericalSolver::StepperType&,
                    const Real&,
                    const Real&,
                    const Real&,
                    const RootSolver&,
                    const NumericalSolver::RootFindingStrategy&>(),
                R"doc(
                    Constructor.

                    Args:
                        log_type (NumericalSolver.LogType): The type of logging.
                        stepper_type (NumericalSolver.StepperType): The type of stepper.
                        time_step (float): The time step.
                        relative_tolerance (float): The relative tolerance.
                        absolute_tolerance (float): The absolute tolerance.
                        root_solver (RootSolver, optional): The root solver. Defaults to RootSolver.Default().

                )doc",
                arg("log_type"),
                arg("stepper_type"),
                arg("time_step"),
                arg("relative_tolerance"),
                arg("absolute_tolerance"),
                arg_v("root_solver", RootSolver::Default(), "RootSolver.default()"),
            )

            .def(
                init<
                    const NumericalSolver::LogType&,
                    const NumericalSolver::StepperType&,
                    const Real&,
                    const Real&,
                    const Real&,
                    const RootSolver&,
                    const NumericalSolver::RootFindingStrategy&>(),
                R"doc(
                    Constructor.

                    Args:
                        log_type (NumericalSolver.LogType): The type of logging.
                        stepper_type (NumericalSolver.StepperType): The type of stepper.
                        time_step (float): The time step.
                        relative_tolerance (float): The relative tolerance.
                        absolute_tolerance (float): The absolute tolerance.
                        root_solver (RootSolver): The root solver.
                        root_finding_strategy (RootFindingStrategy): The root finding strategy.

                )doc",
                arg("log_type"),
                arg("stepper_type"),
                arg("time_step"),
                arg("relative_tolerance"),
                arg("absolute_tolerance"),
                arg("root_solver"),
                arg("root_finding_strategy")
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<NumericalSolver>))
            .def("__repr__", &(shiftToString<NumericalSolver>))

            .def(
                "is_defined",
                &NumericalSolver::isDefined,
                R"doc(
                    Check if the numerical solver is defined.

                    Returns:
                        bool: True if the numerical solver is defined, False otherwise.
                )doc"
            )

            .def(
                "get_observed_states",
                &NumericalSolver::getObservedStates,
                R"doc(
                    Get the observed states.

                    Returns:
                        list[State]: The observed states.
                )doc"
            )
            .def(
                "get_root_solver",
                &NumericalSolver::getRootSolver,
                R"doc(
                    Get the root solver.

                    Returns:
                        RootSolver: The root solver.
                )doc"
            )
            .def(
                "get_root_finding_strategy",
                &NumericalSolver::getRootFindingStrategy,
                R"doc(
                    Get the root finding strategy.

                    Returns:
                        RootFindingStrategy: The root finding strategy.
                )doc"
            )

            .def(
                "integrate_time",
                +[](NumericalSolver& aNumericalSolver,
                    const State& aState,
                    const Instant& anInstant,
                    const object& aSystemOfEquationsObject) -> State
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
                R"doc(
                    Integrate the trajectory to a given instant.

                    Args:
                        state (State): The initial state of the trajectory.
                        instant (Instant): The instant to integrate to.
                        system_of_equations (callable): The system of equations.

                    Returns:
                       State: The state at the requested time.

                )doc",
                arg("state"),
                arg("instant"),
                arg("system_of_equations")
            )

            .def(
                "integrate_time",
                +[](NumericalSolver& aNumericalSolver,
                    const State& aState,
                    const Array<Instant>& instants,
                    const object& aSystemOfEquationsObject) -> Array<State>
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
                R"doc(
                    Integrate the trajectory to a set of instants.

                    Args:
                        state (State): The initial state of the trajectory.
                        instants (list[Instant]): The instants to integrate to.
                        system_of_equations (callable): The system of equations.

                    Returns:
                        list[State]: The states at the requested times.

                )doc",
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
                    const EventCondition& anEventCondition) -> NumericalSolver::ConditionSolution
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
                R"doc(
                    Integrate the trajectory to a given instant, with an event condition.

                    Args:
                        state (State): The initial state of the trajectory.
                        instant (Instant): The instant to integrate to.
                        system_of_equations (callable): The system of equations.
                        event_condition (EventCondition): The event condition.

                    Returns:
                        ConditionSolution: The solution to the event condition.

                )doc",
                arg("state"),
                arg("instant"),
                arg("system_of_equations"),
                arg("event_condition")
            )

            .def_static(
                "default",
                &NumericalSolver::Default,
                R"doc(
                    Return the default numerical solver.

                    Returns:
                        NumericalSolver: The default numerical solver.
                )doc"
            )
            .def_static(
                "undefined",
                &NumericalSolver::Undefined,
                R"doc(
                    Return an undefined numerical solver.

                    Returns:
                        NumericalSolver: The undefined numerical solver.
                )doc"
            )
            .def_static(
                "fixed_step_size",
                &NumericalSolver::FixedStepSize,
                R"doc(
                    Return a Numerical Solver using a fixed stepper.

                    Returns:
                        NumericalSolver: The numerical solver.
                )doc",
                arg("stepper_type"),
                arg("time_step")
            )
            .def_static(
                "default_conditional",
                &NumericalSolver::DefaultConditional,
                R"doc(
                    Return the default conditional numerical solver.

                    Args:
                        state_logger (StateLogger, optional): The state logger. Defaults to None.

                    Returns:
                        NumericalSolver: The default conditional numerical solver.
                )doc",
                arg("state_logger") = nullptr
            )
            .def_static(
                "conditional",
                &NumericalSolver::Conditional,
                R"doc(
                    Return a conditional numerical solver.

                    Args:
                        time_step (float): The time step.
                        relative_tolerance (float): The relative tolerance.
                        absolute_tolerance (float): The absolute tolerance.
                        state_logger (StateLogger, optional): The state logger. Defaults to None.

                    Returns:
                        NumericalSolver: The conditional numerical solver.
                )doc",
                arg("time_step"),
                arg("relative_tolerance"),
                arg("absolute_tolerance"),
                arg("state_logger") = nullptr,
            )
            .def_static(
                "string_from_root_finding_strategy",
                &NumericalSolver::StringFromRootFindingStrategy,
                R"doc(
                    Return the string representation of a root finding strategy.

                    Args:
                        strategy (RootFindingStrategy): The root finding strategy.

                    Returns:
                        str: The string representation.
                )doc",
                arg("strategy")
            );
    }
}
