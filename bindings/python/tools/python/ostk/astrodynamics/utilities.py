# Apache License 2.0

from datetime import datetime
from datetime import timezone

from .OpenSpaceToolkitAstrodynamicsPy import *

from ostk.physics import Environment
from ostk.physics.time import Scale
from ostk.physics.time import Instant
from ostk.physics.time import Duration
from ostk.physics.time import Interval
from ostk.physics.unit import Length
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.coordinate.spherical import AER
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel


def lla_from_state(state: trajectory.State) -> LLA:
    """
    Return latitude (degrees), longitude (degrees), altitude (meters) float list from a state.

    Args:
        state (trajectory.State): A state.

    Returns:
        LLA: The LLA.
    """

    return lla_from_position(state.get_position(), state.get_instant())


def lla_from_position(
    position: Position,
    instant: Instant | None = None,
) -> LLA:
    """
    Return LLA from position and instant.

    Args:
        position (Position): A position.
        instant (Instant): An instant.

    Returns:
        LLA: The LLA.
    """

    if position.access_frame() != Frame.ITRF():
        if instant is None:
            raise ValueError(
                "Instant must be provided if position is not expressed in ECEF."
            )
        position: Position = position.in_frame(Frame.ITRF(), instant)

    return LLA.cartesian(
        position.get_coordinates(),
        EarthGravitationalModel.EGM2008.equatorial_radius,
        EarthGravitationalModel.EGM2008.flattening,
    )


def position_from_lla(lla: LLA) -> Position:
    """
    Return position from lla.

    Args:
        lla (LLA): A LLA.

    Returns:
        Position: The position.
    """

    return Position.meters(
        lla.to_cartesian(
            EarthGravitationalModel.EGM2008.equatorial_radius,
            EarthGravitationalModel.EGM2008.flattening,
        ),
        Frame.ITRF(),
    )


def compute_aer(
    instant: Instant,
    from_position: Position,
    to_position: Position,
    environment: Environment,
) -> AER:
    """
    Return AER from Instant and Positions (observer, target).

    Args:
        instant (Instant): An instant.
        from_position (Position): An observer position.
        to_position (Position): A target position.
        environment (Environment): An environment.

    Returns:
        AER: The AER.
    """

    from_lla: LLA = lla_from_position(from_position, instant)

    earth: Earth = environment.access_celestial_object_with_name("Earth")
    ned_frame: Frame = earth.get_frame_at(from_lla, Earth.FrameType.NED)

    from_position_NED: Position = from_position.in_frame(ned_frame, instant)
    to_position_NED: Position = to_position.in_frame(ned_frame, instant)

    return AER.from_position_to_position(from_position_NED, to_position_NED, True)


def compute_time_lla_aer_coordinates(
    state: trajectory.State,
    from_position: Position,
    environment: Environment,
) -> tuple[datetime, float, float, float, float, float, float]:
    """
    Return [datetime, latitude, longitude, altitude, azimuth, elevation, range] from State and observer Position.

    Args:
        state (trajectory.State): A state.
        from_position (Position): An observer position.
        environment (Environment): An environment.

    Returns:
        tuple[datetime, float, float, float, float, float, float]: The [datetime, latitude, longitude, altitude, azimuth, elevation, range].
    """

    instant: Instant = state.get_instant()

    lla: LLA = lla_from_state(state)

    aer: AER = compute_aer(
        instant,
        from_position,
        state.get_position().in_frame(Frame.ITRF(), state.get_instant()),
        environment,
    )

    return (
        instant.get_date_time(Scale.UTC).replace(tzinfo=timezone.utc),
        float(lla.get_latitude().in_degrees()),
        float(lla.get_longitude().in_degrees()),
        float(lla.get_altitude().in_meters()),
        float(aer.get_azimuth().in_degrees()),
        float(aer.get_elevation().in_degrees()),
        float(aer.get_range().in_meters()),
    )


def compute_trajectory_geometry(
    trajectory: Trajectory,
    interval: Interval,
    step: Duration = Duration.minutes(1.0),
) -> list[LLA]:
    """
    Return list of LLA along a Trajectory during the Interval at the provided step.

    Args:
        trajectory (Trajectory): A trajectory.
        interval (Interval): An interval.
        step (Duration): A step. Defaults to 1.0 minutes.

    Returns:
        list[LLA]: The list of LLA.
    """

    return [
        lla_from_state(state)
        for state in trajectory.get_states_at(interval.generate_grid(step))
    ]


def compute_ground_track(
    trajectory: Trajectory,
    interval: Interval,
    step: Duration = Duration.minutes(1.0),
) -> list[LLA]:
    """
    Return list of LLA along a Trajectory ground track (altitude set to 10.0 meters) during the Interval at the provided step.

    Args:
        trajectory (Trajectory): A trajectory.
        interval (Interval): An interval.
        step (Duration): A step. Defaults to 1.0 minutes.

    Returns:
        list[LLA]: The list of LLA.
    """

    ground_llas: list[LLA] = []
    for state in trajectory.get_states_at(interval.generate_grid(step)):
        lla: LLA = lla_from_state(state)

        ground_lla: LLA = LLA(
            lla.get_latitude(),
            lla.get_longitude(),
            Length.meters(10.0),
        )

        ground_llas.append(ground_lla)

    return ground_llas


def convert_state(
    state: trajectory.State,
) -> tuple[str, float, float, float, float, float, float, float, float, float]:
    """
    Convert a State into dataframe-ready values.
    - Instant (str)
    - Modified Julian Date (float)
    - Position X [meters] (float)
    - Position Y [meters] (float)
    - Position Z [meters] (float)
    - Velocity X [meters/second] (float)
    - Velocity Y [meters/second] (float)
    - Velocity Z [meters/second] (float)
    - Latitude [degrees] (float)
    - Longitude [degrees] (float)
    - Altitude [meters] (float)

    Args:
        state (trajectory.State): A state.

    Returns:
        tuple[str, float, float, float, float, float, float, float, float, float]: The dataframe-ready values.
    """

    lla: LLA = lla_from_state(state)

    instant: Instant = state.get_instant()

    return (
        str(instant.to_string()),
        float(instant.get_modified_julian_date(Scale.UTC)),
        *state.get_position().get_coordinates().transpose().tolist(),
        *state.get_velocity().get_coordinates().transpose().tolist(),
        float(lla.get_latitude().in_degrees()),
        float(lla.get_longitude().in_degrees()),
        float(lla.get_altitude().in_meters()),
    )
