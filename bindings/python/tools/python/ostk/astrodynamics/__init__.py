# Apache License 2.0

from ostk.physics import *

from .OpenSpaceToolkitAstrodynamicsPy import *

from .pytrajectory.pystate import State as PyState

trajectory.State = (
    PyState  # Override the pure c++ State class with the modified Python one
)
