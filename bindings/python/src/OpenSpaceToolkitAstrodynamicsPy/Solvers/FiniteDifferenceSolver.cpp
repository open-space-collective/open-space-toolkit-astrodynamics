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
            init<const ostk::astro::solvers::FiniteDifferenceSolver::Type&, const Real&, const Duration&>(),
            R"doc(
                Construct a FiniteDifferenceSolver.

                Args:
                    type (FiniteDifferenceSolver.Type): Type of finite difference scheme.
                    step_percentage (float): The step percentage to use for computing the STM.
                    step_duration (Duration): The step duration to use for computing the gradient.

                Returns:
                    FiniteDifferenceSolver: The FiniteDifferenceSolver.
            )doc",
            arg("type"),
            arg("step_percentage"),
            arg("step_duration")
        )

        .def("__str__", &(shiftToString<FiniteDifferenceSolver>))
        .def("__repr__", &(shiftToString<FiniteDifferenceSolver>))

        .def(
            "get_type",
            &FiniteDifferenceSolver::getType,
            R"doc(
                Get the type.

                Returns:
                    FiniteDifferenceSolver.Type: The type.
            )doc"
        )
        .def(
            "get_step_percentage",
            &FiniteDifferenceSolver::getStepPercentage,
            R"doc(
                Get the step percentage used for computing the STM.

                Returns:
                    float: The step percentage.
            )doc"
        )
        .def(
            "get_step_duration",
            &FiniteDifferenceSolver::getStepDuration,
            R"doc(
                Get the step duration used for computing the gradient.

                Returns:
                    Duration: The step duration.
            )doc"
        )

        .def(
            "compute_state_transition_matrix",
            +[](const ostk::astro::solvers::FiniteDifferenceSolver& solver,
                const State& aState,
                const Array<Instant>& anInstantArray,
                std::function<MatrixXd(const State&, const Array<Instant>&)> generateStateCoordinates) -> MatrixXd
            {
                return solver.computeStateTransitionMatrix(aState, anInstantArray, generateStateCoordinates);
            },
            R"doc(
                Compute the state transition matrix.

                Args:
                    state (State): The state.
                    instants (Array(Instant)): The instants at which to calculate the STM.
                    generate_states_coordinates (function): The function to get the states.

                Returns:
                    np.array: The state transition matrix.
            )doc",
            arg("state"),
            arg("instants"),
            arg("generate_states_coordinates")
        )
        .def(
            "compute_state_transition_matrix",
            +[](const ostk::astro::solvers::FiniteDifferenceSolver& solver,
                const State& aState,
                const Instant& anInstant,
                std::function<VectorXd(const State&, const Instant&)> generateStateCoordinates) -> MatrixXd
            {
                return solver.computeStateTransitionMatrix(aState, anInstant, generateStateCoordinates);
            },
            R"doc(
                Compute the state transition matrix.

                Args:
                    state (State): The state.
                    instant (Instant): The instant at which to calculate the STM.
                    generate_state_coordinates (function): The function to get the state.

                Returns:
                    np.array: The state transition matrix.
            )doc",
            arg("state"),
            arg("instant"),
            arg("generate_state_coordinates")
        )
        .def(
            "compute_gradient",
            [](const ostk::astro::solvers::FiniteDifferenceSolver& solver,
               const State& aState,
               std::function<VectorXd(const State&, const Instant&)> generateStateCoordinates) -> VectorXd
            {
                return solver.computeGradient(aState, generateStateCoordinates);
            },
            R"doc(
                Compute the gradient.

                Args:
                    state (State): The state.
                    generate_state_coordinates (function): The function to generate the state.

                Returns:
                    np.array: The state transition matrix.
            )doc",
            arg("state"),
            arg("generate_state_coordinates")
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

        .def_static(
            "default",
            &FiniteDifferenceSolver::Default,
            R"doc(
                Get the default Finite Difference Solver.

                Returns:
                    FiniteDifferenceSolver: The default Finite Difference Solver.
            )doc"
        )

        ;
}
