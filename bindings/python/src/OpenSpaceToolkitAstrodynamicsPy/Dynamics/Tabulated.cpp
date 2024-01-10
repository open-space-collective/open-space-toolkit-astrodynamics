/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_Tabulated(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;
    using ostk::core::ctnr::Array;

    using ostk::math::object::MatrixXd;

    using ostk::physics::time::Instant;

    using ostk::astro::Dynamics;
    using ostk::astro::dynamics::Tabulated;
    using ostk::astro::trajectory::state::CoordinatesSubset;

    {
        class_<Tabulated, Dynamics, Shared<Tabulated>>(
            aModule,
            "Tabulated",
            R"doc(
                The tabulated dynamics model.

                Group:
                    dynamics
            )doc"
        )
            .def(
                init<const Array<Instant>&, const MatrixXd&, const Array<Shared<const CoordinatesSubset>>&>(),
                arg("instants"),
                arg("acceleration_profile"),
                arg("write_coordinates_subsets"),
                R"doc(
                    Constructor.

                    Args:
                        instants (Array<Instant>): An array of instants.
                        acceleration_profile (numpy.ndarray): An acceleration profile.
                        write_coordinates_subsets (Array<Shared<CoordinatesSubset>>): An array of coordinates subsets to write to.

                )doc"
            )

            .def("__str__", &(shiftToString<Tabulated>))
            .def("__repr__", &(shiftToString<Tabulated>))

            .def(
                "is_defined",
                &Tabulated::isDefined,
                R"doc(
                    Check if the tabulated model is defined.

                    Returns:
                        bool: True if the tabulated model is defined, False otherwise.

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
