/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Foster1992.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_ProbabilityOfCollisionAlgorithm_Foster1992(
    pybind11::module& aModule
)
{
    using namespace pybind11;

    using ostk::core::type::Shared;
    using ostk::core::type::Size;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::unit::Angle;
    using ostk::physics::unit::Derived;

    using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
    using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Foster1992;

    class_<Foster1992, ProbabilityOfCollisionAlgorithm, Shared<Foster1992>>(
        aModule,
        "Foster1992",
        R"doc(
            Foster 1992 probability of collision algorithm as described in:

            Foster, J.L., Estes, H.S. (1992). A Parametric Analysis of Orbital Debris Collision Probability and
            Maneuver Rate for Space Vehicles. NASA/JSC-25898.

            The two-dimensional probability density function of the relative position is integrated over the
            combined hard-body circle in the encounter plane, in polar coordinates centered on the hard-body circle, as
            described in Equation 2 of:

            Alfano, Salvatore. (2007). Review of Conjunction Probability Methods for Short-term Encounters. AAS 07-148.

            The hard-body circle is sectioned into radial steps (annular sectors) and angular slices, and the
            probability density function is evaluated at the center of each resulting cell (midpoint rule).

            The algorithm relies on the following assumptions:
            - High relative speed encounter
        )doc"
    )

        .def(
            init<const Derived&, const Shared<const Frame>&, const Angle&, const Size&, const bool&>(),
            R"doc(
                Constructor.

                The default step sizes are those of Foster's numerical implementation, as reported by Alfano (2007):
                "the angle theta step size is 0.5 deg, and the radius r step size is OBJ/12" (OBJ being the combined
                hard-body radius). Alfano (2007) also notes that the accuracy degrades when the combined hard-body
                radius is smaller than the miss distance but larger than the smaller standard deviation, which smaller
                step sizes can remedy.

                Args:
                    relative_speed_threshold (Derived, optional): A relative speed threshold below which the
                        close approach is considered low relative speed and the algorithm is not applicable.
                        Defaults to 100 m/s.
                    frame (Frame, optional): An inertial (or quasi-inertial) frame to use. Defaults to GCRF.
                    angular_step_size (Angle, optional): An angular step size of the polar integration, within
                        (0, 360] deg. If it does not divide a full turn evenly, the number of angular slices is rounded
                        up (and the step size reduced accordingly). Defaults to 0.5 deg.
                    number_of_radial_steps (int, optional): A number of radial steps of the polar integration (i.e.
                        the radial step size is the combined hard-body radius divided by this number), strictly
                        positive. Defaults to 12.
                    increase_number_of_radial_steps_if_needed (bool, optional): Whether to increase the number of
                        radial steps (i.e. reduce the radial step size) if needed so that the smaller standard
                        deviation also spans at least the given number of radial steps, so that the probability
                        density function is resolved when the combined hard-body radius is large compared to the
                        smaller standard deviation. This avoids the above-mentioned accuracy degradation at the expense
                        of a slower computation. However, this type of scenarios are rare in practice.
                        Defaults to True.

                Raises:
                    RuntimeError: If the frame is undefined or not quasi-inertial, if the angular step size is
                        undefined or not within (0, 360] deg, or if the number of radial steps is zero.
            )doc",
            arg_v(
                "relative_speed_threshold",
                Derived(100.0, Derived::Unit::MeterPerSecond()),
                "Derived(100.0, Derived.Unit.meter_per_second())"
            ),
            arg_v("frame", Frame::GCRF(), "Frame.GCRF()"),
            arg_v("angular_step_size", Angle::Degrees(0.5), "Angle.degrees(0.5)"),
            arg("number_of_radial_steps") = 12,
            arg("increase_number_of_radial_steps_if_needed") = true
        )

        .def(
            "identify_unsatisfied_assumptions",
            &Foster1992::identifyUnsatisfiedAssumptions,
            R"doc(
                Identify unsatisfied assumptions for a close approach.

                Args:
                    close_approach (CloseApproach): A close approach.

                Returns:
                    list[str]: A non-exhaustive array of unsatisfied assumption descriptions.
            )doc",
            arg("close_approach")
        )

        .def(
            "compute_probability_of_collision",
            &Foster1992::computeProbabilityOfCollision,
            R"doc(
                Compute the probability of collision.

                The hard bodies are approximated by a combined hard-body radius, taken as the enclosing radius of
                their combined cross section in the encounter frame.

                Args:
                    close_approach (CloseApproach): A close approach.
                    hard_body_1 (HardBody): The hard body of Object 1.
                    hard_body_2 (HardBody): The hard body of Object 2.

                Returns:
                    float: The probability of collision.
            )doc",
            arg("close_approach"),
            arg("hard_body_1"),
            arg("hard_body_2")
        )

        ;
}
