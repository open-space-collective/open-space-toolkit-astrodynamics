# Apache License 2.0

from __future__ import annotations

import numpy as np

import pandas as pd

from ostk.mathematics.curve_fitting import Interpolator

from ostk.physics.environment.object import Celestial
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.converters import coerce_to_datetime
from ostk.astrodynamics.converters import coerce_to_instant
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import StateBuilder
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics.trajectory.state.coordinate_subset import AttitudeQuaternion
from ostk.astrodynamics.trajectory.state.coordinate_subset import AngularVelocity
from ostk.astrodynamics.trajectory.orbit.model import Tabulated as TabulatedOrbit
from ostk.astrodynamics.flight import Profile
from ostk.astrodynamics.flight.profile.model import Tabulated as TabulatedProfile


DEFAULT_REFERENCE_FRAME: Frame = Frame.GCRF()
DEFAULT_TIME_COLUMN: str = "Timestamp"
DEFAULT_POSITION_COLUMNS_FORMAT: list[str] = [
    "r_{frame}_x",
    "r_{frame}_y",
    "r_{frame}_z",
]
DEFAULT_VELOCITY_COLUMNS_FORMAT: list[str] = [
    "v_{frame}_x",
    "v_{frame}_y",
    "v_{frame}_z",
]
DEFAULT_ATTITUDE_COLUMNS_FORMAT: list[str] = [
    "q_B_{frame}_x",
    "q_B_{frame}_y",
    "q_B_{frame}_z",
    "q_B_{frame}_s",
]
DEFAULT_ANGULAR_VELOCITY_COLUMNS_FORMAT: list[str] = [
    "w_B_{frame}_in_B_x",
    "w_B_{frame}_in_B_y",
    "w_B_{frame}_in_B_z",
]
DEFAULT_MASS_COLUMN: str = "mass"
DEFAULT_DRAG_COEFFICIENT_COLUMN: str = "drag_coefficient"
DEFAULT_SURFACE_AREA_COLUMN: str = "surface_area"

CARTESIAN_POSITION_SUBSET: CartesianPosition = CartesianPosition.default()
CARTESIAN_VELOCITY_SUBSET: CartesianVelocity = CartesianVelocity.default()
ATTITUDE_QUATERNION_SUBSET: AttitudeQuaternion = AttitudeQuaternion.default()
ANGULAR_VELOCITY_SUBSET: AngularVelocity = AngularVelocity.default()
MASS_SUBSET: CoordinateSubset = CoordinateSubset.mass()
DRAG_COEFFICIENT_SUBSET: CoordinateSubset = CoordinateSubset.drag_coefficient()
SURFACE_AREA_SUBSET: CoordinateSubset = CoordinateSubset.surface_area()

DEFAULT_INTERPOLATION_TYPE: Interpolator.Type = Interpolator.Type.BarycentricRational


def generate_column_names(
    reference_frame: Frame,
    time_column: str | None = None,
    position_columns: list[str] | None = None,
    velocity_columns: list[str] | None = None,
    attitude_columns: list[str] | None = None,
    angular_velocity_columns: list[str] | None = None,
    mass_column: str | None = None,
    drag_coefficient_column: str | None = None,
    surface_area_column: str | None = None,
) -> tuple[str, list[str], list[str], list[str], list[str], str, str, str]:
    """
    Generate column names for a DataFrame containing orbit data.

    Args:
        reference_frame (Frame): Reference frame.
        time_column (str | None): Name of the column containing the time data in [UTC].
        position_columns (list[str] | None): List of column names containing the position data in [m].
        velocity_columns (list[str] | None): List of column names containing the velocity data in [m/s].
        attitude_columns (list[str] | None): List of column names containing the attitude data in [x, y, z, s] form.
        angular_velocity_columns (list[str] | None): List of column names containing the angular velocity data in [rad/s].
        mass_column (str | None): Name of the column containing the mass data in [kg]
        drag_coefficient_column (str | None): Name of the column containing the drag coefficient data.
        surface_area_column (str | None): Name of the column containing the surface area data in [m^2].

    Returns:
        tuple[str, list[str], list[str], list[str], list[str]: Tuple containing the column names.
    """
    reference_frame_name: str = reference_frame.get_name()

    return (
        time_column or DEFAULT_TIME_COLUMN,
        position_columns
        or [
            column.format(frame=reference_frame_name)
            for column in DEFAULT_POSITION_COLUMNS_FORMAT
        ],
        velocity_columns
        or [
            column.format(frame=reference_frame_name)
            for column in DEFAULT_VELOCITY_COLUMNS_FORMAT
        ],
        attitude_columns
        or [
            column.format(frame=reference_frame_name)
            for column in DEFAULT_ATTITUDE_COLUMNS_FORMAT
        ],
        angular_velocity_columns
        or [
            column.format(frame=reference_frame_name)
            for column in DEFAULT_ANGULAR_VELOCITY_COLUMNS_FORMAT
        ],
        mass_column or DEFAULT_MASS_COLUMN,
        drag_coefficient_column or DEFAULT_DRAG_COEFFICIENT_COLUMN,
        surface_area_column or DEFAULT_SURFACE_AREA_COLUMN,
    )


