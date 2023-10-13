/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Kepler/BrouwerLyddaneMean.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Kepler/COE.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Real;

    using ostk::physics::env::obj::Celestial;
    using ostk::physics::time::Instant;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        class_<Kepler, ostk::astro::trajectory::orbit::Model> kepler_class(aModule, "Kepler");

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

            .def("is_defined", &Kepler::isDefined)

            .def("get_classical_orbital_elements", &Kepler::getClassicalOrbitalElements)
            .def("get_epoch", &Kepler::getEpoch)
            .def("get_revolution_number_at_epoch", &Kepler::getRevolutionNumberAtEpoch)
            .def("get_gravitational_parameter", &Kepler::getGravitationalParameter)
            .def("get_equatorial_radius", &Kepler::getEquatorialRadius)
            .def("get_j2", &Kepler::getJ2)
            .def("get_j4", &Kepler::getJ4)
            .def("get_perturbation_type", &Kepler::getPerturbationType)
            .def("calculate_state_at", &Kepler::calculateStateAt, arg("instant"))
            .def("calculate_revolution_number_at", &Kepler::calculateRevolutionNumberAt, arg("instant"))

            .def_static("string_from_perturbation_type", &Kepler::StringFromPerturbationType, arg("perturbation_type"))

            ;

        enum_<Kepler::PerturbationType>(kepler_class, "PerturbationType")

            .value("No", Kepler::PerturbationType::None)
            .value("J2", Kepler::PerturbationType::J2)
            .value("J4", Kepler::PerturbationType::J4)

            ;
    }

    // Create "kepler" python submodule
    auto kepler = aModule.def_submodule("kepler");

    // Add __path__ attribute for "kepler" submodule
    kepler.attr("__path__") = "ostk.astrodynamics.trajectory.orbit.models.kepler";

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler_COE(kepler);
}
