/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Tabulated.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Message.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Pass.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Integer;
    using ostk::core::type::Shared;

    using ostk::physics::environment::object::Celestial;
    using ostk::physics::time::Duration;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::trajectory::Orbit;
    using ostk::astrodynamics::trajectory::orbit::model::Kepler;
    using ostk::astrodynamics::trajectory::orbit::model::Propagated;
    using ostk::astrodynamics::trajectory::orbit::model::SGP4;
    using ostk::astrodynamics::trajectory::orbit::model::Tabulated;
    using ostk::astrodynamics::trajectory::State;

    {
        class_<Orbit, ostk::astrodynamics::Trajectory, Shared<Orbit>> orbit_class(
            aModule,
            "Orbit",
            R"doc(
                Gravitationally curved trajectory of an object.

            )doc"
        );

        enum_<Orbit::FrameType>(
            orbit_class,
            "FrameType",
            R"doc(
                The local orbital frame type.
            )doc"
        )

            .value("Undefined", Orbit::FrameType::Undefined, "Undefined")
            .value("NED", Orbit::FrameType::NED, "North-East-Down")
            .value("LVLH", Orbit::FrameType::LVLH, "Local Vertical-Local Horizontal")
            .value("LVLHGD", Orbit::FrameType::LVLHGD, "Local Vertical-Local Horizontal GeoDetic")
            .value("LVLHGDGT", Orbit::FrameType::LVLHGDGT, "Local Vertical-Local Horizontal GeoDetic Ground Track")
            .value("VVLH", Orbit::FrameType::VVLH, "Vertical-Local Horizontal")
            .value("QSW", Orbit::FrameType::QSW, "Quasi-Satellite West")
            .value("TNW", Orbit::FrameType::TNW, "Topocentric North-West")
            .value("VNC", Orbit::FrameType::VNC, "Velocity-Normal-Co-normal")

            ;

        orbit_class

            .def(
                init<const ostk::astrodynamics::trajectory::orbit::Model&, const Shared<const Celestial>&>(),
                arg("model"),
                arg("celestial_object"),
                R"doc(
                    Constructs an `Orbit` object.

                    Args:
                        model (orbit.Model): The orbit model.
                        celestial_object (Celestial): The celestial object.

                )doc"
            )

            .def(
                init<const Array<State>&, const Integer&, const Shared<const Celestial>&>(),
                arg("states"),
                arg("initial_revolution_number"),
                arg("celestial_object"),
                R"doc(
                    Constructs an `Orbit` object.

                    Args:
                        states (Array<State>): The states.
                        initial_revolution_number (Integer): The initial revolution number.
                        celestial_object (Celestial): The celestial object.

                )doc"
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<Orbit>))
            .def("__repr__", &(shiftToString<Orbit>))

            .def(
                "is_defined",
                &Orbit::isDefined,
                R"doc(
                    Check if the `Orbit` object is defined.

                    Returns:
                        bool: True if the `Orbit` object is defined, False otherwise.

                )doc"
            )

            .def(
                "access_model",
                &Orbit::accessModel,
                return_value_policy::reference,
                R"doc(
                    Access the orbit model.

                    Returns:
                        orbit.Model: The orbit model.

                )doc"
            )
            .def(
                "access_kepler_model",
                +[](const Orbit& anOrbit) -> const Kepler&
                {
                    return anOrbit.accessModel().as<Kepler>();
                },
                return_value_policy::reference,
                R"doc(
                    Access the Kepler orbit model.

                    Returns:
                       Kepler: The Kepler orbit model.

                )doc"
            )
            .def(
                "access_sgp4_model",
                +[](const Orbit& anOrbit) -> const SGP4&
                {
                    return anOrbit.accessModel().as<SGP4>();
                },
                return_value_policy::reference,
                R"doc(
                    Access the SGP4 orbit model.

                    Returns:
                        SGP4: The SGP4 orbit model.

                )doc"
            )
            .def(
                "access_propagated_model",
                +[](const Orbit& anOrbit) -> const Propagated&
                {
                    return anOrbit.accessModel().as<Propagated>();
                },
                return_value_policy::reference,
                R"doc(
                    Access the propagated orbit model.

                    Returns:
                        Propagated: The propagated orbit model.

                )doc"
            )
            .def(
                "access_tabulated_model",
                +[](const Orbit& anOrbit) -> const Tabulated&
                {
                    return anOrbit.accessModel().as<Tabulated>();
                },
                return_value_policy::reference,
                R"doc(
                    Access the tabulated orbit model.

                    Returns:
                        Tabulated: The tabulated orbit model.

                )doc"
            )

            .def(
                "get_revolution_number_at",
                &Orbit::getRevolutionNumberAt,
                R"doc(
                    Get the revolution number at a given instant.

                    Args:
                        instant (Instant): The instant.

                    Returns:
                        int: The revolution number.

                )doc",
                arg("instant")
            )
            .def(
                "get_pass_at",
                &Orbit::getPassAt,
                R"doc(
                    Get the pass at a given instant.

                    Args:
                        instant (Instant): The instant.

                    Returns:
                        ostk::astrodynamics::trajectory::orbit::Pass: The pass.

                )doc",
                arg("instant")
            )
            .def(
                "get_pass_with_revolution_number",
                &Orbit::getPassWithRevolutionNumber,
                R"doc(
                    Get the pass with a given revolution number.

                    Args:
                        revolution_number (int): The revolution number.
                        step_duration (Duration): The initial step duration used for the pass computation algorithm.

                    Returns:
                        Pass: The pass.

                )doc",
                arg("revolution_number"),
                arg_v("step_duration", Duration::Minutes(10.0), "Duration.minutes(10.0)")
            )
            .def(
                "get_passes_within_interval",
                &Orbit::getPassesWithinInterval,
                R"doc(
                    Get the passes within a given interval.

                    Args:
                        interval (Interval): The interval.

                    Returns:
                        list[Pass]: The passes.
                )doc",
                arg("interval")
            )
            .def(
                "get_orbital_frame",
                &Orbit::getOrbitalFrame,
                R"doc(
                    Get the orbital frame.

                    Args:
                        frame_type (Orbit::FrameType): The frame type.

                    Returns:
                        Frame: The orbital frame.

                )doc",
                arg("frame_type")
            )

            .def_static(
                "undefined",
                &Orbit::Undefined,
                R"doc(
                    Get an undefined `Orbit` object.

                    Returns:
                        Orbit: The undefined `Orbit` object.
                )doc"
            )

            .def_static(
                "circular",
                &Orbit::Circular,
                arg("epoch"),
                arg("altitude"),
                arg("inclination"),
                arg("celestial_object"),
                R"doc(
                    Create a circular `Orbit` object.

                    Args:
                        epoch (Instant): The epoch.
                        altitude (Length): The altitude.
                        inclination (Angle): The inclination.
                        celestial_object (Celestial): The celestial object.

                    Returns:
                        Orbit: The circular `Orbit` object.
                )doc"
            )

            .def_static(
                "equatorial",
                &Orbit::Equatorial,
                arg("epoch"),
                arg("apoapsis_altitude"),
                arg("periapsis_altitude"),
                arg("celestial_object"),
                R"doc(
                    Create an equatorial `Orbit` object.

                    Args:
                        epoch (Instant): The epoch.
                        apoapsis_altitude (Length): The apoapsis altitude.
                        periapsis_altitude (Length): The periapsis altitude.
                        celestial_object (Celestial): The celestial object.

                    Returns:
                        Orbit: The equatorial `Orbit` object.
                )doc"
            )

            .def_static(
                "circular_equatorial",
                &Orbit::CircularEquatorial,
                arg("epoch"),
                arg("altitude"),
                arg("celestial_object"),
                R"doc(
                    Create a circular equatorial `Orbit` object.

                    Args:
                        epoch (Instant): The epoch.
                        altitude (Length): The altitude.
                        celestial_object (Celestial): The celestial object.

                    Returns:
                        Orbit: The circular equatorial `Orbit` object.
                )doc"
            )

            .def_static(
                "geo_synchronous",
                &Orbit::GeoSynchronous,
                arg("epoch"),
                arg("inclination"),
                arg("longitude"),
                arg("celestial_object"),
                R"doc(
                    Create a geosynchronous `Orbit` object.

                    Args:
                        epoch (Instant): The epoch.
                        inclination (Angle): The inclination.
                        longitude (double): The longitude.
                        celestial_object (Celestial): The celestial object.

                    Returns:
                        Orbit: The geosynchronous `Orbit` object.
                )doc"
            )

            .def_static(
                "sun_synchronous",
                &Orbit::SunSynchronous,
                arg("epoch"),
                arg("altitude"),
                arg("local_time_at_descending_node"),
                arg("celestial_object"),
                arg_v("argument_of_latitude", Angle::Zero(), "Angle.zero()"),
                R"doc(
                    Create a sun-synchronous `Orbit` object.

                    Args:
                        epoch (Instant): The epoch.
                        altitude (Length): The altitude.
                        local_time_at_descending_node (Time): The local time at descending node.
                        celestial_object (Celestial): The celestial object.
                        argument_of_latitude (Angle): The argument of latitude.

                    Returns:
                        Orbit: The sun-synchronous `Orbit` object.
                )doc"
            )

            .def_static(
                "compute_passes",
                &Orbit::ComputePasses,
                arg("states"),
                arg("initial_revolution_number"),
                R"doc(
                    Compute passes from a set of states.

                    Args:
                        states (Array<State>): The states.
                        initial_revolution_number (Integer): The initial revolution number.

                    Returns:
                        list[tuple[int, Pass]]: List of index-pass pairs
                )doc"
            )

            .def_static(
                "compute_passes_with_model",
                &Orbit::ComputePassesWithModel,
                arg("model"),
                arg("interval"),
                arg("initial_revolution_number"),
                R"doc(
                    Compute passes with the given model for the provided interval.

                    Args:
                        model (orbit.Model): The model.
                        interval (Interval): The interval within which to generate passes.
                        initial_revolution_number (int): The initial revolution number.

                    Returns:
                        list[Pass]: List of passes
                )doc"
            )

            ;
    }

    // Create "orbit" python submodule
    auto orbit = aModule.def_submodule("orbit");

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model(orbit);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Pass(orbit);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Message(orbit);
}
