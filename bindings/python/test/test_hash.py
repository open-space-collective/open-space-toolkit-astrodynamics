# Apache License 2.0

"""Classes that define __eq__ without __hash__ are unhashable, as with the former pybind11 bindings."""

import pytest

from ostk.physics.coordinate import Frame
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.time import Instant

from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition


@pytest.mark.parametrize(
    "value",
    [
        State(
            Instant.J2000(),
            Position.meters([7.0e6, 0.0, 0.0], Frame.GCRF()),
            Velocity.meters_per_second([0.0, 7.5e3, 0.0], Frame.GCRF()),
        ),
        SatelliteSystem.default(),
        LocalOrbitalFrameFactory.VNC(Frame.GCRF()),
    ],
    ids=["State", "SatelliteSystem", "LocalOrbitalFrameFactory"],
)
def test_value_types_are_unhashable(value):
    assert type(value).__hash__ is None

    with pytest.raises(TypeError):
        hash(value)


def test_coordinate_subset_is_hashable():
    # CoordinateSubset binds its own __hash__, so it can be used as a dictionary key
    assert hash(CartesianPosition.default()) == hash(CartesianPosition.default())
    assert {CartesianPosition.default(): 1}[CartesianPosition.default()] == 1
