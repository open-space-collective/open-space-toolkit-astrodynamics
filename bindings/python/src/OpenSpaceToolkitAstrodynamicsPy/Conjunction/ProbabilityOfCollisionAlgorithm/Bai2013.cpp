/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Bai2013.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013(
    pybind11::module& aModule
)
{
    using namespace pybind11;

    using ostk::core::type::Real;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::unit::Derived;
    using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

    using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
    using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Bai2013;

    class_<Bai2013, ProbabilityOfCollisionAlgorithm, Shared<Bai2013>>(
        aModule,
        "Bai2013",
        R"doc(
            Bai 2013 probability of collision algorithm as described in:

            Bai, Xian-Zong & Chen, Lei & Tang, Guo-Jin. (2013). Explicit expression of collision probability in terms
            of RSW and NTW components of relative position. Advances in Space Research. 52. 1078-1096.
            10.1016/j.asr.2013.05.034.

            The algorithm derives an explicit and a modified expression for the probability of collision under the
            following assumptions:
            - High-relative velocity encounter
            - Diagonal position covariance matrix in the RSW frame
            - Near-circular orbits (for the explicit formulation)
        )doc"
    )

        .def(
            init<const Derived&, const Real&, const Shared<const Frame>&, const Derived&>(),
            R"doc(
                Constructor.

                Args:
                    relative_velocity_threshold (Derived, optional): A relative velocity threshold below which the
                        close approach is considered low-relative velocity and the algorithm is not applicable.
                        Defaults to 100 m/s.
                    eccentricity_threshold (float, optional): An eccentricity threshold below which the explicit
                        formulation for near-circular orbits is used. Defaults to 0.001.
                    frame (Frame, optional): An inertial (or quasi-inertial) frame to use. Defaults to GCRF.
                    gravitational_parameter (Derived, optional): A gravitational parameter to use when computing the
                        eccentricities. Defaults to the Earth EGM96 gravitational parameter.

                Raises:
                    RuntimeError: If the frame is undefined or not quasi-inertial.
            )doc",
            arg_v(
                "relative_velocity_threshold",
                Derived(100.0, Derived::Unit::MeterPerSecond()),
                "Derived(100.0, Derived.Unit.meter_per_second())"
            ),
            arg_v("eccentricity_threshold", Real(0.001), "0.001"),
            arg_v("frame", Frame::GCRF(), "Frame.GCRF()"),
            arg_v(
                "gravitational_parameter",
                EarthGravitationalModel::EGM96.gravitationalParameter_,
                "Earth.EGM96.gravitational_parameter"
            )
        )

        .def(
            "identify_unsatisfied_assumptions",
            &Bai2013::identifyUnsatisfiedAssumptions,
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
            &Bai2013::computeProbabilityOfCollision,
            R"doc(
                Compute the probability of collision.

                The hard bodies are approximated by a combined hard-body radius, taken as the enclosing radius of
                their combined cross section in the encounter frame.

                The explicit expression is used when both objects' eccentricities are below the eccentricity
                threshold, the modified expression otherwise.

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
