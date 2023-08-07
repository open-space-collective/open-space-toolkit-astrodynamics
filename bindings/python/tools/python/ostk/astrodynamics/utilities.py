# Apache License 2.0

from .OpenSpaceToolkitAstrodynamicsPy import *

from ostk.physics import Environment
from ostk.physics.time import Scale
from ostk.physics.time import Instant
from ostk.physics.time import Duration
from ostk.physics.time import Interval
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.coordinate.spherical import AER
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame
from ostk.physics.environment.objects.celestial_bodies import Earth
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel


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
