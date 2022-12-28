################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/conjunction/messages/ccsds/test_cdm.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

from datetime import datetime

from ostk.core.containers import Dictionary
from ostk.core.filesystem import Path
from ostk.core.filesystem import File

from ostk.physics.units import Length
from ostk.physics.units import Mass
from ostk.physics.time import Instant
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.conjunction.messages.ccsds import CDM

################################################################################################################################################################

@pytest.fixture
def cdm () -> CDM:

    return CDM(
        header = CDM.Header(
            ccsds_cdm_version = '1.0',
            creation_date = Instant.date_time(datetime(2010, 3, 12, 22, 31, 12), Scale.UTC),
            originator = 'JSPOC',
            message_for = 'SATELLITE A',
            message_id = '201113719185'
        ),
        relative_metadata = CDM.RelativeMetadata(
            comment = '',
            time_of_closest_approach = Instant.date_time(datetime(2010, 3, 13, 22, 37, 52, 618), Scale.UTC),
            miss_distance = Length.meters(715.0),
            start_screen_period = Instant.date_time(datetime(2010, 3, 12, 18, 29, 32, 212), Scale.UTC),
            end_screen_period = Instant.date_time(datetime(2010, 3, 15, 18, 29, 32, 212), Scale.UTC),
            screen_volume_frame = 'RTN',
            screen_volume_shape = 'ELLIPSOID',
            screen_volume_x = 200.0,
            screen_volume_y = 1000.0,
            screen_volume_z = 1000.0,
            screen_entry_time = Instant.date_time(datetime(2010, 3, 13, 22, 37, 52, 222), Scale.UTC),
            screen_exit_time = Instant.date_time(datetime(2010, 3, 13, 22, 37, 52, 824), Scale.UTC),
            collision_probability = 0.0000435,
            collision_probability_method = "FOSTER-1992"
        ),
        objects_metadata_array = [
            CDM.Metadata(
                comment = '',
                object = 'OBJECT1',
                object_designator = 12345,
                catalog_name = 'SATCAT',
                object_name = 'SATELLITE A',
                international_designator = '1997−030E',
                object_type = CDM.ObjectType.PAYLOAD,
                operator_contact_position = 'OSA',
                operator_organization = 'EUTMETSAT',
                operator_phone = '+49615130312',
                operator_email = 'JOHN.DOE@SOMEWHERE.NET',
                ephemeris_name = 'EPHEMERIS SATELLITE A',
                covariance_method = 'CALCULATED',
                maneuverable = 'YES',
                orbit_center = '',
                reference_frame = 'EME2000',
                gravity_model = 'EGM-96: 36D 36O',
                atmospheric_model = 'JACCHIA 70 DCA',
                n_body_perturbations = 'MOON, SUN',
                solar_radiation_pressure = False,
                earth_tides = False,
                in_track_thrust = False,
            ),
            CDM.Metadata(
                object = 'OBJECT2',
                object_designator = 12346,
                catalog_name = 'SATCAT',
                object_name = 'SATELLITE B',
                international_designator = '1997−030F',
                object_type = CDM.ObjectType.PAYLOAD,
                ephemeris_name = 'EPHEMERIS SATELLITE B',
                covariance_method = 'CALCULATED',
                maneuverable = 'YES',
                reference_frame = 'EME2000'
            ),
        ],
        objects_data_array = [
            CDM.Data(
                time_last_observation_start = Instant.date_time(datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC),
                time_last_observation_end = Instant.date_time(datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC),
                recommended_od_span = Duration.days(7.88),
                actual_od_span = Duration.days(5.50),
                observations_available = 592,
                observations_used = 579,
                tracks_available = 123,
                tracks_used = 119,
                residuals_accepted = 97.8,
                weighted_rms = 0.864,
                area_pc = 5.2,
                area_drag = 0.0,
                area_srp = 0.0,
                mass = Mass.kilograms(251.6),
                cd_area_over_mass = 0.0,
                cr_area_over_mass = 0.0,
                thrust_acceleration = 0.0,
                sedr = 0.0
            ),
            CDM.Data(
                time_last_observation_start = Instant.date_time(datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC),
                time_last_observation_end = Instant.date_time(datetime(2010, 3, 12, 2, 14, 12, 746), Scale.UTC),
                recommended_od_span = Duration.days(7.88),
                actual_od_span = Duration.days(5.50),
                observations_available = 592,
                observations_used = 579,
                tracks_available = 123,
                tracks_used = 119,
                residuals_accepted = 97.8,
                weighted_rms = 0.864,
                area_pc = 5.2,
                area_drag = 0.0,
                area_srp = 0.0,
                mass = Mass.kilograms(251.6),
                cd_area_over_mass = 0.0,
                cr_area_over_mass = 0.0,
                thrust_acceleration = 0.0,
                sedr = 0.0
            )
        ],
    )

################################################################################################################################################################

class TestCDM:

    def test_constructor (self, cdm: CDM):

        assert cdm is not None

    def test_get_header (self, cdm: CDM):

        assert cdm.get_header() is not None

    def test_get_relative_metadata (self, cdm: CDM):

        assert cdm.get_relative_metadata() is not None

    def test_undefined (self):

        assert CDM.undefined().is_defined() is False

    # def test_dictionary (self):

    #     dictionary = Dictionary({
    #         'header': {
    #             'generation_date': '2020-01-01T12:34:56.789Z',
    #             'launch_date': '2020-01-02T12:34:56.789Z',
    #         },
    #         'deployments': [{
    #             'name': 'satellite_a',
    #             'sequence_number': 1,
    #             'mission_time_s': 3600.0,
    #             'date': '2020-01-02T13:34:56.789Z',
    #             'r_ecef_m': [693289.644, 6876578.628, -133035.288],
    #             'v_ecef_m_per_s': [1305.783, 39.783, 7525.920],
    #             'mean_perigee_altitude_km': 526.768,
    #             'mean_apogee_altitude_km': 568.430,
    #             'mean_inclination_deg': 97.123,
    #             'mean_argument_of_perigee_deg': -179.513,
    #             'mean_longitude_ascending_node_deg': 85.057,
    #             'mean_mean_anomaly_deg': 179.263,
    #             'ballistic_coef_kg_per_m2': 47.55,
    #         }],
    #     })

    #     assert CDM.dictionary(dictionary) is not None

    # def test_parse (self, opm_file: File):

    #     with open(str(cdm_file.get_path().to_string()), 'r') as stream:
    #         assert CDM.parse(string = stream.read()) is not None

    # def test_load (self, opm_file: File):

    #     assert CDM.load(file = opm_file) is not None

################################################################################################################################################################
