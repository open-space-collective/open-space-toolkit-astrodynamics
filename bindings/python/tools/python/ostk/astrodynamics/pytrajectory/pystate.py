# Apache License 2.0

# Python-only State functionality
import re

import numpy as np

from ostk.physics.coordinate import Frame
from ostk.physics.time import Instant

from ostk.astrodynamics.trajectory import State, StateBuilder
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.converters import coerce_to_instant
from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    CartesianPosition,
    CartesianVelocity,
    AttitudeQuaternion,
    AngularVelocity,
)

CANONICAL_FORMAT: str = r"(r|v)_(.*?)_(x|y|z)"


def custom_class_generator(frame: Frame, coordinate_subsets: list) -> type:
    """
    Emit a custom class type for States. This is meta-programming syntactic sugar on top of the StateBuilder class.

    StateType = State.template(frame, coordinate_subsets)
    state = StateType(instant, coordinates)

    is equivalent to

    state_builder = StateBuilder(frame, coordinate_subsets)
    state = state_builder.build(instant, coordinates)
    """

    class StateTemplateType(State):
        state_builder: StateBuilder = StateBuilder(frame, coordinate_subsets)

        def __init__(self, instant: Instant, coordinates: np.ndarray):
            super().__init__(StateTemplateType.state_builder.build(instant, coordinates))

    return StateTemplateType


