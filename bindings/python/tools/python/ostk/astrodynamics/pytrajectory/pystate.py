# Apache License 2.0


# Python-only State functionality
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


@staticmethod
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


@staticmethod
def from_dict(data: dict) -> State:
    """
    Create a State from a dictionary.
    The dictionary must contain the following:
    - 'timestamp': The timestamp of the state.
    - 'rx'/'x_eci'/'x_ecef': The x-coordinate of the position.
    - 'ry'/'y_eci'/'y_ecef': The y-coordinate of the position.
    - 'rz'/'z_eci'/'z_ecef': The z-coordinate of the position.
    - 'vx'/'vx_eci'/'vx_ecef': The x-coordinate of the velocity.
    - 'vy'/'vy_eci'/'vy_ecef': The y-coordinate of the velocity.
    - 'vz'/'vz_eci'/'vz_ecef': The z-coordinate of the velocity.
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

    frame: Frame
    coordinates: np.ndarray

    coordinate_subsets: list[CoordinateSubset] = [
        CartesianPosition.default(),
        CartesianVelocity.default(),
    ]

    if all(column in data for column in eci_columns):
        frame = Frame.GCRF()
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

    if "drag_coefficient" in data or "cd" in data:
        coordinate_subsets.append(CoordinateSubset.drag_coefficient())
        coordinates = np.append(
            coordinates,
            data.get("drag_coefficient", data.get("cd")),
        )

    if "cross_sectional_area" in data or "surface_area" in data:
        coordinate_subsets.append(CoordinateSubset.surface_area())
        coordinates = np.append(
            coordinates,
            data.get("cross_sectional_area", data.get("surface_area")),
        )

    if "mass" in data:
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


State.from_dict = from_dict
State.template = custom_class_generator
