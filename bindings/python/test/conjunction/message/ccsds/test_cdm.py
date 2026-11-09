# Apache License 2.0

import pytest

from datetime import datetime

from ostk.core.container import Dictionary
from ostk.core.filesystem import File

from ostk.physics.unit import Length
from ostk.physics.unit import Mass
from ostk.physics.time import Instant
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.conjunction.message.ccsds import CDM


def make_state() -> State:
    return State(
        Instant.date_time(datetime(2010, 3, 13, 22, 37, 52, 618), Scale.UTC),
        Position.meters([7.0e6, 0.0, 0.0], Frame.GCRF()),
        Velocity.meters_per_second([0.0, 7.5e3, 0.0], Frame.GCRF()),
    )


@pytest.fixture
def cdm() -> CDM:
    return CDM(
        header=CDM.Header(
            ccsds_cdm_version="1.0",
            creation_date=Instant.date_time(datetime(2010, 3, 12, 22, 31, 12), Scale.UTC),
            originator="JSPOC",
            message_for="SATELLITE A",
            message_id="201113719185",
        ),
        relative_metadata=CDM.RelativeMetadata(
            comment="",
            time_of_closest_approach=Instant.date_time(
                datetime(2010, 3, 13, 22, 37, 52, 618), Scale.UTC
            ),
            miss_distance=Length.meters(715.0),
            start_screen_period=Instant.date_time(
                datetime(2010, 3, 12, 18, 29, 32, 212), Scale.UTC
            ),
            end_screen_period=Instant.date_time(
                datetime(2010, 3, 15, 18, 29, 32, 212), Scale.UTC
            ),
            screen_volume_frame="RTN",
            screen_volume_shape="ELLIPSOID",
            screen_volume_x=200.0,
            screen_volume_y=1000.0,
            screen_volume_z=1000.0,
            screen_entry_time=Instant.date_time(
                datetime(2010, 3, 13, 22, 37, 52, 222), Scale.UTC
            ),
            screen_exit_time=Instant.date_time(
                datetime(2010, 3, 13, 22, 37, 52, 824), Scale.UTC
            ),
            collision_probability=0.0000435,
            collision_probability_method="FOSTER-1992",
        ),
        objects_metadata_array=[
            CDM.Metadata(
                comment="",
                object="OBJECT1",
                object_designator=12345,
                catalog_name="SATCAT",
                object_name="SATELLITE A",
                international_designator="1997−030E",
                object_type=CDM.ObjectType.Payload,
                operator_contact_position="OSA",
                operator_organization="EUTMETSAT",
                operator_phone="+49615130312",
                operator_email="JOHN.DOE@SOMEWHERE.NET",
                ephemeris_name="EPHEMERIS SATELLITE A",
                covariance_method="CALCULATED",
                maneuverable="YES",
                orbit_center="",
                reference_frame="EME2000",
                gravity_model="EGM-96: 36D 36O",
                atmospheric_model="JACCHIA 70 DCA",
                n_body_perturbations="MOON, SUN",
                solar_radiation_pressure=False,
                earth_tides=False,
                in_track_thrust=False,
            ),
            CDM.Metadata(
                object="OBJECT2",
                object_designator=12346,
                catalog_name="SATCAT",
                object_name="SATELLITE B",
                international_designator="1997−030F",
                object_type=CDM.ObjectType.Payload,
                ephemeris_name="EPHEMERIS SATELLITE B",
                covariance_method="CALCULATED",
                maneuverable="YES",
                reference_frame="EME2000",
            ),
        ],
        objects_data_array=[
            CDM.Data(
                time_last_observation_start=Instant.date_time(
                    datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC
                ),
                time_last_observation_end=Instant.date_time(
                    datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC
                ),
                recommended_od_span=Duration.days(7.88),
                actual_od_span=Duration.days(5.50),
                observations_available=592,
                observations_used=579,
                tracks_available=123,
                tracks_used=119,
                residuals_accepted=97.8,
                weighted_rms=0.864,
                area_pc=5.2,
                area_drag=0.0,
                area_srp=0.0,
                mass=Mass.kilograms(251.6),
                cd_area_over_mass=0.0,
                cr_area_over_mass=0.0,
                thrust_acceleration=0.0,
                sedr=0.0,
                state=make_state(),
            ),
            CDM.Data(
                time_last_observation_start=Instant.date_time(
                    datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC
                ),
                time_last_observation_end=Instant.date_time(
                    datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC
                ),
                recommended_od_span=Duration.days(7.88),
                actual_od_span=Duration.days(5.50),
                observations_available=592,
                observations_used=579,
                tracks_available=123,
                tracks_used=119,
                residuals_accepted=97.8,
                weighted_rms=0.864,
                area_pc=5.2,
                area_drag=0.0,
                area_srp=0.0,
                mass=Mass.kilograms(251.6),
                cd_area_over_mass=0.0,
                cr_area_over_mass=0.0,
                thrust_acceleration=0.0,
                sedr=0.0,
                state=make_state(),
            ),
        ],
    )


