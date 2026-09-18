/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Frisbee2015.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015(
    pybind11::module& aModule
)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::unit::Derived;

    using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
    using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Frisbee2015;

    class_<Frisbee2015, ProbabilityOfCollisionAlgorithm, Shared<Frisbee2015>>(
        aModule,
        "Frisbee2015",
        R"doc(
            Frisbee 2015 maximum probability of collision algorithm as described in:

            Frisbee, J.H. (2015). An Upper Bound On High Speed Satellite Collision Probability When Only One Object
            Has Position Uncertainty Information. AAS 15-717.

            The algorithm computes an upper bound on the probability of collision for a high relative speed encounter
            when the position error covariance of only one object is known. The unknown covariance is replaced by the
            critical degenerate covariance that maximizes the probability of collision.

            The result is an upper bound: a value below an action threshold shows that no risk mitigation is required,
            while a value above it is inconclusive.

            The following assumptions apply:
            - High relative speed encounter
            - Position uncertainty available for Object 1, or for at least one of the objects when the Object 2
            covariance is considered

            Ref: https://ntrs.nasa.gov/citations/20150014495
        )doc"
    )

        .def(
            init<
                const std::optional<Shared<const ProbabilityOfCollisionAlgorithm>>&,
                const bool&,
                const bool&,
                const Derived&,
                const Shared<const Frame>&>(),
            R"doc(
                Constructor.

                Frisbee's closed-form solution degrades when the combined hard-body radius is large compared to the
                smaller standard deviation of the critical combined covariance (sigma_min). In such cases, using a 2D
                probability of collision algorithm (e.g. Alfano2005) is recommended.

                Args:
                    probability_of_collision_algorithm (ProbabilityOfCollisionAlgorithm, optional): A probability of
                        collision algorithm used to compute the probability of collision from the critical
                        (degenerate) combined covariance. If None, Frisbee's closed-form solution is used. Defaults to
                        None.
                    consider_object_2_covariance (bool, optional): Whether to use the Object 2 covariance when Object 1
                        has no position uncertainty. If False, only the Object 1 covariance is used. Defaults to True.
                    consider_worst_case_when_both_covariance_are_present (bool, optional): Whether to use the worst
                        case (i.e. the covariance yielding the highest probability of collision) when both covariances
                        are present and the Object 2 covariance can be considered. Defaults to True.
                    relative_speed_threshold (Derived, optional): A relative speed threshold below which the close
                        approach is considered a low relative speed encounter and the algorithm is not applicable.
                        Defaults to 100 m/s.
                    frame (Frame, optional): An inertial (or quasi-inertial) frame to use. Defaults to Frame.GCRF().

                Raises:
                    RuntimeError: If the probability of collision algorithm is undefined, or if the frame is undefined
                        or not quasi-inertial.
            )doc",
            arg("probability_of_collision_algorithm") = none(),
            arg("consider_object_2_covariance") = true,
            arg("consider_worst_case_when_both_covariance_are_present") = true,
            arg_v(
                "relative_speed_threshold",
                Derived(100.0, Derived::Unit::MeterPerSecond()),
                "Derived(100.0, Derived.Unit.meter_per_second())"
            ),
            arg_v("frame", Frame::GCRF(), "Frame.GCRF()")
        )

        .def(
            "identify_unsatisfied_assumptions",
            &Frisbee2015::identifyUnsatisfiedAssumptions,
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
            &Frisbee2015::computeProbabilityOfCollision,
            R"doc(
                Compute the (maximum) probability of collision.

                The hard bodies are approximated by a combined hard-body radius, taken as the enclosing radius of
                their combined cross-section in the encounter frame.

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