def generate_states_from_dataframe(
    dataframe: pd.DataFrame,
    reference_frame: Frame | None = None,
    time_column: str | None = None,
    position_columns: list[str] | None = None,
    velocity_columns: list[str] | None = None,
    attitude_columns: list[str] | None = None,
    angular_velocity_columns: list[str] | None = None,
    mass_column: str | None = None,
    drag_coefficient_column: str | None = None,
    surface_area_column: str | None = None,
    output_frame: Frame | None = None,
) -> list[State]:
    """
    Generate a list of OSTk States from a Pandas DataFrame.

    Args:
        dataframe (pd.DataFrame): Pandas DataFrame containing the orbit data.
        reference_frame (Frame | None, optional): Reference frame of the states in the dataframe.
        time_column (str | None, optional): Name of the column containing the time data in [UTC].
        position_columns (list[str] | None, optional): List of column names containing the position data in [m].
        velocity_columns (list[str] | None, optional): List of column names containing the velocity data in [m/s].
        attitude_columns (list[str] | None, optional): List of column names containing the attitude data in [x, y, z, s] form.
        angular_velocity_columns (list[str] | None, optional): List of column names containing the angular velocity data in [rad/s].
        mass_column (str | None, optional): Name of the column containing the mass data in [kg].
        drag_coefficient_column (str | None, optional): Name of the column containing the drag coefficient data.
        surface_area_column (str | None, optional): Name of the column containing the surface area data in m^2].
        output_frame (Frame | None, optional): Output frame for the states.

    Returns:
        list[State]: List of OSTk States.
    """

    reference_frame = reference_frame or DEFAULT_REFERENCE_FRAME

    output_frame = output_frame or reference_frame

    (
        time_column,
        position_columns,
        velocity_columns,
        attitude_columns,
        angular_velocity_columns,
        mass_column,
        drag_coefficient_column,
        surface_area_column,
    ) = generate_column_names(
        reference_frame=reference_frame,
        time_column=time_column,
        position_columns=position_columns,
        velocity_columns=velocity_columns,
        attitude_columns=attitude_columns,
        angular_velocity_columns=angular_velocity_columns,
        mass_column=mass_column,
        drag_coefficient_column=drag_coefficient_column,
        surface_area_column=surface_area_column,
    )

    if not set(position_columns).issubset(dataframe.columns):
        raise ValueError("Position columns are not present in the DataFrame.")

    if not set(velocity_columns).issubset(dataframe.columns):
        raise ValueError("Velocity columns are not present in the DataFrame.")

    # check if the dataframe contains a timestamp column
    has_timestamp: bool = time_column in dataframe.columns

    # check if the dataframe contains attitude
    has_attitude: bool = set(attitude_columns).issubset(dataframe.columns)

    # check if the dataframe contains angular velocity
    has_angular_velocity: bool = set(angular_velocity_columns).issubset(dataframe.columns)

    # check if the dataframe contains mass
    has_mass: bool = mass_column in dataframe.columns

    # check if the dataframe contains drag coefficient
    has_drag_coefficient: bool = drag_coefficient_column in dataframe.columns

    # check if the dataframe contains surface area
    has_surface_area: bool = surface_area_column in dataframe.columns

    coordinate_subsets = [
        CARTESIAN_POSITION_SUBSET,
        CARTESIAN_VELOCITY_SUBSET,
    ]

    if has_attitude:
        coordinate_subsets.extend([ATTITUDE_QUATERNION_SUBSET])

    if has_angular_velocity:
        coordinate_subsets.extend([ANGULAR_VELOCITY_SUBSET])

    if has_mass:
        coordinate_subsets.append(MASS_SUBSET)

    if has_drag_coefficient:
        coordinate_subsets.append(DRAG_COEFFICIENT_SUBSET)

    if has_surface_area:
        coordinate_subsets.append(SURFACE_AREA_SUBSET)

    state_builder: StateBuilder = StateBuilder(
        frame=reference_frame,
        coordinate_subsets=coordinate_subsets,
    )

    states: list[State] = []
    for index, row in dataframe.to_dict("index").items():

        coordinates: list[float] = [
            *[row[column] for column in position_columns],
            *[row[column] for column in velocity_columns],
        ]

        if has_attitude:
            coordinates.extend([row[column] for column in attitude_columns])

        if has_angular_velocity:
            coordinates.extend([row[column] for column in angular_velocity_columns])

        if has_mass:
            coordinates.append(row[mass_column])

        if has_drag_coefficient:
            coordinates.append(row[drag_coefficient_column])

        if has_surface_area:
            coordinates.append(row[surface_area_column])

        states.append(
            state_builder.build(
                instant=coerce_to_instant(
                    row[time_column] if has_timestamp else index,
                ),
                coordinates=coordinates,
            ).in_frame(output_frame)
        )

    return states


