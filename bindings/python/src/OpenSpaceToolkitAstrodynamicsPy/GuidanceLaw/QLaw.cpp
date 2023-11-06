/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::ctnr::Map;

using ostk::math::obj::Vector3d;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;
using ostk::physics::units::Derived;

using ostk::astro::GuidanceLaw;
using ostk::astro::guidancelaw::QLaw;
using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::solvers::FiniteDifferenceSolver;

void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_QLaw(pybind11::module& aModule)
{
    class_<QLaw::Parameters>(
        aModule,
        "QLawParameters",
        R"doc(
            Q-law parameters.

            Group:
                guidance-law
        )doc"
    )
        .def_readonly(
            "m",
            &QLaw::Parameters::m,
            R"doc(
                Scaling parameter for SMA delta.

                Type:
                    int
            )doc"
        )
        .def_readonly(
            "n",
            &QLaw::Parameters::n,
            R"doc(
                Scaling parameter for SMA delta.

                Type:
                    int
            )doc"
        )
        .def_readonly(
            "r",
            &QLaw::Parameters::r,
            R"doc(
                Scaling parameter for SMA delta.

                Type:
                    int
            )doc"
        )
        .def_readonly(
            "b",
            &QLaw::Parameters::b,
            R"doc(
                Scaling parameter for SMA delta.

                Type:
                    float
            )doc"
        )

        .def(
            init<const Map<COE::Element, Real>&, const Size&, const Size&, const Size&, const Real&>(),
            R"doc(
            Constructor.

            Args:
                element_weights (dict): Key-value pair of COE elements and the weights for the targeter.
                m (int): Scaling parameter for SMA delta.
                n (int): Scaling parameter for SMA delta.
                r (int): Scaling parameter for SMA delta.
                b (float): Scaling parameter for SMA delta.

        )doc",
            arg("element_weights"),
            arg("m"),
            arg("n"),
            arg("r"),
            arg("b")
        )

        .def(
            "get_control_weights",
            &QLaw::Parameters::getControlWeights,
            R"doc(
                Get the control weights.

                Returns:
                    np.array: The control weights.
            )doc"
        )

        ;

    class_<QLaw, GuidanceLaw, Shared<QLaw>>(
        aModule,
        "QLaw",
        R"doc(
            This class implements the Q-law guidance law.

            Ref: https://dataverse.jpl.nasa.gov/api/access/datafile/10307?gbrecs=true
            Ref: https://www.researchgate.net/publication/370849580_Analytic_Calculation_and_Application_of_the_Q-Law_Guidance_Algorithm_Partial_Derivatives
            Ref for derivations: https://dataverse.jpl.nasa.gov/api/access/datafile/13727?gbrecs=true

            The Q-law is a Lyapunov feedback control law developed by Petropoulos,
            based on analytic expressions for maximum rates of change of the orbit elements and
            the desired changes in the elements. Q, the proximity quotient, serves as a candidate Lyapunov
            function. As the spacecraft approaches the target orbit, Q decreases monotonically (becoming zero at the target orbit).

            Group:
                guidance-law
        )doc"
    )

        .def("__str__", &(shiftToString<QLaw>))
        .def("__repr__", &(shiftToString<QLaw>))

        .def(
            init<const COE&, const Derived&, const QLaw::Parameters&, const FiniteDifferenceSolver&>(),
            R"doc(
                Constructor.

                Args:
                    coe (COE): The target orbit described by Classical Orbital Elements.
                    gravitational_parameter (float): The gravitational parameter of the central body.
                    parameters (QLaw.Parameters): A set of parameters for the QLaw.
                    finite_difference_solver (FiniteDifferenceSolver): The finite difference solver.

            )doc",
            arg("target_coe"),
            arg("gravitational_parameter"),
            arg("parameters"),
            arg("finite_difference_solver")
        )

        .def(
            "get_parameters",
            &QLaw::getParameters,
            R"doc(
                Get the parameters.

                Returns:
                    QLaw.Parameters: The parameters.
            )doc"
        )
        .def(
            "get_target_coe",
            &QLaw::getTargetCOE,
            R"doc(
                Get the target COE.

                Returns:
                    COE: The target COE.
            )doc"
        )

        .def(
            "calculate_thrust_acceleration_at",
            &GuidanceLaw::calculateThrustAccelerationAt,
            R"doc(
                Calculate the thrust acceleration at the provided coordinates and instant.

                Args:
                    instant (Instant): Instant of computation.
                    position_coordinates (np.array): Position coordinates.
                    velocity_coordinates (np.array): Velocity coordinates.
                    thrust_acceleration (float): Thrust acceleration magnitude.
                    output_frame (Frame): The frame the acceleration is expressed in.

                Returns:
                    np.array: The acceleration.
            )doc",
            arg("instant"),
            arg("position_coordinates"),
            arg("velocity_coordinates"),
            arg("thrust_acceleration"),
            arg("output_frame")
        )

        ;
}
