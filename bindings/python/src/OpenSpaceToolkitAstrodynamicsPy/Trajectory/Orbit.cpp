////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Pass.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model.cpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit (        pybind11::module&         aModule                                     )
{

    using namespace pybind11 ;

    using ostk::core::types::Shared ;
    using ostk::core::types::Integer ;
    using ostk::core::ctnr::Array ;

    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::trajectory::State ;
    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::orbit::models::Kepler ;
    using ostk::astro::trajectory::orbit::models::SGP4 ;
    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        class_<Orbit, ostk::astro::Trajectory> orbit_class(aModule, "Orbit") ;

        orbit_class

            .def
            (
                init<const ostk::astro::trajectory::orbit::Model&, const Shared<const Celestial>&>(),
                arg("model"),
                arg("celestial_object")
            )
            .def
            (
                init<const Array<State>&, const Integer&, const Shared<const Celestial>&>(),
                arg("states"),
                arg("initial_revolution_number"),
                arg("celestial_object")
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<Orbit>))
            .def("__repr__", &(shiftToString<Orbit>))

            .def("is_defined", &Orbit::isDefined)

            // .def("access_model", &Orbit::accessModel, return_value_policy<reference_existing_object>())
            // .def("access_kepler_model", +[] (const Orbit& anOrbit) -> const Kepler& { return anOrbit.accessModel().as<Kepler>() ; }, return_value_policy<reference_existing_object>()) // [TBR]
            // .def("access_sgp4_model", +[] (const Orbit& anOrbit) -> const SGP4& { return anOrbit.accessModel().as<SGP4>() ; }, return_value_policy<reference_existing_object>()) // [TBR]
            // .def("access_propagated_model", +[] (const Orbit& anOrbit) -> const Propagated& { return anOrbit.accessModel().as<Propagated>() ; }, return_value_policy<reference_existing_object>()) // [TBR]

            .def("access_model", &Orbit::accessModel, return_value_policy::reference) // [TBR]
            .def("access_kepler_model", +[] (const Orbit& anOrbit) -> const Kepler& { return anOrbit.accessModel().as<Kepler>() ; }, return_value_policy::reference) // [TBR]
            .def("access_sgp4_model", +[] (const Orbit& anOrbit) -> const SGP4& { return anOrbit.accessModel().as<SGP4>() ; }, return_value_policy::reference) // [TBR]
            .def("access_propagated_model", +[] (const Orbit& anOrbit) -> const Propagated& { return anOrbit.accessModel().as<Propagated>() ; }, return_value_policy::reference) // [TBR]

            .def("get_revolution_number_at", &Orbit::getRevolutionNumberAt, arg("instant"))
            .def("get_pass_at", &Orbit::getPassAt, arg("instant"))
            .def("get_pass_with_revolution_number", &Orbit::getPassWithRevolutionNumber, arg("revolution_number"))
            .def("get_orbital_frame", &Orbit::getOrbitalFrame, arg("frame_type"))

            .def_static
            (
                "undefined",
                &Orbit::Undefined
            )
            .def_static
            (
                "circular",
                &Orbit::Circular,
                arg("epoch"),
                arg("altitude"),
                arg("inclination"),
                arg("celestial_object")
            )
            .def_static
            (
                "equatorial",
                &Orbit::Equatorial,
                arg("epoch"),
                arg("apoapsis_altitude"),
                arg("periapsis_altitude"),
                arg("celestial_object")
            )
            .def_static
            (
                "circular_equatorial",
                &Orbit::CircularEquatorial,
                arg("epoch"),
                arg("altitude"),
                arg("celestial_object")
            )
            .def_static
            (
                "sun_synchronous",
                &Orbit::SunSynchronous,
                arg("epoch"),
                arg("altitude"),
                arg("local_time_at_descending_node"),
                arg("celestial_object")
            )

        ;

        enum_<Orbit::FrameType>(orbit_class, "FrameType")

            .value("Undefined", Orbit::FrameType::Undefined)
            .value("NED", Orbit::FrameType::NED)
            .value("LVLH", Orbit::FrameType::LVLH)
            .value("LVLHGD", Orbit::FrameType::LVLHGD)
            .value("VVLH", Orbit::FrameType::VVLH)
            .value("QSW", Orbit::FrameType::QSW)
            .value("TNW", Orbit::FrameType::TNW)
            .value("VNC", Orbit::FrameType::VNC)

        ;

    }

    // Create "orbit" python submodule
    auto orbit = aModule.def_submodule("orbit") ;

    // Add __path__ attribute for "orbit" submodule
    orbit.attr("__path__") = "ostk.astrodynamics.trajectory.orbit" ;

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model(orbit) ;
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models(orbit) ;
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Pass(orbit) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
