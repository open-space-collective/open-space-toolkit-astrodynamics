/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_Tabulated(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;
    using ostk::core::container::Array;

    using ostk::mathematics::curvefitting::Interpolator;
    using ostk::mathematics::object::MatrixXd;

    using ostk::physics::time::Instant;
    using ostk::physics::coordinate::Frame;

    using ostk::astrodynamics::Dynamics;
    using ostk::astrodynamics::dynamics::Tabulated;
    using ostk::astrodynamics::trajectory::state::CoordinateSubset;

    {
        class_<Tabulated, Dynamics, Shared<Tabulated>>(
            aModule,
            "Tabulated",
            R"doc(
                The tabulated dynamics.

            )doc"
        )
            .def(
                init<
                    const Array<Instant>&,
                    const MatrixXd&,
                    const Array<Shared<const CoordinateSubset>>&,
                    const Shared<const Frame>&,
                    const Interpolator::Type&>(),
                arg("instants"),
                arg("contribution_profile"),
                arg("coordinate_subsets"),
                arg("frame"),
                arg("interpolation_type") = DEFAULT_TABULATED_DYNAMICS_INTERPOLATION_TYPE,
                R"doc(
                    Constructor.

                    Args:
                        instants (list[Instant]): An array of instants.
                        contribution_profile (numpy.ndarray): A contribution profile.
                        coordinate_subsets (list[CoordinateSubset]): An array of coordinate subsets related to the contribution profile.
                        frame (Frame): A frame.
                        interpolation_type (Interpolator.Type, optional): The interpolation type. Defaults to Barycentric Rational.

                )doc"
            )

            .def("__str__", &(shiftToString<Tabulated>))
            .def("__repr__", &(shiftToString<Tabulated>))

            .def(
                "is_defined",
                &Tabulated::isDefined,
                R"doc(
                    Check if the tabulated dynamics is defined.

                    Returns:
                        bool: True if the tabulated dynamics is defined, False otherwise.

                )doc"
            )

            .def(
                "get_instants",
                &Tabulated::getInstants,
                R"doc(
                    Get the contribution instants.

                    Returns:
                        list[Instant]: The contribution instants.

                )doc"
            )

            .def(
                "get_contribution_profile",
                &Tabulated::getContributionProfile,
                R"doc(
                    Get the contribution profile.

                    Returns:
                        np.ndarray: The contribution profile.

                )doc"
            )

            .def(
                "get_frame",
                &Tabulated::getFrame,
                R"doc(
                    Get the reference frame.

                    Returns:
                        Frame: The reference frame.

                )doc"
            )

            .def(
                "get_interpolation_type",
                &Tabulated::getInterpolationType,
                R"doc(
                    Get the interpolation type used for each row of the contribution profile (they are all the same).

                    Returns:
                        Interpolator.Type: The interpolation type.

                )doc"
            )

            .def(
                "compute_contribution",
                &Tabulated::computeContribution,
                arg("instant"),
                arg("x"),
                arg("frame"),
                R"doc(
                    Compute the contribution from the contribution profile to the state vector.

                    Args:
                        instant (Instant): The instant of the state vector.
                        x (numpy.ndarray): The state vector.
                        frame (Frame): The reference frame.

                    Returns:
                        numpy.ndarray: The contribution from the contribution profile to the state vector.

                )doc"
            );
    }
}