def from_dict(data: dict) -> State:
    """
    Create a State from a dictionary.

    Note: Implicit assumption that ECEF = ITRF, and ECI = GCRF.

    The dictionary must contain the following:
    - 'timestamp': The timestamp of the state.
    - 'r_ITRF_x'/'rx'/'rx_eci'/'rx_ecef': The x-coordinate of the position.
    - 'r_ITRF_y'/'ry'/'ry_eci'/'ry_ecef': The y-coordinate of the position.
    - 'r_ITRF_z'/'rz'/'rz_eci'/'rz_ecef': The z-coordinate of the position.
    - 'v_ITRF_x'/'vx'/'vx_eci'/'vx_ecef': The x-coordinate of the velocity.
    - 'v_ITRF_y'/'vy'/'vy_eci'/'vy_ecef': The y-coordinate of the velocity.
    - 'v_ITRF_z'/'vz'/'vz_eci'/'vz_ecef': The z-coordinate of the velocity.
    - 'frame': The frame of the state. Required if 'rx', 'ry', 'rz', 'vx', 'vy', 'vz' are provided.
    - 'q_B_ECI_x': The x-coordinate of the quaternion. Optional.
    - 'q_B_ECI_y': The y-coordinate of the quaternion. Optional.
    - 'q_B_ECI_z': The z-coordinate of the quaternion. Optional.
    - 'q_B_ECI_s': The s-coordinate of the quaternion. Optional.
    - 'w_B_ECI_in_B_x': The x-coordinate of the angular velocity. Optional.
    - 'w_B_ECI_in_B_y': The y-coordinate of the angular velocity. Optional.
    - 'w_B_ECI_in_B_z': The z-coordinate of the angular velocity. Optional.
    - 'drag_coefficient'/'cd': The drag coefficient. Optional.
    - 'cross_sectional_area'/'surface_area': The cross-sectional area. Optional.
    - 'mass': The mass. Optional.

    Args:
        data (dict): The dictionary.

    Returns:
        State: The State.
    """

    instant: Instant = coerce_to_instant(data["timestamp"])

    eci_columns: list[str] = [
        "rx_eci",
        "ry_eci",
        "rz_eci",
        "vx_eci",
        "vy_eci",
        "vz_eci",
    ]
    ecef_columns: list[str] = [
        "rx_ecef",
        "ry_ecef",
        "rz_ecef",
        "vx_ecef",
        "vy_ecef",
        "vz_ecef",
    ]
    generic_columns: list[str] = [
        "rx",
        "ry",
        "rz",
        "vx",
        "vy",
        "vz",
    ]

    # Replace non-standard position keys with canonical representation
    if all(key in data.keys() for key in ("x_eci", "y_eci", "z_eci")):
        data["rx_eci"] = data["x_eci"]
        data["ry_eci"] = data["y_eci"]
        data["rz_eci"] = data["z_eci"]

    if all(key in data.keys() for key in ("x_ecef", "y_ecef", "z_ecef")):
        data["rx_ecef"] = data["x_ecef"]
        data["ry_ecef"] = data["y_ecef"]
        data["rz_ecef"] = data["z_ecef"]

    frame: Frame
    coordinates: np.ndarray

    coordinate_subsets: list[CoordinateSubset] = [
        CartesianPosition.default(),
        CartesianVelocity.default(),
    ]

    match_groups: list[re.Match] = [
        re.match(CANONICAL_FORMAT, column) for column in data.keys()
    ]

    if len(matches := [match for match in match_groups if match is not None]) == 6:
        frame_name: str = matches[0].group(2)
        try:
            frame: Frame = Frame.with_name(frame_name) or getattr(Frame, frame_name)()
        except Exception:
            raise ValueError(f"No frame exists with name [{frame_name}].")

        coordinates = np.array([data[match.group(0)] for match in matches])

    elif all(column in data for column in eci_columns):
        frame: Frame = Frame.GCRF()
        coordinates = np.array(
            [
                data["rx_eci"],
                data["ry_eci"],
                data["rz_eci"],
                data["vx_eci"],
                data["vy_eci"],
                data["vz_eci"],
            ]
        )

    elif all(column in data for column in ecef_columns):
        frame = Frame.ITRF()
        coordinates = np.array(
            [
                data["rx_ecef"],
                data["ry_ecef"],
                data["rz_ecef"],
                data["vx_ecef"],
                data["vy_ecef"],
                data["vz_ecef"],
            ]
        )

    elif all(column in data for column in generic_columns):
        if "frame" not in data:
            raise ValueError("Frame must be provided for generic columns.")

        if isinstance(data["frame"], str):
            if Frame.exists(data["frame"]):
                frame = Frame.with_name(data["frame"])
            else:
                raise ValueError(f"No frame exists with name [{data['frame']}].")
        elif isinstance(data["frame"], Frame):
            frame = data["frame"]
        else:
            raise ValueError(f"Invalid frame data [{data['frame']}].")

        coordinates = np.array(
            [
                data["rx"],
                data["ry"],
                data["rz"],
                data["vx"],
                data["vy"],
                data["vz"],
            ]
        )
    else:
        raise ValueError("Invalid state data.")

    if all(
        column in data for column in ["q_B_ECI_x", "q_B_ECI_y", "q_B_ECI_z", "q_B_ECI_s"]
    ):
        coordinate_subsets.append(AttitudeQuaternion.default())
        coordinates = np.append(
            coordinates,
            [
                data["q_B_ECI_x"],
                data["q_B_ECI_y"],
                data["q_B_ECI_z"],
                data["q_B_ECI_s"],
            ],
        )

    if all(
        column in data
        for column in ["w_B_ECI_in_B_x", "w_B_ECI_in_B_y", "w_B_ECI_in_B_z"]
    ):
        coordinate_subsets.append(AngularVelocity.default())
        coordinates = np.append(
            coordinates,
            [
                data["w_B_ECI_in_B_x"],
                data["w_B_ECI_in_B_y"],
                data["w_B_ECI_in_B_z"],
            ],
        )

    if (data.get("drag_coefficient") is not None) or (data.get("cd") is not None):
        coordinate_subsets.append(CoordinateSubset.drag_coefficient())
        coordinates = np.append(
            coordinates,
            data.get("drag_coefficient", data.get("cd")),
        )

    if (data.get("cross_sectional_area") is not None) or (
        data.get("surface_area") is not None
    ):
        coordinate_subsets.append(CoordinateSubset.surface_area())
        coordinates = np.append(
            coordinates,
            data.get("cross_sectional_area", data.get("surface_area")),
        )

    if data.get("mass") is not None:
        coordinate_subsets.append(CoordinateSubset.mass())
        coordinates = np.append(
            coordinates,
            data["mass"],
        )

    return State(
        instant=instant,
        coordinates=coordinates,
        frame=frame,
        coordinate_subsets=coordinate_subsets,
    )


State.from_dict = staticmethod(from_dict)
State.template = staticmethod(custom_class_generator)
