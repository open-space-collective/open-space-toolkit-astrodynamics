# Apache License 2.0


# Python-only State functionality
import numpy as np

from ostk.physics.coordinate import Frame
from ostk.physics.time import Instant

from ostk.astrodynamics.trajectory import State, StateBuilder


def custom_class_generator(frame: Frame, coordinates_subsets: list) -> type:
    """
        Custom class type generator for States. This is meta-programming syntactic sugar on top of the StateBuilder class.
    """

    class StateTemplateType(State):

        state_builder: StateBuilder = StateBuilder(frame, coordinates_subsets)

        def __init__(self, instant: Instant, coordinates: np.ndarray):
            super().__init__(
                StateTemplateType.state_builder.build_state(instant, coordinates)
            )
    
    return StateTemplateType

State.template = custom_class_generator
