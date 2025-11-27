# Apache License 2.0

from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime
from datetime import timezone

import numpy as np

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

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.converters import coerce_to_datetime
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import StateBuilder
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory


POSITION_VELOCITY_STATE_BUILDER: StateBuilder = StateBuilder(
    frame=Frame.GCRF(),
    coordinate_subsets=[
        CartesianPosition.default(),
        CartesianVelocity.default(),
    ],
)

DEFAULT_INERTIAL_FRAME: Frame = Frame.GCRF()


@dataclass
class Residual:
    timestamp: datetime
    frame: Frame
    dr: float
    dr_x: float
    dr_y: float
    dr_z: float
    dv: float
    dv_x: float
    dv_y: float
    dv_z: float


def compute_residuals(
    candidate_states: list[State],
    reference_states: list[State],
    local_orbital_frame_factory_or_frame: (
        LocalOrbitalFrameFactory | Frame
    ) = DEFAULT_INERTIAL_FRAME,
) -> list[Residual]:
    """
    Compute position and velocity residuals by comparing candidate states against reference states.

    Args:
        candidate_states (list[State]): Candidate list of States.
        reference_states (list[State]): List of reference States to compare against.
        local_orbital_frame_factory_or_frame (LocalOrbitalFrameFactory | Frame, optional): The local orbital frame factory to use. Defaults to Frame.GCRF().

    Returns:
        list[Residual]: List of Residuals.
    """
    residuals: list[Residual] = []

    for candidate_state, reference_state in zip(candidate_states, reference_states):
        frame: Frame
        if isinstance(local_orbital_frame_factory_or_frame, LocalOrbitalFrameFactory):
            frame = local_orbital_frame_factory_or_frame.generate_frame(candidate_state)
        else:
            frame = local_orbital_frame_factory_or_frame

        relative_state: State = POSITION_VELOCITY_STATE_BUILDER.reduce(
            candidate_state
        ).in_frame(frame) - POSITION_VELOCITY_STATE_BUILDER.reduce(
            reference_state
        ).in_frame(
            frame
        )

        dr: np.ndarray = relative_state.get_position().get_coordinates()
        dv: np.ndarray = relative_state.get_velocity().get_coordinates()

        residuals.append(
            Residual(
                timestamp=coerce_to_datetime(reference_state.get_instant()),
                frame=frame,
                dr=float(np.linalg.norm(dr)),
                dr_x=float(dr[0]),
                dr_y=float(dr[1]),
                dr_z=float(dr[2]),
                dv=float(np.linalg.norm(dv)),
                dv_x=float(dv[0]),
                dv_y=float(dv[1]),
                dv_z=float(dv[2]),
            )
        )

    return residuals


def compute_residuals_for_orbit(
    candidate_orbit: Orbit,
    reference_states: list[State],
    local_orbital_frame_factory_or_frame: (
        LocalOrbitalFrameFactory | Frame
    ) = DEFAULT_INERTIAL_FRAME,
) -> list[Residual]:
    """
    Compute position and velocity residuals for an orbit against a list of reference states.

    Args:
        candidate_orbit (Orbit): Candidate Orbit used to generate states at the instants of the reference states.
        reference_states (list[State]): List of reference States to compare against.
        local_orbital_frame_factory_or_frame (LocalOrbitalFrameFactory | Frame, optional): The local orbital frame factory to use. Defaults to Frame.GCRF().

    Returns:
        list[Residual]: List of Residuals.
    """
    instants: list[Instant] = [state.get_instant() for state in reference_states]
    candidate_states: list[State] = candidate_orbit.get_states_at(instants)

    return compute_residuals(
        candidate_states=candidate_states,
        reference_states=reference_states,
        local_orbital_frame_factory_or_frame=local_orbital_frame_factory_or_frame,
    )


def compute_residuals_for_orbits(
    candidate_orbit: Orbit,
    reference_orbit: Orbit,
    instants: list[Instant],
    local_orbital_frame_factory_or_frame: (
        LocalOrbitalFrameFactory | Frame
    ) = DEFAULT_INERTIAL_FRAME,
) -> list[Residual]:
    """
    Compare two orbits at the provided instants and compute position and velocity residuals.

    Args:
        candidate_orbit (Orbit): Candidate Orbit.
        reference_orbit (Orbit): Reference Orbit.
        instants (list[Instant]): List of instants to generate states at.
        local_orbital_frame_factory_or_frame (LocalOrbitalFrameFactory | Frame, optional): The local orbital frame factory to use. Defaults to Frame.GCRF().

    Returns:
        list[Residual]: List of Residuals.
    """
    candidate_states: list[State] = candidate_orbit.get_states_at(instants)
    reference_states: list[State] = reference_orbit.get_states_at(instants)

    return compute_residuals(
        candidate_states=candidate_states,
        reference_states=reference_states,
        local_orbital_frame_factory_or_frame=local_orbital_frame_factory_or_frame,
    )


def lla_from_state(state: State) -> LLA:
    """
    Return latitude (degrees), longitude (degrees), altitude (meters) float list from a state.

    Args:
        state (State): A state.

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
    state: State,
    from_position: Position,
    environment: Environment,
) -> tuple[datetime, float, float, float, float, float, float]:
    """
    Return [datetime, latitude, longitude, altitude, azimuth, elevation, range] from State and observer Position.

    Args:
        state (State): A state.
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
    state: State,
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
        state (State): A state.

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