def generate_dataframe_from_states(
    states: list[State],
    reference_frame: Frame | None = None,
    time_column: str | None = None,
    position_columns: list[str] | None = None,
    velocity_columns: list[str] | None = None,
    attitude_columns: list[str] | None = None,
    angular_velocity_columns: list[str] | None = None,
    mass_column: str | None = None,
    drag_coefficient_column: str | None = None,
    surface_area_column: str | None = None,
    set_time_index: bool = True,
) -> pd.DataFrame:
    """
    Generate a Pandas DataFrame from a list of OSTk States.

    Args:
        states (list[State]): List of OSTk States.
        reference_frame (Frame | None, optional): The desired reference frame.
        time_column (str | None, optional): Name of the column containing the time data in [UTC].
        position_columns (list[str] | None, optional): List of column names containing the position data in [m].
        velocity_columns (list[str] | None, optional): List of column names containing the velocity data in [m/s].
        attitude_columns (list[str] | None, optional): List of column names containing the attitude data in [x, y, z, s] form.
        angular_velocity_columns (list[str] | None, optional): List of column names containing the angular velocity data in [rad/s].
        mass_column (str | None, optional): Name of the column containing the mass data in [kg].
        drag_coefficient_column (str | None, optional): Name of the column containing the drag coefficient data.
        surface_area_column (str | None, optional): Name of the column containing the surface area data in [m^2].
        set_time_index (bool, optional): Whether to set the time column as the index. Defaults to True.

    Returns:
        pd.DataFrame: Pandas DataFrame containing the orbit data.
    """
    has_attitude: bool = states[0].has_subset(ATTITUDE_QUATERNION_SUBSET)
    has_angular_velocity: bool = states[0].has_subset(ANGULAR_VELOCITY_SUBSET)
    has_mass: bool = states[0].has_subset(MASS_SUBSET)
    has_drag_coefficient: bool = states[0].has_subset(DRAG_COEFFICIENT_SUBSET)
    has_surface_area: bool = states[0].has_subset(SURFACE_AREA_SUBSET)

    reference_frame = reference_frame or DEFAULT_REFERENCE_FRAME

    (
        time_column,
        position_columns,
        velocity_columns,
        attitude_columns,
        angular_velocity_columns,
        mass_column,
        drag_coefficient_column,
        surface_area_column,
    ) = generate_column_names(
        reference_frame=reference_frame,
        time_column=time_column,
        position_columns=position_columns,
        velocity_columns=velocity_columns,
        attitude_columns=attitude_columns,
        angular_velocity_columns=angular_velocity_columns,
        mass_column=mass_column,
        drag_coefficient_column=drag_coefficient_column,
        surface_area_column=surface_area_column,
    )

    if len(position_columns) != 3:
        raise ValueError("Position columns must be a list of length 3.")

    if len(velocity_columns) != 3:
        raise ValueError("Velocity columns must be a list of length 3.")

    if len(attitude_columns) != 4:
        raise ValueError("Attitude columns must be a list of length 4.")

    if len(angular_velocity_columns) != 3:
        raise ValueError("Angular velocity columns must be a list of length 3.")

    data: list[dict] = []

    for state in states:
        state = state.in_frame(reference_frame)

        datum: dict = {
            time_column: coerce_to_datetime(state.get_instant()),
            **_get_entry_from_state(state, position_columns, CARTESIAN_POSITION_SUBSET),
            **_get_entry_from_state(state, velocity_columns, CARTESIAN_VELOCITY_SUBSET),
        }

        if has_attitude:
            datum.update(
                _get_entry_from_state(state, attitude_columns, ATTITUDE_QUATERNION_SUBSET)
            )
        if has_angular_velocity:
            datum.update(
                _get_entry_from_state(
                    state, angular_velocity_columns, ANGULAR_VELOCITY_SUBSET
                )
            )
        if has_mass:
            datum.update(**_get_entry_from_state(state, [mass_column], MASS_SUBSET))
        if has_drag_coefficient:
            datum.update(
                **_get_entry_from_state(
                    state, [drag_coefficient_column], DRAG_COEFFICIENT_SUBSET
                )
            )
        if has_surface_area:
            datum.update(
                **_get_entry_from_state(state, [surface_area_column], SURFACE_AREA_SUBSET)
            )

        data.append(datum)

    dataframe: pd.DataFrame = pd.DataFrame(data)
    if set_time_index:
        dataframe.set_index(time_column, inplace=True)

    return dataframe


