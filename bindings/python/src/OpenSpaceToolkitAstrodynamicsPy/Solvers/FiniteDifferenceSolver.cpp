/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Solvers/FiniteDifferenceSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Solvers_FiniteDifferenceSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;

    using ostk::math::obj::VectorXd;
    using ostk::math::obj::MatrixXd;

    using ostk::physics::time::Instant;
    using ostk::physics::time::Duration;

    using ostk::astro::trajectory::State;
    using ostk::astro::solvers::FiniteDifferenceSolver;

    class_<FiniteDifferenceSolver> finiteDifferenceSolver(
        aModule,
        "FiniteDifferenceSolver",
        R"doc(
            A Finite Difference Solver to compute the gradient, and state transition matrix of a function.

            Group:
                solvers
        )doc"
    );

    enum_<FiniteDifferenceSolver::Type>(
        finiteDifferenceSolver,
        "Type",
        R"doc(
            Type of finite difference scheme.

        )doc"
    )
        .value("Forward", FiniteDifferenceSolver::Type::Forward, "Forward difference scheme.")
        .value("Backward", FiniteDifferenceSolver::Type::Backward, "Backward difference scheme.")
        .value("Central", FiniteDifferenceSolver::Type::Central, "Central difference scheme.")

        ;

    finiteDifferenceSolver

        .def(
            init<const ostk::astro::solvers::FiniteDifferenceSolver::Type&>(),
            R"doc(
                Construct a FiniteDifferenceSolver.

                Args:
                    type (FiniteDifferenceSolver.Type): Type of finite difference scheme.

                Returns:
                    FiniteDifferenceSolver: The FiniteDifferenceSolver.
            )doc",
            arg("type")
        )

        .def("__str__", &(shiftToString<FiniteDifferenceSolver>))
        .def("__repr__", &(shiftToString<FiniteDifferenceSolver>))

        .def(
            "compute_state_transition_matrix",
            [](const ostk::astro::solvers::FiniteDifferenceSolver& solver,
               const State& aState,
               const Array<Instant>& anInstantArray,
               std::function<Array<State>(const State&, const Array<Instant>&)> getStates,
               const Real& aStepPercentage = 1e-3) -> Eigen::MatrixXd
            {
                return solver.computeStateTransitionMatrix(aState, anInstantArray, getStates, aStepPercentage);
            },
            R"doc(
                Compute the state transition matrix.

                Args:
                    state (State): The state.
                    instants (Array(Instant)): The instants at which to calculate the STM.
                    get_states (function): The function to get the states.
                    step_percentage (float): The step percentage. Defaults to 1e-3.

                Returns:
                    np.array: The state transition matrix.
            )doc",
            arg("state"),
            arg("instants"),
            arg("get_states"),
            arg("step_percentage")
        )
        .def(
            "compute_state_transition_matrix",
            [](const ostk::astro::solvers::FiniteDifferenceSolver& solver,
               const State& aState,
               const Instant& anInstant,
               std::function<State(const State&, const Instant&)> generateState,
               const Real& aStepPercentage = 1e-3) -> MatrixXd
            {
                return solver.computeStateTransitionMatrix(aState, anInstant, generateState, aStepPercentage);
            },
            R"doc(
                Compute the state transition matrix.

                Args:
                    state (State): The state.
                    instant (Instant): The instant at which to calculate the STM.
                    get_state (function): The function to get the state.
                    step_percentage (float): The step percentage. Defaults to 1e-3.

                Returns:
                    np.array: The state transition matrix.
            )doc",
            arg("state"),
            arg("instant"),
            arg("get_state"),
            arg("step_percentage")
        )
        .def(
            "compute_gradient",
            [](const ostk::astro::solvers::FiniteDifferenceSolver& solver,
               const State& aState,
               std::function<State(const State&, const Instant&)> generateState,
               const Duration& aStepSize = Duration::Seconds(1e-6)) -> VectorXd
            {
                return solver.computeGradient(aState, generateState, aStepSize);
            },
            R"doc(
                Compute the gradient.

                Args:
                    state (State): The state.
                    get_state (function): The function to get the state.
                    step_size (duration): The step size as a duration. Defaults to 1e-6 seconds.

                Returns:
                    np.array: The state transition matrix.
            )doc",
            arg("state"),
            arg("get_state"),
            arg("step_size")
        )

        .def_static(
            "string_from_type",
            &FiniteDifferenceSolver::StringFromType,
            R"doc(
                Convert a type to string.

                Args:
                    type (FiniteDifferenceSolver.Type): The type.

                Returns:
                    str: The string name of the type.
            )doc",
            arg("type")
        )

        ;
}
