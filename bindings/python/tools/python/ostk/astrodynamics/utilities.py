# Apache License 2.0

import numpy as np

import pandas as pd

from ostk.physics import Environment
from ostk.physics.time import Scale
from ostk.physics.time import Instant
from ostk.physics.time import Duration
from ostk.physics.time import Interval
from ostk.physics.units import Angle, Length, Mass
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.coordinate.spherical import AER
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.environment.objects.celestial_bodies import Earth
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel

from ostk.astrodynamics import converters
from .OpenSpaceToolkitAstrodynamicsPy import *


def lla_from_state(state: trajectory.State) -> list:
    """
    Return latitude (degrees), longitude (degrees), altitude (meters) float list from a state.
    """

    lla = lla_from_position(state.get_position(), state.get_instant())

    return [
        float(lla.get_latitude().in_degrees()),
        float(lla.get_longitude().in_degrees()),
        float(lla.get_altitude().in_meters()),
    ]


def lla_from_position(
    position: Position,
    instant: Instant | None = None,
) -> LLA:
    """
    Return LLA from position and instant.
    """

    if position.access_frame() != Frame.ITRF():
        if instant is None:
            raise ValueError(
                "Instant must be provided if position is not expressed in ECEF."
            )
        position = position.in_frame(Frame.ITRF(), instant)

    return LLA.cartesian(
        position.get_coordinates(),
        EarthGravitationalModel.EGM2008.equatorial_radius,
        EarthGravitationalModel.EGM2008.flattening,
    )


def position_from_lla(lla: LLA) -> Position:
    """
    Return position from lla.
    """

    return Position.meters(
        lla.to_cartesian(
            EarthGravitationalModel.EGM2008.equatorial_radius,
            EarthGravitationalModel.EGM2008.flattening,
        ),
        Frame.ITRF(),
    )


def parse_ground_station_lla(ground_station):
    return LLA(
        Angle.degrees(ground_station["latitude"]),
        Angle.degrees(ground_station["longitude"]),
        Length.meters(ground_station["altitude"]),
    )


def compute_aer(
    instant: Instant,
    from_position: Position,
    to_position: Position,
    environment: Environment,
) -> list:
    """
    Return [azimuth (degrees), elevation (degrees), range (meters)] from Instant and Positions (observer, target).
    """

    from_lla = lla_from_position(from_position, instant)

    earth = environment.access_celestial_object_with_name("Earth")
    ned_frame = earth.get_frame_at(from_lla, Earth.FrameType.NED)

    from_position_NED = from_position.in_frame(ned_frame, instant)
    to_position_NED = to_position.in_frame(ned_frame, instant)

    aer = AER.from_position_to_position(from_position_NED, to_position_NED, True)

    return [
        float(aer.get_azimuth().in_degrees()),
        float(aer.get_elevation().in_degrees()),
        float(aer.get_range().in_meters()),
    ]


def compute_time_lla_aer_state(
    state: trajectory.State,
    from_position: Position,
    environment: Environment,
) -> list:
    """
    Return [instant, latitude, longitude, altitude, azimuth, elevation, range] from State and observer Position.
    """

    instant = state.get_instant()

    lla = lla_from_state(state)
    aer = compute_aer(
        instant,
        from_position,
        state.get_position().in_frame(Frame.ITRF(), state.get_instant()),
        environment,
    )

    return [instant, lla[0], lla[1], lla[2], aer[0], aer[1], aer[2]]


def compute_trajectory_geometry(trajectory: Trajectory, interval: Interval) -> list:
    """
    Return [latitude (degrees), longitude (degrees), altitude (meters)] values along a Trajectory during Interval.
    """

    return [
        lla_from_state(state)
        for state in trajectory.get_states_at(
            interval.generate_grid(Duration.minutes(1.0))
        )
    ]


def convert_state(instant: Instant, state: trajectory.State) -> list:
    """
    Convert an input (Instant, State) into dataframe-ready values.
    """

    lla = LLA.cartesian(
        state.get_position()
        .in_frame(Frame.ITRF(), state.get_instant())
        .get_coordinates(),
        EarthGravitationalModel.EGM2008.equatorial_radius,
        EarthGravitationalModel.EGM2008.flattening,
    )

    return [
        repr(instant),
        float(instant.get_modified_julian_date(Scale.UTC)),
        *state.get_position().get_coordinates().transpose().tolist(),
        *state.get_velocity().get_coordinates().transpose().tolist(),
        float(lla.get_latitude().in_degrees()),
        float(lla.get_longitude().in_degrees()),
        float(lla.get_altitude().in_meters()),
    ]


def generate_states_from_dataframe(
    dataframe: pd.DataFrame,
    position_columns: list[str],
    velocity_columns: list[str],
    reference_frame: Frame,
    time_column_if_needed: str | None = None,
) -> list[trajectory.State]:
    """
    Generate a list of OSTk States from a Pandas DataFrame.

    Args:
        dataframe (pd.DataFrame): Pandas DataFrame containing the orbit data.
        time_column (str): Name of the column containing the time data in [UTC].
        position_columns (list[str]): List of column names containing the position data in [m].
        velocity_columns (list[str]): List of column names containing the velocity data in [m/s].
        reference_frame (Frame): Reference frame.
        has_timestamp_as_index (bool): Whether the timestamp for each data point is being used as the DataFrame's index.

    Returns:
        list[State]: List of OSTk States.
    """

    def _get_ostk_state(row: pd.Series) -> trajectory.State:
        return trajectory.State(
            instant=converters.coerce_to_instant(
                row.name
                if time_column_if_needed is None
                else getattr(row, time_column_if_needed),
            ),
            position=Position.meters(
                np.array([getattr(row, column) for column in position_columns]),
                reference_frame,
            ),
            velocity=Velocity.meters_per_second(
                np.array([getattr(row, column) for column in velocity_columns]),
                reference_frame,
            ),
        )

    return dataframe.apply(_get_ostk_state, axis=1).tolist()


