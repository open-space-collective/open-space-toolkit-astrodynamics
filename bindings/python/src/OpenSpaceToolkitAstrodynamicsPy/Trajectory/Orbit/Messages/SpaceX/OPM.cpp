/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Messages/SpaceX/OPM.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Messages_SpaceX_OPM(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::orbit::messages::spacex::OPM;

    class_<OPM> opm(
        aModule,
        "OPM",
        R"doc(
            The SpaceX OPM message.

            See Also:
                `SpaceX OPM <https://public.ccsds.org/Pubs/502x0b3e1.pdf>`_.

            Group:
                spacex
        )doc"
    );

    opm

        .def(
            init<const OPM::Header&, const Array<OPM::Deployment>&>(),
            R"doc(
                Constructor.

                Args:
                    header (Header): The header of the OPM message.
                    deployments (list[Deployment]): The deployments of the OPM message.

            )doc",
            arg("header"),
            arg("deployments")
        )

        .def(
            "is_defined",
            &OPM::isDefined,
            R"doc(
                Check if the OPM message is defined.

                Returns:
                    is_defined (bool): True if the OPM message is defined, False otherwise.

            )doc"
        )

        .def(
            "get_header",
            &OPM::getHeader,
            R"doc(
                Get the header of the OPM message.

                Returns:
                    header (Header): The header of the OPM message.

            )doc"
        )
        .def(
            "get_deployments",
            &OPM::getDeployments,
            R"doc(
                Get the deployments of the OPM message.

                Returns:
                    deployments (list[Deployment]): The deployments of the OPM message.

            )doc"
        )
        .def(
            "get_deployment_at",
            &OPM::getDeploymentAt,
            arg("index"),
            R"doc(
                Get the deployment at a given index.

                Args:
                    index (int): The index of the deployment.

                Returns:
                    deployment (Deployment): The deployment at the given index.

            )doc"
        )
        .def(
            "get_deployment_with_name",
            &OPM::getDeploymentWithName,
            R"doc(
                Get the deployment with a given name.

                Args:
                    name (str): The name of the deployment.

                Returns:
                    deployment (Deployment): The deployment with the given name.

            )doc",
            arg("name")
        )

        .def_static(
            "undefined",
            &OPM::Undefined,
            R"doc(
                Return an undefined OPM message.

                Returns:
                    opm (OPM): An undefined OPM message.

                Group:
                    Static methods

            )doc"
        )
        .def_static(
            "dictionary",
            &OPM::Dictionary,
            R"doc(
                Build an OPM message from a dictionary.

                Args:
                    dictionary (dict): The dictionary containing the OPM message information.

                Returns:
                    opm (OPM): The OPM message.

                Group:
                    Static methods

            )doc",
            arg("dictionary")
        )
        .def_static(
            "parse",
            &OPM::Parse,
            R"doc(
                Parse an OPM message from a string.

                Args:
                    string (str): The string containing the OPM message.

                Returns:
                    opm (OPM): The OPM message.

                Group:
                    Static methods

            )doc",
            arg("string")
        )
        .def_static(
            "load",
            &OPM::Load,
            R"doc(
                Load an OPM message from a file.

                Args:
                    file (str): The path to the file containing the OPM message.

                Returns:
                    opm (OPM): The OPM message.

                Group:
                    Static methods

            )doc",
            arg("file")
        )

        ;

    class_<OPM::Header>(
        opm,
        "Header",
        R"doc(
            The header of the SpaceX OPM message.

            Group:
                spacex
        )doc"
    )

        .def(
            init<const Instant&, const Instant&>(),
            R"doc(
                Constructor.

                Args:
                    generation_date (Instant): The date at which the OPM message was generated.
                    launch_date (Instant): The date at which the spacecraft was launched.

            )doc",
            arg("generation_date"),
            arg("launch_date")
        )

        .def_readonly(
            "generation_date",
            &OPM::Header::generationDate,
            R"doc(
                Get the date at which the OPM message was generated.

                Returns:
                    instant (Instant): The date at which the OPM message was generated.

            )doc"
        )
        .def_readonly(
            "launch_date",
            &OPM::Header::launchDate,
            R"doc(
                Get the date at which the spacecraft was launched.

                Returns:
                    instant (Instant): The date at which the spacecraft was launched.

            )doc"
        )

        ;

    class_<OPM::Deployment>(
        opm,
        "Deployment",
        R"doc(
            The deployment of the SpaceX OPM message.

            Group:
                spacex
        )doc"
    )

        .def(
            init<
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
                const Real&>(),
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
            arg("ballistic_coefficient"),
            R"doc(
                Constructor.

                Args:
                    name (str): The name of the deployment.
                    sequence_number (int): The sequence number of the deployment.
                    mission_time (Duration): The mission time of the deployment.
                    date (Instant): The date of the deployment.
                    position (Position): The position of the deployment.
                    velocity (Velocity): The velocity of the deployment.
                    mean_perigee_altitude (Length): The mean perigee altitude of the deployment.
                    mean_apogee_altitude (Length): The mean apogee altitude of the deployment.
                    mean_inclination (Angle): The mean inclination of the deployment.
                    mean_argument_of_perigee (Angle): The mean argument of perigee of the deployment.
                    mean_longitude_ascending_node (Angle): The mean longitude of the ascending node of the deployment.
                    mean_mean_anomaly (Angle): The mean mean anomaly of the deployment.
                    ballistic_coefficient (float): The ballistic coefficient of the deployment.

            )doc"
        )

        .def_readonly(
            "name",
            &OPM::Deployment::name,
            R"doc(
                Get the name of the deployment.

                :type: str

        )doc"
        )
        .def_readonly(
            "sequence_number",
            &OPM::Deployment::sequenceNumber,
            R"doc(
                Get the sequence number of the deployment.

                :type: int

        )doc"
        )
        .def_readonly(
            "mission_time",
            &OPM::Deployment::missionTime,
            R"doc(
                Get the mission time of the deployment.

                :type: Duration

        )doc"
        )
        .def_readonly(
            "date",
            &OPM::Deployment::date,
            R"doc(
                Get the date of the deployment.

                :type: Instant

        )doc"
        )
        .def_readonly(
            "position",
            &OPM::Deployment::position,
            R"doc(
                Get the position of the deployment.

                :type: Position

        )doc"
        )
        .def_readonly(
            "velocity",
            &OPM::Deployment::velocity,
            R"doc(
                Get the velocity of the deployment.

                :type: Velocity

        )doc"
        )
        .def_readonly(
            "mean_perigee_altitude",
            &OPM::Deployment::meanPerigeeAltitude,
            R"doc(
                Get the mean perigee altitude of the deployment.

                :type: Length

        )doc"
        )
        .def_readonly(
            "mean_apogee_altitude",
            &OPM::Deployment::meanApogeeAltitude,
            R"doc(
                Get the mean apogee altitude of the deployment.

                :type: Length

        )doc"
        )
        .def_readonly(
            "mean_inclination",
            &OPM::Deployment::meanInclination,
            R"doc(
                Get the mean inclination of the deployment.

                :type: Angle

        )doc"
        )
        .def_readonly(
            "mean_argument_of_perigee",
            &OPM::Deployment::meanArgumentOfPerigee,
            R"doc(
                Get the mean argument of perigee of the deployment.

                :type: Angle

        )doc"
        )
        .def_readonly(
            "mean_longitude_ascending_node",
            &OPM::Deployment::meanLongitudeAscendingNode,
            R"doc(
                Get the mean longitude of the ascending node of the deployment.

                :type: Angle

        )doc"
        )
        .def_readonly(
            "mean_mean_anomaly",
            &OPM::Deployment::meanMeanAnomaly,
            R"doc(
                Get the mean mean anomaly of the deployment.

                :type: Angle

        )doc"
        )
        .def_readonly(
            "ballistic_coefficient",
            &OPM::Deployment::ballisticCoefficient,
            R"doc(
                Get the ballistic coefficient of the deployment.

                :type: float

        )doc"
        )

        .def(
            "to_state",
            &OPM::Deployment::toState,
            R"doc(
                Convert the deployment to a state.

                Returns:
                    state (State): The state of the deployment.

            )doc"
        )

        ;
}
