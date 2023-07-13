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

    class_<CDM> cdm(aModule, "CDM");

    cdm

        .def(
            init<const CDM::Header&, const CDM::RelativeMetadata, const Array<CDM::Metadata>, const Array<CDM::Data> >(
            ),
            arg("header"),
            arg("relative_metadata"),
            arg("objects_metadata_array"),
            arg("objects_data_array")
        )

        .def("is_defined", &CDM::isDefined)

        .def("get_header", &CDM::getHeader)
        .def("get_relative_metadata", &CDM::getRelativeMetadata)
        .def("get_metadata_array", &CDM::getMetadataArray)
        .def("get_data_array", &CDM::getDataArray)
        .def("get_object_metadata_at", &CDM::getObjectMetadataAt, arg("index"))
        .def("get_object_data_at", &CDM::getObjectDataAt, arg("index"))
        .def("get_ccsds_cdm_version", &CDM::getCCSDSCDMVersion)
        .def("get_creation_instant", &CDM::getCreationDate)
        .def("get_originator", &CDM::getOriginator)
        .def("get_message_for", &CDM::getMessageFor)
        .def("get_message_id", &CDM::getMessageId)
        .def("get_time_of_closest_approach", &CDM::getTCA)
        .def("get_miss_distance", &CDM::getMissDistance)
        .def("get_relative_position", &CDM::getRelativePosition)
        .def("get_relative_velocity", &CDM::getRelativeVelocity)
        .def("get_collision_probability", &CDM::getCollisionProbability)
        .def("get_collision_probability_method", &CDM::getCollisionProbabilityMethod)

        .def_static("undefined", &CDM::Undefined)
        .def_static("dictionary", &CDM::Dictionary, arg("dictionary"))
        .def_static("parse", &CDM::Parse, arg("string"))
        .def_static("load", &CDM::Load, arg("file"))
        .def_static("object_type_from_string", &CDM::ObjectTypeFromString, arg("string"))

        ;

    enum_<CDM::ObjectType>(cdm, "ObjectType")

        .value("Payload", CDM::ObjectType::Payload)
        .value("RocketBody", CDM::ObjectType::RocketBody)
        .value("Debris", CDM::ObjectType::Debris)
        .value("Unknown", CDM::ObjectType::Unknown)
        .value("Other", CDM::ObjectType::Other)

        ;

    class_<CDM::Header>(cdm, "Header")

        .def(
            init<const String&, const String&, const Instant&, const String&, const String&, const String&>(),
            arg("ccsds_cdm_version"),
            arg("comment") = String::Empty(),
            arg("creation_date"),
            arg("originator"),
            arg("message_for") = String::Empty(),
            arg("message_id")
        )

        .def_readonly("ccsds_cdm_version", &CDM::Header::ccsdsCdmVersion)
        .def_readonly("comment", &CDM::Header::comment)
        .def_readonly("creation_date", &CDM::Header::creationDate)
        .def_readonly("originator", &CDM::Header::originator)
        .def_readonly("message_for", &CDM::Header::messageFor)
        .def_readonly("message_id", &CDM::Header::messageId)

        ;

    class_<CDM::RelativeMetadata>(cdm, "RelativeMetadata")

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

        .def_readonly("comment", &CDM::RelativeMetadata::comment)
        .def_readonly("time_of_closest_approach", &CDM::RelativeMetadata::TCA)
        .def_readonly("miss_distance", &CDM::RelativeMetadata::missDistance)
        .def_readonly("relative_position", &CDM::RelativeMetadata::relativePosition)
        .def_readonly("relative_velocity", &CDM::RelativeMetadata::relativeVelocity)
        .def_readonly("start_screen_period", &CDM::RelativeMetadata::startScreenPeriod)
        .def_readonly("end_screen_period", &CDM::RelativeMetadata::endScreenPeriod)
        .def_readonly("screen_volume_frame", &CDM::RelativeMetadata::screenVolumeFrame)
        .def_readonly("screen_volume_shape", &CDM::RelativeMetadata::screenVolumeShape)
        .def_readonly("screen_volume_x", &CDM::RelativeMetadata::screenVolumeX)
        .def_readonly("screen_volume_y", &CDM::RelativeMetadata::screenVolumeY)
        .def_readonly("screen_volume_z", &CDM::RelativeMetadata::screenVolumeZ)
        .def_readonly("screen_entry_time", &CDM::RelativeMetadata::screenEntryTime)
        .def_readonly("screen_exit_time", &CDM::RelativeMetadata::screenExitTime)
        .def_readonly("collision_probability", &CDM::RelativeMetadata::collisionProbability)
        .def_readonly("collision_probability_method", &CDM::RelativeMetadata::collisionProbabilityMethod)

        ;

    class_<CDM::Metadata>(cdm, "Metadata")

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

        .def_readonly("comment", &CDM::Metadata::comment)
        .def_readonly("object", &CDM::Metadata::object)
        .def_readonly("object_designator", &CDM::Metadata::objectDesignator)
        .def_readonly("catalog_name", &CDM::Metadata::catalogName)
        .def_readonly("object_name", &CDM::Metadata::objectName)
        .def_readonly("international_designator", &CDM::Metadata::internationalDesignator)
        .def_readonly("object_type", &CDM::Metadata::objectType)
        .def_readonly("operator_contact_position", &CDM::Metadata::operatorContactPosition)
        .def_readonly("operator_organization", &CDM::Metadata::operatorOrgnization)
        .def_readonly("operator_phone", &CDM::Metadata::operatorPhone)
        .def_readonly("operator_email", &CDM::Metadata::operatorEmail)
        .def_readonly("ephemeris_name", &CDM::Metadata::ephemerisName)
        .def_readonly("covariance_method", &CDM::Metadata::covarianceMethod)
        .def_readonly("maneuverable", &CDM::Metadata::maneuverable)
        .def_readonly("orbit_center", &CDM::Metadata::orbitCenter)
        .def_readonly("reference_frame", &CDM::Metadata::refFrame)
        .def_readonly("gravity_model", &CDM::Metadata::gravityModel)
        .def_readonly("atmospheric_model", &CDM::Metadata::atmosphericModel)
        .def_readonly("n_body_perturbations", &CDM::Metadata::nBodyPerturbations)
        .def_readonly("solar_radiation_pressure", &CDM::Metadata::solarRadiationPressure)
        .def_readonly("earth_tides", &CDM::Metadata::earthTides)
        .def_readonly("in_track_thrust", &CDM::Metadata::inTrackThruster)

        ;

    class_<CDM::Data>(cdm, "Data")

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

        .def_readonly("time_last_observation_start", &CDM::Data::timeLastObStart)
        .def_readonly("time_last_observation_end", &CDM::Data::timeLastObEnd)
        .def_readonly("recommended_od_span", &CDM::Data::recommendedODSpan)
        .def_readonly("actual_od_span", &CDM::Data::actualODSpan)
        .def_readonly("observations_available", &CDM::Data::obsAvailable)
        .def_readonly("observations_used", &CDM::Data::obsUsed)
        .def_readonly("tracks_available", &CDM::Data::tracksAvailable)
        .def_readonly("tracks_used", &CDM::Data::tracksUsed)
        .def_readonly("residuals_accepted", &CDM::Data::residualsAccepted)
        .def_readonly("weighted_rms", &CDM::Data::weightedRMS)
        .def_readonly("area_pc", &CDM::Data::areaPC)
        .def_readonly("area_drag", &CDM::Data::areaDrag)
        .def_readonly("area_srp", &CDM::Data::areaSRP)
        .def_readonly("mass", &CDM::Data::mass)
        .def_readonly("cd_area_over_mass", &CDM::Data::cdAreaOverMass)
        .def_readonly("cr_area_over_mass", &CDM::Data::crAreaOverMass)
        .def_readonly("thrust_acceleration", &CDM::Data::thrustAcceleration)
        .def_readonly("sedr", &CDM::Data::SEDR)
        .def_readonly("state", &CDM::Data::state)
        .def_readonly("covariance_matrix", &CDM::Data::covarianceMatrix)

        ;
}