def generate_norm_wrt_reference_state(
    truth_state: trajectory.State,
    estimated_state: trajectory.State,
) -> dict:
    return {
        "time": truth_state.get_instant().get_date_time(Scale.UTC),
        "dr": np.linalg.norm(
            (truth_state - estimated_state).get_position().get_coordinates()
        ),
    }


def generate_norm_wrt_reference_state_df(
    truth_state: trajectory.State,
    estimated_state: trajectory.State,
) -> dict:
    # current_df = pd.DataFrame(
    #     [
    #         utilities.generate_norm_wrt_reference_state(truth_state, estimated_state)
    #         for (truth_state, estimated_state) in zip(validation_states, current_states)
    #     ]
    # )
    return {
        "time": truth_state.get_instant().get_date_time(Scale.UTC),
        "dr": np.linalg.norm(
            (truth_state - estimated_state).get_position().get_coordinates()
        ),
    }


def parse_ground_station_lla(ground_station):
    return LLA(
        Angle.degrees(ground_station["latitude"]),
        Angle.degrees(ground_station["longitude"]),
        Length.meters(ground_station["altitude"]),
    )


def compute_access_geometry(orbit, access, ground_station):
    ground_station_position = utilities.position_from_lla(
        parse_ground_station_lla(ground_station)
    )

    return [
        compute_time_lla_aer_state(state, ground_station_position)
        for state in orbit.get_states_at(
            access.get_interval().generate_grid(Duration.seconds(1.0))
        )
    ]


def get_max_elevation(df):
    return df.loc[df["Elevation"].idxmax()]["Elevation"]


def generate_orbit_df(orbit, interval, step):
    if isinstance(interval, tuple):
        interval = Interval.closed(
            Instant.date_time(interval[0], Scale.UTC),
            Instant.date_time(interval[1], Scale.UTC),
        )

    if isinstance(step, datetime.timedelta):
        step = Duration.seconds(step.total_seconds())

    orbit_data = [
        convert_state(instant, orbit.get_state_at(instant))
        for instant in interval.generate_grid(step)
    ]

    return pd.DataFrame(
        orbit_data,
        columns=[
            "$Time^{UTC}$",
            "$MJD^{UTC}$",
            "$x_{x}^{ECI}$",
            "$x_{y}^{ECI}$",
            "$x_{z}^{ECI}$",
            "$v_{x}^{ECI}$",
            "$v_{y}^{ECI}$",
            "$v_{z}^{ECI}$",
            "$Latitude$",
            "$Longitude$",
            "$Altitude$",
        ],
    )


def generate_accesses(analysis_interval, orbit, ground_stations, environment):
    if isinstance(analysis_interval, tuple):
        analysis_interval = Interval.closed(
            Instant.date_time(analysis_interval[0], Scale.UTC),
            Instant.date_time(analysis_interval[1], Scale.UTC),
        )

    accesses = []

    for ground_station_name, ground_station in ground_stations.items():
        # Groud station position
        lla = parse_ground_station_lla(ground_station)
        position = utilities.position_from_lla(lla)

        # Access constraints
        azimuth_range = RealInterval.closed(0.0, 360.0)  # [deg]
        elevation_range = RealInterval.closed(
            ground_station["min_elevation_angle"], 90.0
        )  # [deg]
        range_range = RealInterval.closed(0.0, 10000e3)  # [m]

        # Access generator with Azimuth-Range-Elevation constraints
        access_generator = AccessGenerator.aer_ranges(
            azimuth_range, elevation_range, range_range, environment
        )

        # Generate Accesses
        accesses += [
            (ground_station_name, ground_station, access)
            for access in access_generator.compute_accesses(
                analysis_interval, Trajectory.position(position), orbit
            )
        ]

    return accesses


def generate_accesses_df(accesses):
    df = pd.DataFrame(
        [
            {
                "Ground Station": ground_station_name,
                "Type": access.get_type().name,
                "AOS (UTC)": access.get_acquisition_of_signal().get_date_time(
                    Scale.UTC
                ),
                "TCA (UTC)": access.get_time_of_closest_approach().get_date_time(
                    Scale.UTC
                ),
                "LOS (UTC)": access.get_loss_of_signal().get_date_time(Scale.UTC),
                "Duration": datetime.timedelta(
                    seconds=float(access.get_duration().in_seconds())
                ),
            }
            for (ground_station_name, ground_station, access) in accesses
        ]
    )

    df = df.sort_values(["AOS (UTC)", "LOS (UTC)"], ascending=True)

    return df


def generate_access_geometries_df(orbit, access, ground_station):
    return pd.DataFrame(
        compute_access_geometry(orbit, access, ground_station),
        columns=[
            "Time",
            "Latitude",
            "Longitude",
            "Altitude",
            "Azimuth",
            "Elevation",
            "Range",
        ],
    )


def flatten(t):
    return [item for sublist in t for item in sublist]


def get_tle_index(tles, description):
    try:
        return next(
            i for (i, tle) in enumerate(tles) if tle["description"] == description
        )
    except:
        raise ValueError(f"Cannot find TLE with description [{description}].")
