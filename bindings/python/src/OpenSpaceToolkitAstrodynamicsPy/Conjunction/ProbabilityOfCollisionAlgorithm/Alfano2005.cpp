/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Alfano2005.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005(
    pybind11::module& aModule
)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::unit::Derived;

    using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
    using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Alfano2005;

    class_<Alfano2005, ProbabilityOfCollisionAlgorithm, Shared<Alfano2005>>(
        aModule,
        "Alfano2005",
        R"doc(
            Alfano 2005 probability of collision algorithm as described in:

            Alfano, Salvatore. (2005). A Numerical Implementation of Spherical Object Collision Probability. The
            Journal of the Astronautical Sciences. 53. 103-109. 10.1007/BF03546397.

            The two-dimensional probability density function of the relative position is integrated over the
            combined hard-body circle in the encounter plane. The integral along the principal axis of the combined
            covariance is computed with Simpson's rule, while the integral along the other principal axis is expressed
            with error functions.

            The algorithm relies on the following assumptions:
            - High-relative velocity encounter
        )doc"
    )

        .def(
            init<const Derived&, const Shared<const Frame>&>(),
            R"doc(
                Constructor.

                Args:
                    relative_velocity_threshold (Derived, optional): A relative velocity threshold below which the
                        close approach is considered low-relative velocity and the algorithm is not applicable.
                        Defaults to 100 m/s.
                    frame (Frame, optional): An inertial (or quasi-inertial) frame to use. Defaults to GCRF.

                Raises:
                    RuntimeError: If the frame is undefined or not quasi-inertial.
            )doc",
            arg_v(
                "relative_velocity_threshold",
                Derived(100.0, Derived::Unit::MeterPerSecond()),
                "Derived(100.0, Derived.Unit.meter_per_second())"
            ),
            arg_v("frame", Frame::GCRF(), "Frame.GCRF()")
        )

        .def(
            "identify_unsatisfied_assumptions",
            &Alfano2005::identifyUnsatisfiedAssumptions,
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
            &Alfano2005::computeProbabilityOfCollision,
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