def generate_orbit_from_dataframe(
    dataframe: pd.DataFrame,
    central_body: Celestial = Earth.default(),
    reference_frame: Frame | None = None,
    time_column: str | None = None,
    position_columns: list[str] | None = None,
    velocity_columns: list[str] | None = None,
    mass_column: str | None = None,
    drag_coefficient_column: str | None = None,
    surface_area_column: str | None = None,
    initial_revolution_number: int = 1,
    interpolation_type: Interpolator.Type | None = None,
    output_frame: Frame | None = None,
) -> Orbit:
    """
    Generate an OSTk Orbit from a Pandas DataFrame.

    Args:
        dataframe (pd.DataFrame): Pandas DataFrame containing the orbit data.
        central_body (Celestial, optional): Celestial object around which the Orbit is defined. Defaults to Earth.
        reference_frame (Frame | None, optional): Reference frame.
        time_column (str | None, optional): Name of the column containing the time data in [UTC].
        position_columns (list[str] | None, optional): List of column names containing the position data in [m].
        velocity_columns (list[str] | None, optional): List of column names containing the velocity data in [m/s].
        mass_column (str | None, optional): Name of the column containing the mass data in [kg].
        drag_coefficient_column (str | None, optional): Name of the column containing the drag coefficient data.
        surface_area_column (str | None, optional): Name of the column containing the surface area data  in [m^2].
        initial_revolution_number (int, optional): Initial revolution number. Defaults to 1.
        interpolation_type (Interpolator.Type | None, optional): Interpolation type.
        output_frame (Frame | None, optional): Output frame for the states.

    Returns:
        Orbit: OSTk Orbit.
    """

    return Orbit(
        model=TabulatedOrbit(
            states=generate_states_from_dataframe(
                dataframe=dataframe,
                time_column=time_column,
                position_columns=position_columns,
                velocity_columns=velocity_columns,
                mass_column=mass_column,
                drag_coefficient_column=drag_coefficient_column,
                surface_area_column=surface_area_column,
                reference_frame=reference_frame,
                output_frame=output_frame,
            ),
            initial_revolution_number=initial_revolution_number,
            interpolation_type=interpolation_type or DEFAULT_INTERPOLATION_TYPE,
        ),
        celestial_object=central_body,
    )


def generate_dataframe_from_orbit(
    orbit: Orbit,
    instants: list[Instant],
    reference_frame: Frame | None = None,
    time_column: str | None = None,
    position_columns: list[str] | None = None,
    velocity_columns: list[str] | None = None,
    mass_column: str | None = None,
    drag_coefficient_column: str | None = None,
    surface_area_column: str | None = None,
    set_time_index: bool = True,
) -> pd.DataFrame:
    """
    Generate a Pandas DataFrame from an OSTk Orbit.

    Args:
        orbit (Orbit): OSTk Orbit.
        instants (list[Instant]): List of instants at which the Orbit is to be evaluated.
        reference_frame (Frame | None, optional): Reference frame.
        time_column (str | None, optional): Name of the column containing the time data in [UTC].
        position_columns (list[str] | None, optional): List of column names containing the position data in [m].
        velocity_columns (list[str] | None, optional): List of column names containing the velocity data in [m/s].
        mass_column (str | None, optional): Name of the column containing the mass data in [kg].
        drag_coefficient_column (str | None, optional): Name of the column containing the drag coefficient data.
        surface_area_column (str | None, optional): Name of the column containing the surface area data  in [m^2].
        set_time_index (bool, optional): Whether to set the time column as the DataFrame index. Defaults to True.

    Returns:
        pd.DataFrame: Pandas DataFrame containing the Orbit data.
    """

    states: list[State] = orbit.get_states_at(instants)

    return generate_dataframe_from_states(
        states,
        reference_frame=reference_frame,
        time_column=time_column,
        position_columns=position_columns,
        velocity_columns=velocity_columns,
        mass_column=mass_column,
        drag_coefficient_column=drag_coefficient_column,
        surface_area_column=surface_area_column,
        set_time_index=set_time_index,
    )


