# Apache License 2.0

import pandas as pd

import pytest

from datetime import datetime

from ostk.physics import Environment
from ostk.physics.units import Angle
from ostk.physics.units import Length
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Interval
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.coordinate.spherical import AER

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import State


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def scale() -> Scale:
    return Scale.UTC


@pytest.fixture
def instant_1(scale: Scale) -> Instant:
    return Instant.date_time(DateTime(2023, 1, 8, 13, 45, 34), scale)


@pytest.fixture
def instant_2(scale: Scale) -> Instant:
    return Instant.date_time(DateTime(2023, 1, 8, 13, 46, 34), scale)


@pytest.fixture
def instant_3(scale: Scale) -> Instant:
    return Instant.date_time(DateTime(2023, 1, 8, 13, 47, 34), scale)


@pytest.fixture
def instant(instant_1: Instant) -> Instant:
    return instant_1


@pytest.fixture
def interval(instant_1: Instant, instant_2: Instant) -> Interval:
    return Interval(instant_1, instant_2, Interval.Type.Closed)


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def position(frame: Frame) -> Position:
    return Position.meters([7000000.0, 0.0, 0.0], frame)


@pytest.fixture
def velocity(frame: Frame) -> Velocity:
    return Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)


@pytest.fixture
def state(instant: Instant, position: Position, velocity: Velocity) -> State:
    return State(instant, position, velocity)


@pytest.fixture
def latitude() -> Angle:
    return Angle(30.0, Angle.Unit.Degree)


@pytest.fixture
def longitude() -> Angle:
    return Angle(-111.0, Angle.Unit.Degree)


@pytest.fixture
def altitude() -> Length:
    return Length(10000.0, Length.Unit.Meter)


@pytest.fixture
def lla(latitude: Angle, longitude: Angle, altitude: Length) -> LLA:
    return LLA(latitude, longitude, altitude)


@pytest.fixture
def azimuth() -> Angle:
    return Angle(270.0, Angle.Unit.Degree)


@pytest.fixture
def elevation() -> Angle:
    return Angle(10.45, Angle.Unit.Degree)


@pytest.fixture
def range() -> Length:
    return Length(100000.0, Length.Unit.Meter)


@pytest.fixture
def aer(azimuth: Angle, elevation: Angle, range: Length) -> LLA:
    return AER(azimuth, elevation, range)


@pytest.fixture
def trajectory(position: Position) -> Trajectory:
    return Trajectory.position(position)


@pytest.fixture
def sample_gps_telemetry_column_names() -> list[str]:
    return [
        "_time",
        "adcs.meas.gps_ecef_pos_x",
        "adcs.meas.gps_ecef_pos_y",
        "adcs.meas.gps_ecef_pos_z",
        "adcs.meas.gps_ecef_vel_x",
        "adcs.meas.gps_ecef_vel_y",
        "adcs.meas.gps_ecef_vel_z",
        "adcs.meas.gps_pdop",
    ]


@pytest.fixture
def sample_gps_telemetry_list() -> list:
    return [
        [
            datetime.fromisoformat("2023-01-05T00:13:27.503"),
            -3578476.96,
            2110881.74,
            5508369.35,
            -4108.9619140625,
            4690.2236328125,
            -4470.1328125,
            3.0,
        ],
        [
            datetime.fromisoformat("2023-01-05T00:15:00.503"),
            -3938400.39,
            2537812.93,
            5064490.61,
            -3625.06201171875,
            4482.0263671875,
            -5067.38916015625,
            3.0,
        ],
        [
            datetime.fromisoformat("2023-01-05T00:20:00.503"),
            -4767226.88,
            3741539.81,
            3297412.08,
            -1857.27978515625,
            3456.681884765625,
            -6606.12451171875,
            3.0,
        ],
        [
            datetime.fromisoformat("2023-01-05T00:25:00.503"),
            -5037890.02,
            4566072.62,
            1172553.33,
            58.7464599609375,
            1975.9852294921875,
            -7430.14892578125,
            3.0,
        ],
        [
            datetime.fromisoformat("2023-01-05T00:30:00.503"),
            -4739694.78,
            4897336.32,
            -1079687.37,
            1898.0599365234375,
            199.03018188476562,
            -7447.29443359375,
            3.0,
        ],
        [
            datetime.fromisoformat("2023-01-05T00:35:00.503"),
            -3927840.85,
            4676052.8,
            -3214691.2,
            3450.987548828125,
            -1671.7001953125,
            -6656.1416015625,
            3.0,
        ],
        [
            datetime.fromisoformat("2023-01-05T00:40:00.503"),
            -2714686.67,
            3906926.3,
            -5001202.05,
            4550.12548828125,
            -3416.688720703125,
            -5145.97802734375,
            3.0,
        ],
        [
            datetime.fromisoformat("2023-01-05T00:45:00.503"),
            -1254041.83,
            2659654.6,
            -6246934.11,
            5088.9208984375,
            -4826.73095703125,
            -3084.27880859375,
            3.0,
        ],
        [
            datetime.fromisoformat("2023-01-05T00:50:00.503"),
            278743.68,
            1061970.2,
            -6818969.42,
            5030.94970703125,
            -5727.87060546875,
            -695.4893798828125,
            3.0,
        ],
        [
            datetime.fromisoformat("2023-01-05T00:55:00.503"),
            1707974.04,
            -714068.29,
            -6657080.21,
            4409.57666015625,
            -6001.28369140625,
            1764.5458984375,
            3.0,
        ],
    ]


@pytest.fixture
def sample_gps_telemetry_timestamp_index_df(
    sample_gps_telemetry_column_names: list[str],
    sample_gps_telemetry_list: list,
) -> pd.DataFrame:
    return pd.DataFrame(
        data=sample_gps_telemetry_list, columns=sample_gps_telemetry_column_names
    ).set_index("_time")


@pytest.fixture
def sample_gps_telemetry_no_timestamp_index_df(
    sample_gps_telemetry_column_names: list[str],
    sample_gps_telemetry_list: list,
) -> pd.DataFrame:
    return pd.DataFrame(
        data=sample_gps_telemetry_list, columns=sample_gps_telemetry_column_names
    )