class TestCDM:
    def test_constructor(self, cdm: CDM):
        assert cdm is not None

    def test_get_header(self, cdm: CDM):
        cdm_header: CDM.Header = cdm.get_header()

        assert cdm_header is not None
        assert cdm_header.ccsds_cdm_version == "1.0"
        assert cdm_header.creation_date == Instant.date_time(
            datetime(2010, 3, 12, 22, 31, 12), Scale.UTC
        )
        assert cdm_header.originator == "JSPOC"
        assert cdm_header.message_for == "SATELLITE A"
        assert cdm_header.message_id == "201113719185"

    def test_get_relative_metadata(self, cdm: CDM):
        cdm_relative_metadata: CDM.RelativeMetadata = cdm.get_relative_metadata()

        assert cdm_relative_metadata is not None
        assert cdm_relative_metadata.comment == ""
        assert cdm_relative_metadata.time_of_closest_approach == Instant.date_time(
            datetime(2010, 3, 13, 22, 37, 52, 618), Scale.UTC
        )
        assert cdm_relative_metadata.miss_distance == Length.meters(715.0)
        assert cdm_relative_metadata.start_screen_period == Instant.date_time(
            datetime(2010, 3, 12, 18, 29, 32, 212), Scale.UTC
        )
        assert cdm_relative_metadata.end_screen_period == Instant.date_time(
            datetime(2010, 3, 15, 18, 29, 32, 212), Scale.UTC
        )
        assert cdm_relative_metadata.screen_volume_frame == "RTN"
        assert cdm_relative_metadata.screen_volume_shape == "ELLIPSOID"
        assert cdm_relative_metadata.screen_volume_x == 200.0
        assert cdm_relative_metadata.screen_volume_y == 1000.0
        assert cdm_relative_metadata.screen_volume_z == 1000.0
        assert cdm_relative_metadata.screen_entry_time == Instant.date_time(
            datetime(2010, 3, 13, 22, 37, 52, 222), Scale.UTC
        )
        assert cdm_relative_metadata.screen_exit_time == Instant.date_time(
            datetime(2010, 3, 13, 22, 37, 52, 824), Scale.UTC
        )
        assert cdm_relative_metadata.collision_probability == 0.0000435
        assert cdm_relative_metadata.collision_probability_method == "FOSTER-1992"

    def test_get_metadata_array(self, cdm: CDM):
        assert cdm.get_metadata_array() is not None
        assert len(cdm.get_metadata_array()) == 2

    def test_get_data_array(self, cdm: CDM):
        assert cdm.get_data_array() is not None
        assert len(cdm.get_data_array()) == 2

    def test_get_object_metadata_at(self, cdm: CDM):
        assert cdm.get_object_metadata_at(0) is not None
        assert cdm.get_object_metadata_at(1) is not None

        cdm_object_1_metadata: CDM.Metadata = cdm.get_object_metadata_at(0)

        assert cdm_object_1_metadata.comment == ""
        assert cdm_object_1_metadata.object == "OBJECT1"
        assert cdm_object_1_metadata.object_designator == 12345
        assert cdm_object_1_metadata.catalog_name == "SATCAT"
        assert cdm_object_1_metadata.object_name == "SATELLITE A"
        assert cdm_object_1_metadata.international_designator == "1997−030E"
        assert cdm_object_1_metadata.object_type == CDM.ObjectType.Payload
        assert cdm_object_1_metadata.operator_contact_position == "OSA"
        assert cdm_object_1_metadata.operator_organization == "EUTMETSAT"
        assert cdm_object_1_metadata.operator_phone == "+49615130312"
        assert cdm_object_1_metadata.operator_email == "JOHN.DOE@SOMEWHERE.NET"
        assert cdm_object_1_metadata.ephemeris_name == "EPHEMERIS SATELLITE A"
        assert cdm_object_1_metadata.covariance_method == "CALCULATED"
        assert cdm_object_1_metadata.maneuverable == "YES"
        assert cdm_object_1_metadata.orbit_center == ""
        assert cdm_object_1_metadata.reference_frame == "EME2000"
        assert cdm_object_1_metadata.gravity_model == "EGM-96: 36D 36O"
        assert cdm_object_1_metadata.atmospheric_model == "JACCHIA 70 DCA"
        assert cdm_object_1_metadata.n_body_perturbations == "MOON, SUN"
        assert cdm_object_1_metadata.solar_radiation_pressure == False
        assert cdm_object_1_metadata.earth_tides == False
        assert cdm_object_1_metadata.in_track_thrust == False

        cdm_object_2_metadata: CDM.Metadata = cdm.get_object_metadata_at(1)

        assert cdm_object_2_metadata.comment == ""
        assert cdm_object_2_metadata.object == "OBJECT2"
        assert cdm_object_2_metadata.object_designator == 12346
        assert cdm_object_2_metadata.catalog_name == "SATCAT"
        assert cdm_object_2_metadata.object_name == "SATELLITE B"
        assert cdm_object_2_metadata.international_designator == "1997−030F"
        assert cdm_object_2_metadata.object_type == CDM.ObjectType.Payload
        assert cdm_object_2_metadata.operator_contact_position == ""
        assert cdm_object_2_metadata.operator_organization == ""
        assert cdm_object_2_metadata.operator_phone == ""
        assert cdm_object_2_metadata.operator_email == ""
        assert cdm_object_2_metadata.ephemeris_name == "EPHEMERIS SATELLITE B"
        assert cdm_object_2_metadata.covariance_method == "CALCULATED"
        assert cdm_object_2_metadata.maneuverable == "YES"
        assert cdm_object_2_metadata.orbit_center == ""
        assert cdm_object_2_metadata.reference_frame == "EME2000"
        assert cdm_object_2_metadata.gravity_model == ""
        assert cdm_object_2_metadata.atmospheric_model == ""
        assert cdm_object_2_metadata.n_body_perturbations == ""
        assert cdm_object_2_metadata.solar_radiation_pressure == False
        assert cdm_object_2_metadata.earth_tides == False
        assert cdm_object_2_metadata.in_track_thrust == False

        with pytest.raises(Exception) as e:
            cdm.get_object_metadata_at(2)

    def test_get_object_data_at(self, cdm: CDM):
        assert cdm.get_object_data_at(0) is not None
        assert cdm.get_object_data_at(1) is not None

        cdm_object_1_data: CDM.Data = cdm.get_object_data_at(0)

        assert cdm_object_1_data.time_last_observation_start == Instant.date_time(
            datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC
        )
        assert cdm_object_1_data.time_last_observation_end == Instant.date_time(
            datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC
        )
        assert cdm_object_1_data.recommended_od_span == Duration.days(7.88)
        assert cdm_object_1_data.actual_od_span == Duration.days(5.50)
        assert cdm_object_1_data.observations_available == 592
        assert cdm_object_1_data.observations_used == 579
        assert cdm_object_1_data.tracks_available == 123
        assert cdm_object_1_data.tracks_used == 119
        assert cdm_object_1_data.residuals_accepted == 97.8
        assert cdm_object_1_data.weighted_rms == 0.864
        assert cdm_object_1_data.area_pc == 5.2
        assert cdm_object_1_data.area_drag == 0.0
        assert cdm_object_1_data.area_srp == 0.0
        assert cdm_object_1_data.mass == Mass.kilograms(251.6)
        assert cdm_object_1_data.cd_area_over_mass == 0.0
        assert cdm_object_1_data.cr_area_over_mass == 0.0
        assert cdm_object_1_data.thrust_acceleration == 0.0
        assert cdm_object_1_data.sedr == 0.0

        cdm_object_2_data: CDM.Data = cdm.get_object_data_at(1)

        assert cdm_object_2_data.time_last_observation_start == Instant.date_time(
            datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC
        )
        assert cdm_object_2_data.time_last_observation_end == Instant.date_time(
            datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC
        )
        assert cdm_object_2_data.recommended_od_span == Duration.days(7.88)
        assert cdm_object_2_data.actual_od_span == Duration.days(5.50)
        assert cdm_object_2_data.observations_available == 592
        assert cdm_object_2_data.observations_used == 579
        assert cdm_object_2_data.tracks_available == 123
        assert cdm_object_2_data.tracks_used == 119
        assert cdm_object_2_data.residuals_accepted == 97.8
        assert cdm_object_2_data.weighted_rms == 0.864
        assert cdm_object_2_data.area_pc == 5.2
        assert cdm_object_2_data.area_drag == 0.0
        assert cdm_object_2_data.area_srp == 0.0
        assert cdm_object_2_data.mass == Mass.kilograms(251.6)
        assert cdm_object_2_data.cd_area_over_mass == 0.0
        assert cdm_object_2_data.cr_area_over_mass == 0.0
        assert cdm_object_2_data.thrust_acceleration == 0.0
        assert cdm_object_2_data.sedr == 0.0

        with pytest.raises(Exception) as e:
            cdm.get_object_data_at(2)

    def test_data_without_optional_values(self):
        data = CDM.Data(
            time_last_observation_start=None,
            time_last_observation_end=None,
            recommended_od_span=None,
            actual_od_span=None,
            observations_available=None,
            observations_used=None,
            tracks_available=None,
            tracks_used=None,
            residuals_accepted=None,
            weighted_rms=None,
            area_pc=None,
            area_drag=None,
            area_srp=None,
            mass=Mass.kilograms(1.0),
            cd_area_over_mass=None,
            cr_area_over_mass=None,
            thrust_acceleration=None,
            sedr=None,
            state=make_state(),
        )

        assert data.observations_available is None
        assert data.observations_used is None
        assert data.tracks_available is None
        assert data.tracks_used is None
        assert data.residuals_accepted is None
        assert data.weighted_rms is None
        assert data.area_pc is None
        assert data.area_drag is None
        assert data.area_srp is None
        assert data.cd_area_over_mass is None
        assert data.cr_area_over_mass is None
        assert data.thrust_acceleration is None
        assert data.sedr is None

    def test_data_with_optional_values(self):
        data = CDM.Data(
            time_last_observation_start=None,
            time_last_observation_end=None,
            recommended_od_span=None,
            actual_od_span=None,
            observations_available=None,
            observations_used=None,
            tracks_available=None,
            tracks_used=None,
            residuals_accepted=97.8,
            weighted_rms=0.864,
            area_pc=5.2,
            area_drag=1.0,
            area_srp=2.0,
            mass=Mass.kilograms(1.0),
            cd_area_over_mass=0.1,
            cr_area_over_mass=0.2,
            thrust_acceleration=0.0,
            sedr=0.3,
            state=make_state(),
        )

        assert data.residuals_accepted == 97.8
        assert data.weighted_rms == 0.864
        assert data.area_pc == 5.2
        assert data.area_drag == 1.0
        assert data.area_srp == 2.0
        assert data.cd_area_over_mass == 0.1
        assert data.cr_area_over_mass == 0.2
        assert data.thrust_acceleration == 0.0
        assert data.sedr == 0.3

    def test_relative_metadata_without_optional_values(self):
        relative_metadata = CDM.RelativeMetadata(
            time_of_closest_approach=Instant.date_time(
                datetime(2010, 3, 13, 22, 37, 52, 618), Scale.UTC
            ),
            miss_distance=Length.meters(715.0),
            start_screen_period=None,
            end_screen_period=None,
            screen_entry_time=None,
            screen_exit_time=None,
            collision_probability_method="FOSTER-1992",
        )

        assert relative_metadata.screen_volume_x is None
        assert relative_metadata.screen_volume_y is None
        assert relative_metadata.screen_volume_z is None
        assert relative_metadata.collision_probability is None

    def test_get_collision_probability_none(self, cdm: CDM):
        cdm_without_probability = CDM(
            header=cdm.get_header(),
            relative_metadata=CDM.RelativeMetadata(
                time_of_closest_approach=cdm.get_time_of_closest_approach(),
                miss_distance=cdm.get_miss_distance(),
                start_screen_period=None,
                end_screen_period=None,
                screen_entry_time=None,
                screen_exit_time=None,
                collision_probability=None,
                collision_probability_method="FOSTER-1992",
            ),
            objects_metadata_array=cdm.get_metadata_array(),
            objects_data_array=cdm.get_data_array(),
        )

        assert cdm_without_probability.get_collision_probability() is None

    def test_dictionary_without_collision_probability(
        self, cdm_spacetrack_dictionary: dict
    ):
        cdm_spacetrack_dictionary["COLLISION_PROBABILITY"] = ""

        cdm: CDM = CDM.dictionary(Dictionary(cdm_spacetrack_dictionary))

        assert cdm.get_collision_probability() is None

        cdm_spacetrack_dictionary["COLLISION_PROBABILITY"] = "NaN"

        with pytest.raises(Exception):
            CDM.dictionary(Dictionary(cdm_spacetrack_dictionary))

    def test_get_ccsds_cdm_version(self, cdm: CDM):
        assert cdm.get_ccsds_cdm_version() == "1.0"

    def test_get_creation_instant(self, cdm: CDM):
        assert cdm.get_creation_instant() == Instant.date_time(
            datetime(2010, 3, 12, 22, 31, 12), Scale.UTC
        )

    def test_get_originator(self, cdm: CDM):
        assert cdm.get_originator() == "JSPOC"

    def test_get_message_for(self, cdm: CDM):
        assert cdm.get_message_for() == "SATELLITE A"

    def test_get_message_id(self, cdm: CDM):
        assert cdm.get_message_id() == "201113719185"

    def test_get_time_of_closest_approach(self, cdm: CDM):
        assert cdm.get_time_of_closest_approach() == Instant.date_time(
            datetime(2010, 3, 13, 22, 37, 52, 618), Scale.UTC
        )

    def test_get_miss_distance(self, cdm: CDM):
        assert cdm.get_miss_distance() == Length.meters(715.0)

    def test_get_relative_position(self, cdm: CDM):
        assert cdm.get_relative_position() is None

    def test_get_relative_velocity(self, cdm: CDM):
        assert cdm.get_relative_velocity() is None

    def test_get_collision_probability(self, cdm: CDM):
        assert cdm.get_collision_probability() == 0.0000435

    def test_get_collision_probability_method(self, cdm: CDM):
        assert cdm.get_collision_probability_method() == "FOSTER-1992"

    def test_get_relative_position_and_velocity_provided(self, cdm: CDM):
        relative_metadata = CDM.RelativeMetadata(
            time_of_closest_approach=cdm.get_time_of_closest_approach(),
            miss_distance=cdm.get_miss_distance(),
            relative_position=Position.meters([27.4, -70.2, 711.8], Frame.GCRF()),
            relative_velocity=Velocity.meters_per_second(
                [-7.2, 14692.0, -1437.2], Frame.GCRF()
            ),
            collision_probability_method="FOSTER-1992",
        )

        cdm_with_relative_state = CDM(
            header=cdm.get_header(),
            relative_metadata=relative_metadata,
            objects_metadata_array=cdm.get_metadata_array(),
            objects_data_array=cdm.get_data_array(),
        )

        assert cdm_with_relative_state.get_relative_position() is not None
        assert cdm_with_relative_state.get_relative_velocity() is not None

    def test_dictionary(self, cdm_spacetrack_dictionary: dict):
        dictionary = Dictionary(cdm_spacetrack_dictionary)

        cdm: CDM = CDM.dictionary(dictionary)

        assert cdm is not None
        assert cdm.get_header() is not None
        assert cdm.get_ccsds_cdm_version() == "1.0"
        assert cdm.get_creation_instant() == Instant.date_time(
            datetime(2022, 12, 25, 0, 33, 16), Scale.UTC
        )
        assert cdm.get_originator() == "CSpOC"
        assert cdm.get_message_for() == "YAM-2"
        assert cdm.get_message_id() == "406320986"
        assert cdm.get_time_of_closest_approach() == Instant.date_time(
            datetime(2022, 12, 27, 13, 28, 59, 516000), Scale.UTC
        )
        assert cdm.get_miss_distance() == Length.meters(974.0)
        assert cdm.get_relative_position() is None
        assert cdm.get_relative_velocity() is None
        assert cdm.get_collision_probability() == 5.162516e-16
        assert cdm.get_collision_probability_method() == "FOSTER-1992"

        object_1_metadata: CDM.Metadata = cdm.get_object_metadata_at(0)

        assert object_1_metadata.object == "OBJECT1"
        assert object_1_metadata.object_designator == 48911
        assert object_1_metadata.catalog_name == "SATCAT"
        assert object_1_metadata.object_name == "YAM-2"
        assert object_1_metadata.international_designator == "2021-059AJ"
        assert object_1_metadata.object_type == CDM.ObjectType.Payload
        assert object_1_metadata.ephemeris_name == "NONE"
        assert object_1_metadata.covariance_method == "CALCULATED"
        assert object_1_metadata.maneuverable == "N/A"
        assert object_1_metadata.reference_frame == "ITRF"

        object_1_data_counts: CDM.Data = cdm.get_object_data_at(0)

        assert object_1_data_counts.observations_available is None
        assert object_1_data_counts.observations_used is None
        assert object_1_data_counts.tracks_available is None
        assert object_1_data_counts.tracks_used is None

        # object_1_data: CDM.Data = cdm.get_object_data_at(0)

        # assert object_1_data.state == State(
        #     Instant.date_time(datetime(2022, 12, 27, 13, 28, 59, 516000), Scale.UTC),
        #     Position.meters([-4988150.231999999844, -1691825.955000000075, -4469421.481999999844], Frame.ITRF()),
        #     Velocity.meters_per_second([-5122.248844000000, 54.300816000000, 5699.434412000000], Frame.ITRF())
        # )

    def test_parse(self, cdm_file: File):
        with open(str(cdm_file.get_path().to_string()), "r") as stream:
            assert CDM.parse(string=stream.read()) is not None

    def test_load(self, cdm_file: File):
        assert CDM.load(file=cdm_file) is not None

    def test_object_type_from_string(self):
        assert CDM.object_type_from_string("PAYLOAD") == CDM.ObjectType.Payload
        assert CDM.object_type_from_string("ROCKET BODY") == CDM.ObjectType.RocketBody
        assert CDM.object_type_from_string("DEBRIS") == CDM.ObjectType.Debris
        assert CDM.object_type_from_string("UNKNOWN") == CDM.ObjectType.Unknown
        assert CDM.object_type_from_string("OTHER") == CDM.ObjectType.Other

        with pytest.raises(Exception) as e:
            CDM.object_type_from_string("ANOTHERSUPPORTEDTYPE")
