/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/Messages/CCSDS/CDM.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_Messages_CCSDS_CDM(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::math::obj::MatrixXd;

    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Length;
    using ostk::physics::units::Mass;

    using ostk::astro::conjunction::messages::ccsds::CDM;
    using ostk::astro::trajectory::State;

    class_<CDM> cdm(
        aModule,
        "CDM",
        R"doc(
            Conjunction Data Message.

            Ref: https://public.ccsds.org/Pubs/508x0b1e2s.pdf

            Group:
                ccsds
        )doc"
    );

    enum_<CDM::ObjectType>(
        cdm,
        "ObjectType",
        R"doc(
            Object type.

        )doc"
    )

        .value("Payload", CDM::ObjectType::Payload, "Payload")
        .value("RocketBody", CDM::ObjectType::RocketBody, "Rocket Body")
        .value("Debris", CDM::ObjectType::Debris, "Debris")
        .value("Unknown", CDM::ObjectType::Unknown, "Unknown")
        .value("Other", CDM::ObjectType::Other, "Other")

        ;

    cdm

        .def(
            init<const CDM::Header&, const CDM::RelativeMetadata, const Array<CDM::Metadata>, const Array<CDM::Data> >(
            ),
            R"doc(
                Constructor.

                Args:
                    header (CDM::Header): The CDM header.
                    relative_metadata (CDM::RelativeMetadata): The relative metadata.
                    objects_metadata_array (Array<CDM::Metadata>): The objects metadata array.
                    objects_data_array (Array<CDM::Data>): The objects data array.

            )doc",
            arg("header"),
            arg("relative_metadata"),
            arg("objects_metadata_array"),
            arg("objects_data_array")
        )

        .def(
            "is_defined",
            &CDM::isDefined,
            R"doc(
                Check if the CDM is defined.

                Returns:
                    bool: True if the CDM is defined, False otherwise.

            )doc"
        )

        .def(
            "get_header",
            &CDM::getHeader,
            R"doc(
                Get the CDM header.

            Returns:
                CDM::Header: The CDM header.

            )doc"
        )
        .def(
            "get_relative_metadata",
            &CDM::getRelativeMetadata,
            R"doc(
                Get the relative metadata.

            Returns:
                CDM::RelativeMetadata: The relative metadata.

            )doc"
        )
        .def(
            "get_metadata_array",
            &CDM::getMetadataArray,
            R"doc(
                Get the objects metadata array.

            Returns:
                Array<CDM::Metadata>: The objects metadata array.

            )doc"
        )
        .def(
            "get_data_array",
            &CDM::getDataArray,
            R"doc(
                Get the objects data array.

            Returns:
                Array<CDM::Data>: The objects data array.

            )doc"
        )
        .def(
            "get_object_metadata_at",
            &CDM::getObjectMetadataAt,
            R"doc(
                Get the object metadata at the specified index.

                Args:
                    index (int): The index of the object metadata.

                Returns:
                    CDM::Metadata: The object metadata.

            )doc",
            arg("index")
        )
        .def(
            "get_object_data_at",
            &CDM::getObjectDataAt,
            R"doc(
                Get the object data at the specified index.

                Args:
                    index (int): The index of the object data.

                Returns:
                    CDM::Data: The object data.

            )doc",
            arg("index")
        )
        .def(
            "get_ccsds_cdm_version",
            &CDM::getCCSDSCDMVersion,
            R"doc(
                Get the CCSDS CDM version.

                Returns:
                    std::string: The CCSDS CDM version.

            )doc"
        )
        .def(
            "get_creation_instant",
            &CDM::getCreationDate,
            R"doc(
                Get the creation instant.

                Returns:
                    Instant: The creation instant.

            )doc"
        )
        .def(
            "get_originator",
            &CDM::getOriginator,
            R"doc(
                Get the originator.

                Returns:
                    std::string: The originator.

            )doc"
        )
        .def(
            "get_message_for",
            &CDM::getMessageFor,
            R"doc(
                Get the message for.

                Returns:
                    std::string: The message for.

            )doc"
        )
        .def(
            "get_message_id",
            &CDM::getMessageId,
            R"doc(
                Get the message ID.

                Returns:
                    std::string: The message ID.

            )doc"
        )
        .def(
            "get_time_of_closest_approach",
            &CDM::getTCA,
            R"doc(
                Get the time of closest approach.

                Returns:
                    Instant: The time of closest approach.

            )doc"
        )
        .def(
            "get_miss_distance",
            &CDM::getMissDistance,
            R"doc(
                Get the miss distance.

                Returns:
                    Distance: The miss distance.

            )doc"
        )
        .def(
            "get_relative_position",
            &CDM::getRelativePosition,
            R"doc(
                Get the relative position.

                Returns:
                    Position: The relative position.

            )doc"
        )
        .def(
            "get_relative_velocity",
            &CDM::getRelativeVelocity,
            R"doc(
                Get the relative velocity.

                Returns:
                    Velocity: The relative velocity.

            )doc"
        )
        .def(
            "get_collision_probability",
            &CDM::getCollisionProbability,
            R"doc(
                Get the collision probability.

                Returns:
                    Probability: The collision probability.

            )doc"
        )
        .def(
            "get_collision_probability_method",
            &CDM::getCollisionProbabilityMethod,
            R"doc(
                Get the collision probability method.

                Returns:
                    std::string: The collision probability method.

            )doc"
        )

        .def_static(
            "undefined",
            &CDM::Undefined,
            R"doc(
                Get an undefined CDM.

                Returns:
                    CDM: An undefined CDM.

                Group:
                    Static members
            )doc"
        )
        .def_static(
            "dictionary",
            &CDM::Dictionary,
            R"doc(
                Get the CDM dictionary.

                Returns:
                    Dictionary: The CDM dictionary.

                Group:
                    Static members

            )doc",
            arg("dictionary")
        )
        .def_static(
            "parse",
            &CDM::Parse,
            R"doc(
                Parse a CDM from a string.

                Args:
                    string (str): The string to parse.

                Returns:
                    CDM: The parsed CDM.

                Group:
                    Static members

            )doc",
            arg("string")
        )
        .def_static(
            "load",
            &CDM::Load,
            R"doc(
            Load a CDM from a file.

            Args:
                file (str): The file to load.

            Returns:
                CDM: The loaded CDM.

                Group:
                    Static members

        )doc",
            arg("file")
        )
        .def_static(
            "object_type_from_string",
            &CDM::ObjectTypeFromString,
            R"doc(
                Get the object type from a string.

                Args:
                    string (str): The string to get the object type from.

                Returns:
                    CDM::ObjectType: The object type.

                Group:
                    Static members

            )doc",
            arg("string")
        )

        ;

    ;

    class_<CDM::Header>(
        cdm,
        "Header",
        R"doc(
            Conjunction Data Message header.

            Ref: https://public.ccsds.org/Pubs/508x0b1e2s.pdf

            Group:
                ccsds
        )doc"
    )

        .def(
            init<const String&, const String&, const Instant&, const String&, const String&, const String&>(),
            R"doc(
                Constructor.

                Args:
                    ccsds_cdm_version (str): The CCSDS CDM version.
                    comment (str): The comment.
                    creation_date (Instant): The creation date.
                    originator (str): The originator.
                    message_for (str): The message for.
                    message_id (str): The message ID.

            )doc",
            arg("ccsds_cdm_version"),
            arg("comment") = String::Empty(),
            arg("creation_date"),
            arg("originator"),
            arg("message_for") = String::Empty(),
            arg("message_id")
        )

        .def_readonly(
            "ccsds_cdm_version",
            &CDM::Header::ccsdsCdmVersion,
            R"doc(
                The CCSDS CDM version.
            )doc"
        )
        .def_readonly(
            "comment",
            &CDM::Header::comment,
            R"doc(
                The comment.
            )doc"
        )
        .def_readonly(
            "creation_date",
            &CDM::Header::creationDate,
            R"doc(
                The creation date.
            )doc"
        )
        .def_readonly(
            "originator",
            &CDM::Header::originator,
            R"doc(
                The originator.
            )doc"
        )
        .def_readonly(
            "message_for",
            &CDM::Header::messageFor,
            R"doc(
                The message for.
            )doc"
        )
        .def_readonly(
            "message_id",
            &CDM::Header::messageId,
            R"doc(
                The message ID.
            )doc"
        )

        ;

    class_<CDM::RelativeMetadata>(
        cdm,
        "RelativeMetadata",
        R"doc(
            Relative metadata.

            Ref: https://public.ccsds.org/Pubs/508x0b1e2s.pdf

            Group:
                ccsds
        )doc"
    )

        .def(
            init<
                const String&,
                const Instant&,
                const Length&,
                const Position&,
                const Velocity&,
                const Instant&,
                const Instant&,
                const String&,
                const String&,
                const Real&,
                const Real&,
                const Real&,
                const Instant&,
                const Instant&,
                const Real&,
                const String&>(),
            R"doc(
                Constructor.

                Args:
                    comment (str): The comment.
                    time_of_closest_approach (Instant): The time of closest approach.
                    miss_distance (Distance): The miss distance.
                    relative_position (Position): The relative position.
                    relative_velocity (Velocity): The relative velocity.
                    start_screen_period (Instant): The start screen period.
                    end_screen_period (Instant): The end screen period.
                    screen_volume_frame (str): The screen volume frame.
                    screen_volume_shape (str): The screen volume shape.
                    screen_volume_x (float): The screen volume x.
                    screen_volume_y (float): The screen volume y.
                    screen_volume_z (float): The screen volume z.
                    screen_entry_time (Instant): The screen entry time.
                    screen_exit_time (Instant): The screen exit time.
                    collision_probability (Probability): The collision probability.
                    collision_probability_method (str): The collision probability method.

            )doc",
            arg("comment") = String::Empty(),
            arg("time_of_closest_approach"),
            arg("miss_distance"),
            arg("relative_position") = Position::Undefined(),
            arg("relative_velocity") = Velocity::Undefined(),
            arg("start_screen_period"),
            arg("end_screen_period"),
            arg("screen_volume_frame") = String::Empty(),
            arg("screen_volume_shape") = String::Empty(),
            arg("screen_volume_x") = Real::Undefined(),
            arg("screen_volume_y") = Real::Undefined(),
            arg("screen_volume_z") = Real::Undefined(),
            arg("screen_entry_time"),
            arg("screen_exit_time"),
            arg("collision_probability"),
            arg("collision_probability_method")
        )

        .def_readonly(
            "comment",
            &CDM::RelativeMetadata::comment,
            R"doc(
                The comment.
            )doc"
        )
        .def_readonly(
            "time_of_closest_approach",
            &CDM::RelativeMetadata::TCA,
            R"doc(
                The time of closest approach.
            )doc"
        )
        .def_readonly(
            "miss_distance",
            &CDM::RelativeMetadata::missDistance,
            R"doc(
                The miss distance.
            )doc"
        )
        .def_readonly(
            "relative_position",
            &CDM::RelativeMetadata::relativePosition,
            R"doc(
                The relative position.
            )doc"
        )
        .def_readonly(
            "relative_velocity",
            &CDM::RelativeMetadata::relativeVelocity,
            R"doc(
                The relative velocity.
            )doc"
        )
        .def_readonly(
            "start_screen_period",
            &CDM::RelativeMetadata::startScreenPeriod,
            R"doc(
                The start screen period.
            )doc"
        )
        .def_readonly(
            "end_screen_period",
            &CDM::RelativeMetadata::endScreenPeriod,
            R"doc(
                The end screen period.
            )doc"
        )
        .def_readonly(
            "screen_volume_frame",
            &CDM::RelativeMetadata::screenVolumeFrame,
            R"doc(
                The screen volume frame.
            )doc"
        )
        .def_readonly(
            "screen_volume_shape",
            &CDM::RelativeMetadata::screenVolumeShape,
            R"doc(
                The screen volume shape.
            )doc"
        )
        .def_readonly(
            "screen_volume_x",
            &CDM::RelativeMetadata::screenVolumeX,
            R"doc(
                The screen volume x.
            )doc"
        )
        .def_readonly(
            "screen_volume_y",
            &CDM::RelativeMetadata::screenVolumeY,
            R"doc(
                The screen volume y.
            )doc"
        )
        .def_readonly(
            "screen_volume_z",
            &CDM::RelativeMetadata::screenVolumeZ,
            R"doc(
                The screen volume z.
            )doc"
        )
        .def_readonly(
            "screen_entry_time",
            &CDM::RelativeMetadata::screenEntryTime,
            R"doc(
                The screen entry time.
            )doc"
        )
        .def_readonly(
            "screen_exit_time",
            &CDM::RelativeMetadata::screenExitTime,
            R"doc(
                The screen exit time.
            )doc"
        )
        .def_readonly(
            "collision_probability",
            &CDM::RelativeMetadata::collisionProbability,
            R"doc(
                The collision probability.
            )doc"
        )
        .def_readonly(
            "collision_probability_method",
            &CDM::RelativeMetadata::collisionProbabilityMethod,
            R"doc(
                The collision probability method.
            )doc"
        )

        ;

    class_<CDM::Metadata>(
        cdm,
        "Metadata",
        R"doc(
            Conjunction Data Message metadata.

            Ref: https://public.ccsds.org/Pubs/508x0b1e2s.pdf

            Group:
                ccsds
        )doc"
    )

        .def(
            init<
                const String&,
                const String&,
                const Integer&,
                const String&,
                const String&,
                const String&,
                const CDM::ObjectType&,
                const String&,
                const String&,
                const String&,
                const String&,
                const String&,
                const String&,
                const String&,
                const String&,
                const String&,
                const String&,
                const String&,
                const String&,
                const bool&,
                const bool&,
                const bool&>(),
            R"doc(
                Constructor.

                Args:
                    comment (str): The comment.
                    object (str): The object.
                    object_designator (int): The object designator.
                    catalog_name (str): The catalog name.
                    object_name (str): The object name.
                    international_designator (str): The international designator.
                    object_type (ObjectType): The object type.
                    operator_contact_position (str): The operator contact position.
                    operator_organization (str): The operator organization.
                    operator_phone (str): The operator phone.
                    operator_email (str): The operator email.
                    ephemeris_name (str): The ephemeris name.
                    covariance_method (str): The covariance method.
                    maneuverable (str): The maneuverable.
                    orbit_center (str): The orbit center.
                    reference_frame (str): The reference frame.
                    gravity_model (str): The gravity model.
                    atmospheric_model (str): The atmospheric model.
                    n_body_perturbations (str): The n-body perturbations.
                    solar_radiation_pressure (bool): The solar radiation pressure.
                    earth_tides (bool): The earth tides.
                    in_track_thrust (bool): The in-track thrust.

            )doc",
            arg("comment") = String::Empty(),
            arg("object"),
            arg("object_designator"),
            arg("catalog_name") = String::Empty(),
            arg("object_name"),
            arg("international_designator"),
            arg("object_type"),
            arg("operator_contact_position") = String::Empty(),
            arg("operator_organization") = String::Empty(),
            arg("operator_phone") = String::Empty(),
            arg("operator_email") = String::Empty(),
            arg("ephemeris_name"),
            arg("covariance_method"),
            arg("maneuverable"),
            arg("orbit_center") = String::Empty(),
            arg("reference_frame"),
            arg("gravity_model") = String::Empty(),
            arg("atmospheric_model") = String::Empty(),
            arg("n_body_perturbations") = String::Empty(),
            arg("solar_radiation_pressure") = false,
            arg("earth_tides") = false,
            arg("in_track_thrust") = false
        )

        .def_readonly(
            "comment",
            &CDM::Metadata::comment,
            R"doc(
                The comment.
            )doc"
        )
        .def_readonly(
            "object",
            &CDM::Metadata::object,
            R"doc(
                The object.
            )doc"
        )
        .def_readonly(
            "object_designator",
            &CDM::Metadata::objectDesignator,
            R"doc(
                The object designator.
            )doc"
        )
        .def_readonly(
            "catalog_name",
            &CDM::Metadata::catalogName,
            R"doc(
                The catalog name.
            )doc"
        )
        .def_readonly(
            "object_name",
            &CDM::Metadata::objectName,
            R"doc(
                The object name.
            )doc"
        )
        .def_readonly(
            "international_designator",
            &CDM::Metadata::internationalDesignator,
            R"doc(
                The international designator.
            )doc"
        )
        .def_readonly(
            "object_type",
            &CDM::Metadata::objectType,
            R"doc(
                The object type.
            )doc"
        )
        .def_readonly(
            "operator_contact_position",
            &CDM::Metadata::operatorContactPosition,
            R"doc(
                The operator contact position.
            )doc"
        )
        .def_readonly(
            "operator_organization",
            &CDM::Metadata::operatorOrgnization,
            R"doc(
                The operator organization.
            )doc"
        )
        .def_readonly(
            "operator_phone",
            &CDM::Metadata::operatorPhone,
            R"doc(
                The operator phone.
            )doc"
        )
        .def_readonly(
            "operator_email",
            &CDM::Metadata::operatorEmail,
            R"doc(
                The operator email.
            )doc"
        )
        .def_readonly(
            "ephemeris_name",
            &CDM::Metadata::ephemerisName,
            R"doc(
                The ephemeris name.
            )doc"
        )
        .def_readonly(
            "covariance_method",
            &CDM::Metadata::covarianceMethod,
            R"doc(
                The covariance method.
            )doc"
        )
        .def_readonly(
            "maneuverable",
            &CDM::Metadata::maneuverable,
            R"doc(
                The maneuverable.
            )doc"
        )
        .def_readonly(
            "orbit_center",
            &CDM::Metadata::orbitCenter,
            R"doc(
                The orbit center.
            )doc"
        )
        .def_readonly(
            "reference_frame",
            &CDM::Metadata::refFrame,
            R"doc(
                The reference frame.
            )doc"
        )
        .def_readonly(
            "gravity_model",
            &CDM::Metadata::gravityModel,
            R"doc(
                The gravity model.
            )doc"
        )
        .def_readonly(
            "atmospheric_model",
            &CDM::Metadata::atmosphericModel,
            R"doc(
                The atmospheric model.
            )doc"
        )
        .def_readonly(
            "n_body_perturbations",
            &CDM::Metadata::nBodyPerturbations,
            R"doc(
                The n-body perturbations.
            )doc"
        )
        .def_readonly(
            "solar_radiation_pressure",
            &CDM::Metadata::solarRadiationPressure,
            R"doc(
                The solar radiation pressure.
            )doc"
        )
        .def_readonly(
            "earth_tides",
            &CDM::Metadata::earthTides,
            R"doc(
                The earth tides.
            )doc"
        )
        .def_readonly(
            "in_track_thrust",
            &CDM::Metadata::inTrackThrust,
            R"doc(
                The in-track thrust.
            )doc"
        )

        ;

    class_<CDM::Data>(
        cdm,
        "Data",
        R"doc(
            Conjunction Data Message data.

            Ref: https://public.ccsds.org/Pubs/508x0b1e2s.pdf

            Group:
                ccsds
        )doc"
    )

        .def(
            init<
                const Instant&,
                const Instant&,
                const Duration&,
                const Duration&,
                const Integer&,
                const Integer&,
                const Integer&,
                const Integer&,
                const Real&,
                const Real&,
                const Real&,
                const Real&,
                const Real&,
                const Mass&,
                const Real&,
                const Real&,
                const Real&,
                const Real&,
                const State&,
                const MatrixXd&>(),
            R"doc(
            Constructor.

            Args:
                time_last_observation_start (Instant): The time of the last observation start.
                time_last_observation_end (Instant): The time of the last observation end.
                recommended_od_span (Duration): The recommended OD span.
                actual_od_span (Duration): The actual OD span.
                observations_available (int): The number of observations available.
                observations_used (int): The number of observations used.
                tracks_available (int): The number of tracks available.
                tracks_used (int): The number of tracks used.
                residuals_accepted (float): The residuals accepted.
                weighted_rms (float): The weighted RMS.
                area_pc (float): The area PC.
                area_drag (float): The area drag.
                area_srp (float): The area SRP.
                mass (Mass): The mass.
                cd_area_over_mass (float): The CD area over mass.
                cr_area_over_mass (float): The CR area over mass.
                thrust_acceleration (float): The thrust acceleration.
                sedr (float): The SEDR.
                state (State): The state.
                covariance_matrix (MatrixXd): The covariance matrix.

        )doc",
            arg("time_last_observation_start"),
            arg("time_last_observation_end"),
            arg("recommended_od_span"),
            arg("actual_od_span"),
            arg("observations_available"),
            arg("observations_used"),
            arg("tracks_available"),
            arg("tracks_used"),
            arg("residuals_accepted"),
            arg("weighted_rms"),
            arg("area_pc"),
            arg("area_drag"),
            arg("area_srp"),
            arg("mass"),
            arg("cd_area_over_mass"),
            arg("cr_area_over_mass"),
            arg("thrust_acceleration"),
            arg("sedr"),
            arg("state") = State::Undefined(),
            arg("covariance_matrix") = MatrixXd::Zero(9, 9)
        )

        .def_readonly(
            "time_last_observation_start",
            &CDM::Data::timeLastObStart,
            R"doc(
                The time of the last observation start.
            )doc"
        )
        .def_readonly(
            "time_last_observation_end",
            &CDM::Data::timeLastObEnd,
            R"doc(
                The time of the last observation end.
            )doc"
        )
        .def_readonly(
            "recommended_od_span",
            &CDM::Data::recommendedODSpan,
            R"doc(
                The recommended OD span.
            )doc"
        )
        .def_readonly(
            "actual_od_span",
            &CDM::Data::actualODSpan,
            R"doc(
                The actual OD span.
            )doc"
        )
        .def_readonly(
            "observations_available",
            &CDM::Data::obsAvailable,
            R"doc(
                The number of observations available.
            )doc"
        )
        .def_readonly(
            "observations_used",
            &CDM::Data::obsUsed,
            R"doc(
                The number of observations used.
            )doc"
        )
        .def_readonly(
            "tracks_available",
            &CDM::Data::tracksAvailable,
            R"doc(
                The number of tracks available.
            )doc"
        )
        .def_readonly(
            "tracks_used",
            &CDM::Data::tracksUsed,
            R"doc(
                The number of tracks used.
            )doc"
        )
        .def_readonly(
            "residuals_accepted",
            &CDM::Data::residualsAccepted,
            R"doc(
                The residuals accepted.
            )doc"
        )
        .def_readonly(
            "weighted_rms",
            &CDM::Data::weightedRMS,
            R"doc(
                The weighted RMS.
            )doc"
        )
        .def_readonly(
            "area_pc",
            &CDM::Data::areaPC,
            R"doc(
                The area PC.
            )doc"
        )
        .def_readonly(
            "area_drag",
            &CDM::Data::areaDrag,
            R"doc(
                The area drag.
            )doc"
        )
        .def_readonly(
            "area_srp",
            &CDM::Data::areaSRP,
            R"doc(
                The area SRP.
            )doc"
        )
        .def_readonly(
            "mass",
            &CDM::Data::mass,
            R"doc(
                The mass.
            )doc"
        )
        .def_readonly(
            "cd_area_over_mass",
            &CDM::Data::cdAreaOverMass,
            R"doc(
                The CD area over mass.
            )doc"
        )
        .def_readonly(
            "cr_area_over_mass",
            &CDM::Data::crAreaOverMass,
            R"doc(
                The CR area over mass.
            )doc"
        )
        .def_readonly(
            "thrust_acceleration",
            &CDM::Data::thrustAcceleration,
            R"doc(
                The thrust acceleration.
            )doc"
        )
        .def_readonly(
            "sedr",
            &CDM::Data::SEDR,
            R"doc(
                The SEDR.
            )doc"
        )
        .def_readonly(
            "state",
            &CDM::Data::state,
            R"doc(
                The state.
            )doc"
        )
        .def_readonly(
            "covariance_matrix",
            &CDM::Data::covarianceMatrix,
            R"doc(
                The covariance matrix.
            )doc"
        )

        ;
}