def generate_profile_from_dataframe(
    dataframe: pd.DataFrame,
    reference_frame: Frame | None = None,
    time_column: str | None = None,
    position_columns: list[str] | None = None,
    velocity_columns: list[str] | None = None,
    attitude_columns: list[str] | None = None,
    angular_velocity_columns: list[str] | None = None,
    mass_column: str | None = None,
    drag_coefficient_column: str | None = None,
    surface_area_column: str | None = None,
    output_frame: Frame | None = None,
) -> Profile:
    """
    Generate an OSTk Profile from a Pandas DataFrame.

    Args:
        dataframe (pd.DataFrame): Pandas DataFrame containing the Profile data.
        reference_frame (Frame | None, optional): Reference frame.
        time_column (str | None, optional): Name of the column containing the time data in [UTC].
        position_columns (list[str] | None, optional): List of column names containing the position data in [m].
        velocity_columns (list[str] | None, optional): List of column names containing the velocity data in [m/s].
        attitude_columns (list[str] | None, optional): List of column names containing the attitude data in [x, y, z, s] form.
        angular_velocity_columns (list[str] | None, optional): List of column names containing the angular velocity data in [rad/s].
        mass_column (str | None, optional): Name of the column containing the mass data in [kg].
        drag_coefficient_column (str | None, optional): Name of the column containing the drag coefficient data.
        surface_area_column (str | None, optional): Name of the column containing the surface area data [m^2].
        output_frame (Frame | None, optional): Output frame for the states.

    Returns:
        Profile: OSTk Profile.
    """

    return Profile(
        model=TabulatedProfile(
            states=generate_states_from_dataframe(
                dataframe=dataframe,
                reference_frame=reference_frame,
                time_column=time_column,
                position_columns=position_columns,
                velocity_columns=velocity_columns,
                attitude_columns=attitude_columns,
                angular_velocity_columns=angular_velocity_columns,
                mass_column=mass_column,
                drag_coefficient_column=drag_coefficient_column,
                surface_area_column=surface_area_column,
                output_frame=output_frame,
            ),
        ),
    )


def generate_dataframe_from_profile(
    profile: Profile,
    instants: list[Instant],
    reference_frame: Frame | None = None,
    time_column: str | None = None,
    position_columns: list[str] | None = None,
    velocity_columns: list[str] | None = None,
    attitude_columns: list[str] | None = None,
    angular_velocity_columns: list[str] | None = None,
    mass_column: str | None = None,
    drag_coefficient_column: str | None = None,
    surface_area_column: str | None = None,
    set_time_index: bool = True,
) -> pd.DataFrame:
    """
    Generate a Pandas DataFrame from an OSTk Profile.

    Args:
        profile (Profile): OSTk Profile.
        instants (list[Instant]): List of instants at which the Profile is to be evaluated.
        reference_frame (Frame | None, optional): Reference frame.
        time_column (str | None, optional): Name of the column containing the time data in [UTC].
        position_columns (list[str] | None, optional): List of column names containing the position data in [m].
        velocity_columns (list[str] | None, optional): List of column names containing the velocity data in [m/s].
        attitude_columns (list[str] | None, optional): List of column names containing the attitude data in [x, y, z, s] form.
        angular_velocity_columns (list[str] | None, optional): List of column names containing the angular velocity data in [rad/s].
        mass_column (str | None, optional): Name of the column containing the mass data in [kg].
        drag_coefficient_column (str | None, optional): Name of the column containing the drag coefficient data.
        surface_area_column (str | None, optional): Name of the column containing the surface area data [m^2].
        set_time_index (bool, optional): Whether to set the time column as the DataFrame index. Defaults to True.

    Returns:
        pd.DataFrame: Pandas DataFrame containing the Profile data.
    """

    states: list[State] = profile.get_states_at(instants)

    return generate_dataframe_from_states(
        states,
        reference_frame=reference_frame,
        time_column=time_column,
        position_columns=position_columns,
        velocity_columns=velocity_columns,
        attitude_columns=attitude_columns,
        angular_velocity_columns=angular_velocity_columns,
        mass_column=mass_column,
        drag_coefficient_column=drag_coefficient_column,
        surface_area_column=surface_area_column,
        set_time_index=set_time_index,
    )


def _get_entry_from_state(
    state: State, columns: list[str], subset: CoordinateSubset
) -> dict:
    coordinates: np.ndarray = state.extract_coordinate(subset)
    return {
        column.format(frame=state.get_frame().get_name()): coordinates[idx]
        for idx, column in enumerate(columns)
    }
