/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Kepler/COE.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Real;

    using ostk::physics::environment::object::Celestial;
    using ostk::physics::time::Instant;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        class_<Kepler, ostk::astro::trajectory::orbit::Model> kepler_class(
            aModule,
            "Kepler",
            R"doc(
                A Kepler orbit model.

                Provides the interface for orbit models.

                Group:
                    orbit
            )doc"
        );

        enum_<Kepler::PerturbationType>(
            kepler_class,
            "PerturbationType",
            R"doc(
                The Perturbation Type due to Oblateness
            
            )doc"
        )

            .value("No", Kepler::PerturbationType::None, "No perturbation")

            .value("J2", Kepler::PerturbationType::J2, "J2 perturbation")

            .value("J4", Kepler::PerturbationType::J4, "J4 perturbation")

            ;

        kepler_class

            .def(
                init<
                    const COE&,
                    const Instant&,
                    const Derived&,
                    const Length&,
                    const Real&,
                    const Real&,
                    const Kepler::PerturbationType&>(),
                R"doc(
                    Constructor.

                    Args:
                        coe (COE): The classical orbital elements.
                        epoch (Instant): The epoch.
                        gravitational_parameter (Derived): The gravitational parameter.
                        equatorial_radius (Length): The equatorial radius.
                        j2 (float): The J2 coefficient.
                        j4 (float): The J4 coefficient.
                        perturbation_type (PerturbationType): The perturbation type.

                )doc",
                arg("coe"),
                arg("epoch"),
                arg("gravitational_parameter"),
                arg("equatorial_radius"),
                arg("j2"),
                arg("j4"),
                arg("perturbation_type")
            )

            .def(
                init<const COE&, const Instant&, const Celestial&, const Kepler::PerturbationType&, const bool>(),
                R"doc(
                    Constructor.

                    Args:
                        coe (COE): The classical orbital elements.
                        epoch (Instant): The epoch.
                        celestial_object (Celestial): The celestial object.
                        perturbation_type (PerturbationType): The perturbation type.
                        in_fixed_frame (bool): If True, the state is expressed in the fixed frame, otherwise it is expressed in the inertial frame. Default is False.

                )doc",
                arg("coe"),
                arg("epoch"),
                arg("celestial_object"),
                arg("perturbation_type"),
                arg("in_fixed_frame") = DEFAULT_IN_FIXED_FRAME
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<Kepler>))
            .def("__repr__", &(shiftToString<Kepler>))

            .def(
                "is_defined",
                &Kepler::isDefined,
                R"doc(
                    Check if the `Kepler` model is defined.

                    Returns:
                        bool: True if the `Kepler` model is defined, False otherwise.
                )doc"
            )

            .def(
                "get_classical_orbital_elements",
                &Kepler::getClassicalOrbitalElements,
                R"doc(
                    Get the classical orbital elements of the `Kepler` model.

                    Returns:
                        COE: The classical orbital elements.
                )doc"
            )

            .def(
                "get_epoch",
                &Kepler::getEpoch,
                R"doc(
                    Get the epoch of the `Kepler` model.

                    Returns:
                        Instant: The epoch.
                )doc"
            )

            .def(
                "get_revolution_number_at_epoch",
                &Kepler::getRevolutionNumberAtEpoch,
                R"doc(
                    Get the revolution number at the epoch of the `Kepler` model.

                    Returns:
                        int: The revolution number.
                )doc"
            )

            .def(
                "get_gravitational_parameter",
                &Kepler::getGravitationalParameter,
                R"doc(
                    Get the gravitational parameter of the `Kepler` model.

                    Returns:
                        Derived: The gravitational parameter.
                )doc"
            )

            .def(
                "get_equatorial_radius",
                &Kepler::getEquatorialRadius,
                R"doc(
                    Get the equatorial radius of the `Kepler` model.

                    Returns:
                        Length: The equatorial radius.
                )doc"
            )

            .def(
                "get_j2",
                &Kepler::getJ2,
                R"doc(
                    Get the J2 coefficient of the `Kepler` model.

                    Returns:
                        float: The J2 coefficient.
                )doc"
            )

            .def(
                "get_j4",
                &Kepler::getJ4,
                R"doc(
                    Get the J4 coefficient of the `Kepler` model.

                    Returns:
                        float: The J4 coefficient.
                )doc"
            )

            .def(
                "get_perturbation_type",
                &Kepler::getPerturbationType,
                R"doc(
                    Get the perturbation type of the `Kepler` model.

                    Returns:
                        PerturbationType: The perturbation type.
                )doc"
            )

            .def(
                "calculate_state_at",
                &Kepler::calculateStateAt,
                arg("instant"),
                R"doc(
                    Calculate the state of the `Kepler` model at a given instant.

                    Args:
                        instant (Instant): The instant.

                    Returns:
                        State: The state.
                )doc"
            )

            .def(
                "calculate_revolution_number_at",
                &Kepler::calculateRevolutionNumberAt,
                arg("instant"),
                R"doc(
                    Calculate the revolution number of the `Kepler` model at a given instant.

                    Args:
                        instant (Instant): The instant.

                    Returns:
                        int: The revolution number.
                )doc"
            )

            .def_static(
                "string_from_perturbation_type",
                &Kepler::StringFromPerturbationType,
                arg("perturbation_type"),
                R"doc(
                    Get the string representation of a `PerturbationType`.

                    Args:
                        perturbation_type (PerturbationType): The perturbation type.

                    Returns:
                        str: The string representation.
                )doc"
            )

            ;
    }

    // Create "kepler" python submodule
    auto kepler = aModule.def_submodule("kepler");

    // Add __path__ attribute for "kepler" submodule
    kepler.attr("__path__") = "ostk.astrodynamics.trajectory.orbit.models.kepler";

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler_COE(kepler);
}
