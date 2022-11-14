################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/trajectory/orbit/messages/spacex/test_opm.py
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

from datetime import datetime

from ostk.core.containers import Dictionary
from ostk.core.filesystem import Path
from ostk.core.filesystem import File

from ostk.physics.units import Length
from ostk.physics.units import Angle
from ostk.physics.time import Instant
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory.orbit.messages.spacex import OPM

################################################################################################################################################################

@pytest.fixture
def opm () -> OPM:

    return OPM(
        header = OPM.Header(
            generation_date = Instant.date_time(datetime(2020, 1, 2, 3, 4, 5), Scale.UTC),
            launch_date = Instant.date_time(datetime(2020, 1, 3, 3, 4, 5), Scale.UTC),
        ),
        deployments = [
            OPM.Deployment(
                name = 'A',
                sequence_number = 1,
                mission_time = Duration.hours(3600.0),
                date = Instant.date_time(datetime(2020, 1, 4, 3, 4, 5), Scale.UTC),
                position = Position.meters((1.0, 2.0, 3.0), Frame.ITRF()),
                velocity = Velocity.meters_per_second((4.0, 5.0, 6.0), Frame.ITRF()),
                mean_perigee_altitude = Length.kilometers(500.0),
                mean_apogee_altitude = Length.kilometers(500.0),
                mean_inclination = Angle.degrees(1.0),
                mean_argument_of_perigee = Angle.degrees(2.0),
                mean_longitude_ascending_node = Angle.degrees(3.0),
                mean_mean_anomaly = Angle.degrees(4.0),
                ballistic_coefficient = 123.456,
            ),
        ],
    )

################################################################################################################################################################

class TestOPM:

    def test_constructor (self, opm: OPM):

        assert opm is not None

    def test_get_header (self, opm: OPM):

        assert opm.get_header() is not None

    def test_get_deployments (self, opm: OPM):

        assert len(opm.get_deployments()) == 1

    def test_get_deployment_at (self, opm: OPM):

        assert opm.get_deployment_at(index = 0).name == 'A'

    def test_get_deployment_with_name (self, opm: OPM):

        assert opm.get_deployment_with_name(name = 'A').name == 'A'

    def test_undefined (self):

        assert OPM.undefined().is_defined() is False

    def test_dictionary (self):

        dictionary = Dictionary({
            'header': {
                'generation_date': '2020-01-01T12:34:56.789Z',
                'launch_date': '2020-01-02T12:34:56.789Z',
            },
            'deployments': [{
                'name': 'satellite_a',
                'sequence_number': 1,
                'mission_time_s': 3600.0,
                'date': '2020-01-02T13:34:56.789Z',
                'r_ecef_m': [693289.644, 6876578.628, -133035.288],
                'v_ecef_m_per_s': [1305.783, 39.783, 7525.920],
                'mean_perigee_altitude_km': 526.768,
                'mean_apogee_altitude_km': 568.430,
                'mean_inclination_deg': 97.123,
                'mean_argument_of_perigee_deg': -179.513,
                'mean_longitude_ascending_node_deg': 85.057,
                'mean_mean_anomaly_deg': 179.263,
                'ballistic_coef_kg_per_m2': 47.55,
            }],
        })

        assert OPM.dictionary(dictionary) is not None

    def test_parse (self, opm_file: File):

        with open(str(opm_file.get_path().to_string()), 'r') as stream:
            assert OPM.parse(string = stream.read()) is not None

    def test_load (self, opm_file: File):

        assert OPM.load(file = opm_file) is not None

################################################################################################################################################################

class TestOPMDeployment:

    def test_to_state (self, opm: OPM):

        assert opm.get_deployment_with_name(name = 'A').to_state() is not None

################################################################################################################################################################
