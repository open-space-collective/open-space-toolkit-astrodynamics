# Apache License 2.0


# Python-only State functionality
import numpy as np

from ostk.physics.coordinate import Frame
from ostk.physics.time import Instant

from ostk.astrodynamics.trajectory import State, StateBuilder


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


State.template = custom_class_generator
