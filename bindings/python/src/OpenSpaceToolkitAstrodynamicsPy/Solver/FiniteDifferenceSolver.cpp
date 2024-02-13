/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Solver/FiniteDifferenceSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Solvers_FiniteDifferenceSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::container::Size;

    using ostk::mathematics::object::VectorXd;
    using ostk::mathematics::object::MatrixXd;

    using ostk::physics::time::Instant;
    using ostk::physics::time::Duration;

    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::solver::FiniteDifferenceSolver;

    class_<FiniteDifferenceSolver> finiteDifferenceSolver(
        aModule,
        "FiniteDifferenceSolver",
        R"doc(
            A Finite Difference Solver to compute the gradient, and jacobian of a function.

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
            init<const ostk::astrodynamics::solver::FiniteDifferenceSolver::Type&, const Real&, const Duration&>(),
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
            "compute_jacobian",
            +[](const ostk::astrodynamics::solver::FiniteDifferenceSolver& solver,
                const State& aState,
                const Array<Instant>& anInstantArray,
                const std::function<MatrixXd(const State&, const Array<Instant>&)>& generateStateCoordinates,
                const Size& aCoordinatesDimension) -> MatrixXd
            {
                return solver.computeJacobian(aState, anInstantArray, generateStateCoordinates, aCoordinatesDimension);
            },
            R"doc(
                Compute the jacobian.

                Args:
                    state (State): The state.
                    instants (Array(Instant)): The instants at which to calculate the STM.
                    generate_states_coordinates (function): The function to get the states.
                    coordinates_dimension (int): The dimension of the coordinates produced by `generate_states_coordinates`.

                Returns:
                    np.array: The jacobian.
            )doc",
            arg("state"),
            arg("instants"),
            arg("generate_states_coordinates"),
            arg("coordinates_dimension")
        )
        .def(
            "compute_jacobian",
            +[](const ostk::astrodynamics::solver::FiniteDifferenceSolver& solver,
                const State& aState,
                const Instant& anInstant,
                const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates,
                const Size& aCoordinatesDimension) -> MatrixXd
            {
                return solver.computeJacobian(aState, anInstant, generateStateCoordinates, aCoordinatesDimension);
            },
            R"doc(
                Compute the jacobian.

                Args:
                    state (State): The state.
                    instant (Instant): The instant at which to calculate the STM.
                    generate_state_coordinates (function): The function to get the state.
                    coordinates_dimension (int): The dimension of the coordinates produced by `generate_state_coordinates`

                Returns:
                    np.array: The jacobian.
            )doc",
            arg("state"),
            arg("instant"),
            arg("generate_state_coordinates"),
            arg("coordinates_dimension")
        )
        .def(
            "compute_gradient",
            [](const ostk::astrodynamics::solver::FiniteDifferenceSolver& solver,
               const State& aState,
               std::function<VectorXd(const State&, const Instant&)> generateStateCoordinates) -> VectorXd
            {
                return solver.computeGradient(aState, generateStateCoordinates);
            },
            R"doc(
                Compute the gradient.

                Args:
                    state (State): The state.
                    generate_state_coordinates (function): The function to generate the state coordinates.

                Returns:
                    np.array: The jacobian.
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
