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
            Frisbee 2015 maximum probability of collision algorithm.

            Computes an upper bound on the probability of collision for a high-relative velocity
            encounter when the position error covariance of only one object is known. The unknown
            covariance is replaced by the critical degenerate covariance that maximizes the
            probability of collision.

            The result is an upper bound: a value below an action threshold shows that no risk
            mitigation is required, while a value above it is inconclusive.

            Ref: J.H. Frisbee, "An Upper Bound On High Speed Satellite Collision Probability When
            Only One Object Has Position Uncertainty Information." AAS 15-717, 2015.
            https://ntrs.nasa.gov/citations/20150014495
        )doc"
    )

        .def(
            init<const Derived&, const Shared<const Frame>&, const bool&>(),
            R"doc(
                Constructor.

                Args:
                    relative_velocity_threshold (Derived, optional): A relative velocity threshold below which the
                        close approach is considered low-relative velocity and the algorithm is not applicable.
                        Defaults to 100 m/s.
                    frame (Frame, optional): An inertial (or quasi-inertial) frame to use. Defaults to Frame.GCRF().
                    fall_back_to_object_2_covariance (bool, optional): Whether to fall back to Object 2 covariance
                        when Object 1 has no position uncertainty. Defaults to True.
            )doc",
            arg_v(
                "relative_velocity_threshold",
                Derived(100.0, Derived::Unit::MeterPerSecond()),
                "Derived(100.0, Derived.Unit.meter_per_second())"
            ),
            arg_v("frame", Frame::GCRF(), "Frame.GCRF()"),
            arg("fall_back_to_object_2_covariance") = true
        )

        ;
}
