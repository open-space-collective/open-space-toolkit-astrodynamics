/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>

using namespace pybind11;

using ostk::core::container::Map;
using ostk::core::container::Tuple;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::guidancelaw::QLaw;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_QLaw(pybind11::module& aModule)
{
    class_<QLaw, GuidanceLaw, Shared<QLaw>> qLaw(
        aModule,
        "QLaw",
        R"doc(
            This class implements the Q-law guidance law.

            - Ref: https://dataverse.jpl.nasa.gov/api/access/datafile/10307?gbrecs=true
            - Ref: https://www.researchgate.net/publication/370849580_Analytic_Calculation_and_Application_of_the_Q-Law_Guidance_Algorithm_Partial_Derivatives
            - Ref for derivations: https://dataverse.jpl.nasa.gov/api/access/datafile/13727?gbrecs=true

            The Q-law is a Lyapunov feedback control law developed by Petropoulos,
            based on analytic expressions for maximum rates of change of the orbit elements and
            the desired changes in the elements. Q, the proximity quotient, serves as a candidate Lyapunov
            function. As the spacecraft approaches the target orbit, Q decreases monotonically (becoming zero at the target orbit).

        )doc"
    )

        ;
    class_<QLaw::Parameters>(
        qLaw,
        "Parameters",
        R"doc(
            Q-law parameters.

        )doc"
    )
        .def_readonly(
            "m",
            &QLaw::Parameters::m,
            R"doc(
                Scaling parameter for Semi-Major Axis delta.

                Type:
                    int
            )doc"
        )
        .def_readonly(
            "n",
            &QLaw::Parameters::n,
            R"doc(
                Scaling parameter for Semi-Major Axis delta.

                Type:
                    int
            )doc"
        )
        .def_readonly(
            "r",
            &QLaw::Parameters::r,
            R"doc(
                Scaling parameter for Semi-Major Axis delta.

                Type:
                    int
            )doc"
        )
        .def_readonly(
            "b",
            &QLaw::Parameters::b,
            R"doc(
                Scaling parameter for Argument of Periapsis.

                Type:
                    float
            )doc"
        )
        .def_readonly(
            "k",
            &QLaw::Parameters::k,
            R"doc(
                Penalty parameter for periapsis.

                Type:
                    int
            )doc"
        )
        .def_readonly(
            "periapsis_weight",
            &QLaw::Parameters::periapsisWeight,
            R"doc(
                Periapsis weight.

                Type:
                    float
            )doc"
        )
        .def_readonly(
            "absolute_effectivity_threshold",
            &QLaw::Parameters::absoluteEffectivityThreshold,
            R"doc(
                Absolute effectivity threshold.

                Type:
                    Real
            )doc"
        )
        .def_readonly(
            "relative_effectivity_threshold",
            &QLaw::Parameters::relativeEffectivityThreshold,
            R"doc(
                Relative effectivity threshold.

                Type:
                    Real
            )doc"
        )

        .def(
            init<
                const Map<COE::Element, Tuple<double, double>>&,
                const Size&,
                const Size&,
                const Size&,
                const double&,
                const Size&,
                const double&,
                const Length&,
                const Real&,
                const Real&>(),
            R"doc(
                Constructor.

                Args:
                    element_weights (dict): Key-value pair of COE elements and the (weights, tolerances) for the targeter.
                    m (int): Scaling parameter for Semi-Major Axis delta. Default to 3.
                    n (int): Scaling parameter for Semi-Major Axis delta. Default to 4.
                    r (int): Scaling parameter for Semi-Major Axis delta. Default to 2.
                    b (float): Scaling parameter for Argument of Periapsis maximal change. Default to 0.01.
                    k (int): Penalty parameter for periapsis. Default to 100.
                    periapsis_weight (float): Periapsis weight. Default to 0.0.
                    minimum_periapsis_radius (Length): Minimum periapsis radius. Default to 6578.0 km.
                    absolute_effectivity_threshold (Real): Absolute effectivity threshold. Default to undefined (not used).
                    relative_effectivity_threshold (Real): Relative effectivity threshold. Default to undefined (not used).

            )doc",
            arg("element_weights"),
            arg("m") = 3,
            arg("n") = 4,
            arg("r") = 2,
            arg("b") = 0.01,
            arg("k") = 100,
            arg("periapsis_weight") = 0.0,
            arg_v("minimum_periapsis_radius", Length::Kilometers(6578.0), "Length.kilometers(6578.0)"),
            arg("absolute_effectivity_threshold") = Real::Undefined(),
            arg("relative_effectivity_threshold") = Real::Undefined()
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

        .def(
            "get_control_weight",
            &QLaw::Parameters::getControlWeight,
            R"doc(
                Get the control weight for a specific orbital element.

                Args:
                    element (COE.Element): The orbital element.

                Returns:
                    float: The control weight.
            )doc",
            arg("element")
        )

        .def(
            "get_convergence_thresholds",
            &QLaw::Parameters::getConvergenceThresholds,
            R"doc(
                Get the convergence thresholds.

                Returns:
                    np.array: The convergence thresholds.
            )doc"
        )

        .def(
            "get_minimum_periapsis_radius",
            &QLaw::Parameters::getMinimumPeriapsisRadius,
            R"doc(
                Get the minimum periapsis radius.

                Returns:
                    Length: The minimum periapsis radius.
            )doc"
        )

        .def(
            "set_control_weight",
            &QLaw::Parameters::setControlWeight,
            R"doc(
                Set the control weight for a specific orbital element.

                Args:
                    element (COE.Element): The orbital element.
                    weight (float): The weight to set.
            )doc",
            arg("element"),
            arg("weight")
        )

        ;

    enum_<QLaw::GradientStrategy>(
        qLaw,
        "GradientStrategy",
        R"doc(
            Gradient strategy.
        )doc"
    )

        .value("Analytical", QLaw::GradientStrategy::Analytical, "Analytical")
        .value("FiniteDifference", QLaw::GradientStrategy::FiniteDifference, "Finite Differenced")

        ;

    enum_<QLaw::COEDomain>(
        qLaw,
        "COEDomain",
        R"doc(
            Classical Orbital Elements domain.
        )doc"
    )

        .value("Osculating", QLaw::COEDomain::Osculating, "Osculating")
        .value("BrouwerLyddaneMeanLong", QLaw::COEDomain::BrouwerLyddaneMeanLong, "Brouwer Lyddane Mean Long")
        .value("BrouwerLyddaneMeanShort", QLaw::COEDomain::BrouwerLyddaneMeanShort, "Brouwer Lyddane Mean Short")

        ;

    qLaw

        .def("__str__", &(shiftToString<QLaw>))
        .def("__repr__", &(shiftToString<QLaw>))

        .def(
            init<
                const COE&,
                const Derived&,
                const QLaw::Parameters&,
                const QLaw::COEDomain&,
                const QLaw::GradientStrategy&>(),
            R"doc(
                Constructor.

                Args:
                    coe (COE): The target orbit described by Classical Orbital Elements.
                    gravitational_parameter (float): The gravitational parameter of the central body.
                    parameters (QLaw.Parameters): A set of parameters for the QLaw.
                    coe_domain (QLaw.COEDomain): The domain of the Classical Orbital Elements.
                    gradient_strategy (QLaw.GradientStrategy): The strategy used to compute the gradient dQ_dOE. Defaults to FiniteDifference.

            )doc",
            arg("target_coe"),
            arg("gravitational_parameter"),
            arg("parameters"),
            arg("coe_domain"),
            arg("gradient_strategy") = QLaw::GradientStrategy::FiniteDifference
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
            "access_parameters",
            &QLaw::accessParameters,
            return_value_policy::reference_internal,
            R"doc(
                Access the parameters.

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
            "get_gradient_strategy",
            &QLaw::getGradientStrategy,
            R"doc(
                Get the gradient strategy.

                Returns:
                    QLaw.GradientStrategy: The gradient strategy.
            )doc"
        )
        .def(
            "get_coe_domain",
            &QLaw::getCOEDomain,
            R"doc(
                Get the COE domain.

                Returns:
                    QLaw.COEDomain: The COE domain.
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

        .def(
            "compute_effectivity",
            &QLaw::computeEffectivity,
            R"doc(
                Compute the relative and absolute effectivity of the guidance law.

                Args:
                    state (State): The state from which to extract orbital elements.
                    thrust_acceleration (float): The thrust acceleration.
                    discretization_step_count (int): The number of discretization steps for the true anomaly. Default to 50.

                Returns:
                    tuple[float, float]: A tuple containing the relative and absolute effectivity.

            )doc",
            arg("state"),
            arg("thrust_acceleration"),
            arg("discretization_step_count") = 50
        )

        ;
}
