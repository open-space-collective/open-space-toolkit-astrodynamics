////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Messages/SpaceX/OPM.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Messages/SpaceX/OPM.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Messages_SpaceX_OPM ( pybind11::module& aModule                                )
{

    using namespace pybind11 ;

    using ostk::core::types::Integer ;
    using ostk::core::types::Real ;
    using ostk::core::types::String ;
    using ostk::core::ctnr::Array ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::units::Length ;
    using ostk::physics::units::Angle ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;

    using ostk::astro::trajectory::orbit::messages::spacex::OPM ;

    class_<OPM> opm(aModule, "OPM") ;

    opm

        .def(init<const OPM::Header&, const Array<OPM::Deployment>&>(), arg("header"), arg("deployments"))

        .def("is_defined", &OPM::isDefined)

        .def("get_header", &OPM::getHeader)
        .def("get_deployments", &OPM::getDeployments)
        .def("get_deployment_at", &OPM::getDeploymentAt, arg("index"))
        .def("get_deployment_with_name", &OPM::getDeploymentWithName, arg("name"))

        .def_static("undefined", &OPM::Undefined)
        .def_static("dictionary", &OPM::Dictionary, arg("dictionary"))
        .def_static("parse", &OPM::Parse, arg("string"))
        .def_static("load", &OPM::Load, arg("file"))

    ;

    class_<OPM::Header>(opm, "Header")

        .def
        (
            init<const Instant&, const Instant&>(),
            arg("generation_date"),
            arg("launch_date")
        )

        .def_readonly("generation_date", &OPM::Header::generationDate)
        .def_readonly("launch_date", &OPM::Header::launchDate)

    ;

    class_<OPM::Deployment>(opm, "Deployment")

        .def
        (
            init
            <
                const String&,
                const Integer&,
                const Duration&,
                const Instant&,
                const Position&,
                const Velocity&,
                const Length&,
                const Length&,
                const Angle&,
                const Angle&,
                const Angle&,
                const Angle&,
                const Real&
            >(),
            arg("name"),
            arg("sequence_number"),
            arg("mission_time"),
            arg("date"),
            arg("position"),
            arg("velocity"),
            arg("mean_perigee_altitude"),
            arg("mean_apogee_altitude"),
            arg("mean_inclination"),
            arg("mean_argument_of_perigee"),
            arg("mean_longitude_ascending_node"),
            arg("mean_mean_anomaly"),
            arg("ballistic_coefficient")
        )

        .def_readonly("name", &OPM::Deployment::name)
        .def_readonly("sequence_number", &OPM::Deployment::sequenceNumber)
        .def_readonly("mission_time", &OPM::Deployment::missionTime)
        .def_readonly("date", &OPM::Deployment::date)
        .def_readonly("position", &OPM::Deployment::position)
        .def_readonly("velocity", &OPM::Deployment::velocity)
        .def_readonly("mean_perigee_altitude", &OPM::Deployment::meanPerigeeAltitude)
        .def_readonly("mean_apogee_altitude", &OPM::Deployment::meanApogeeAltitude)
        .def_readonly("mean_inclination", &OPM::Deployment::meanInclination)
        .def_readonly("mean_argument_of_perigee", &OPM::Deployment::meanArgumentOfPerigee)
        .def_readonly("mean_longitude_ascending_node", &OPM::Deployment::meanLongitudeAscendingNode)
        .def_readonly("mean_mean_anomaly", &OPM::Deployment::meanMeanAnomaly)
        .def_readonly("ballistic_coefficient", &OPM::Deployment::ballisticCoefficient)

        .def("to_state", &OPM::Deployment::toState)

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
