# Apache License 2.0

"""Eigen types crossing the binding boundary (Utility/EigenCasting.hpp).

Inputs: NumPy arrays of any memory order, dtype or stride, and also Python lists and tuples (as with the former
pybind11 bindings), are copied into the Eigen type. Outputs are independent, writeable float64 NumPy arrays.
"""

import numpy as np

import pytest

from ostk.mathematics.geometry.d3.object import Composite
from ostk.mathematics.geometry.d3.object import Cuboid
from ostk.mathematics.geometry.d3.object import Point

from ostk.physics.coordinate import Frame
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.time import Instant
from ostk.physics.unit import Mass

from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.guidance_law import QLaw
from ostk.astrodynamics.trajectory import LocalOrbitalFrameDirection
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity

COORDINATES = [7.0e6, 1.0, 2.0, 3.0, 7.5e3, 4.0]


def make_state(coordinates) -> State:
    return State(
        Instant.J2000(),
        coordinates,
        Frame.GCRF(),
        [CartesianPosition.default(), CartesianVelocity.default()],
    )


def make_direction(value) -> LocalOrbitalFrameDirection:
    return LocalOrbitalFrameDirection(value, LocalOrbitalFrameFactory.VNC(Frame.GCRF()))


def make_satellite_system(inertia_tensor) -> SatelliteSystem:
    return SatelliteSystem(
        Mass.kilograms(100.0),
        Composite(Cuboid.cube(Point(0.0, 0.0, 0.0), 1.0)),
        inertia_tensor,
        1.0,
        2.1,
    )


class TestVectorXd:
    @pytest.mark.parametrize(
        "coordinates",
        [
            COORDINATES,
            tuple(COORDINATES),
            np.array(COORDINATES),
            np.array(COORDINATES, dtype=np.float32),
            np.array([int(value) for value in COORDINATES]),
            np.array(COORDINATES).reshape(6, 1),
            np.array(COORDINATES * 2)[::2],
            np.asfortranarray(np.array(COORDINATES)),
        ],
        ids=[
            "list",
            "tuple",
            "float64",
            "float32",
            "int",
            "column",
            "strided",
            "fortran",
        ],
    )
    def test_inputs(self, coordinates):
        state = make_state(coordinates)

        assert state.get_coordinates() == pytest.approx(
            np.asarray(coordinates, dtype=np.float64).reshape(-1), rel=1e-6
        )

    def test_input_is_copied(self):
        coordinates = np.array(COORDINATES)

        state = make_state(coordinates)

        coordinates[0] = 0.0

        assert state.get_coordinates()[0] == 7.0e6

    def test_read_only_input(self):
        coordinates = np.array(COORDINATES)
        coordinates.flags.writeable = False

        assert make_state(coordinates).get_coordinates() == pytest.approx(COORDINATES)

    def test_output_is_an_independent_copy(self):
        state = make_state(COORDINATES)

        coordinates = state.get_coordinates()

        assert isinstance(coordinates, np.ndarray)
        assert coordinates.dtype == np.float64
        assert coordinates.shape == (6,)
        assert coordinates.flags.writeable

        coordinates[0] = 0.0

        assert state.get_coordinates()[0] == 7.0e6

    def test_output_outlives_its_source(self):
        coordinates = make_state(COORDINATES).get_coordinates()

        assert coordinates == pytest.approx(COORDINATES)

    @pytest.mark.parametrize(
        "coordinates",
        [
            ["a"] * 6,
            "abcdef",
            [[1.0, 2.0], [3.0]],
            np.zeros((2, 3)),
            np.zeros(6, dtype=complex),
        ],
        ids=["str-list", "str", "ragged", "matrix", "complex"],
    )
    def test_rejected_inputs(self, coordinates):
        with pytest.raises(TypeError):
            make_state(coordinates)


class TestVector3d:
    @pytest.mark.parametrize(
        "value",
        [
            [1.0, 0.0, 0.0],
            (1.0, 0.0, 0.0),
            np.array([1, 0, 0]),
            np.array([[1.0], [0.0], [0.0]]),
        ],
        ids=["list", "tuple", "int", "column"],
    )
    def test_inputs(self, value):
        direction = make_direction(value)

        result = direction.get_value()

        assert isinstance(result, np.ndarray)
        assert result.dtype == np.float64
        assert result.shape == (3,)
        assert result == pytest.approx([1.0, 0.0, 0.0])

    @pytest.mark.parametrize(
        "value",
        [[1.0, 0.0], [1.0, 0.0, 0.0, 0.0], np.array([[1.0, 0.0, 0.0]])],
        ids=["short", "long", "row"],
    )
    def test_rejected_inputs(self, value):
        with pytest.raises(TypeError):
            make_direction(value)


class TestMatrix3d:
    def test_round_trip_keeps_row_column_layout(self):
        inertia_tensor = np.array([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, 8.0, 10.0]])

        for value in (
            inertia_tensor,
            inertia_tensor.tolist(),
            np.asfortranarray(inertia_tensor),
        ):
            result = make_satellite_system(value).get_inertia_tensor()

            assert isinstance(result, np.ndarray)
            assert result.shape == (3, 3)
            assert result.dtype == np.float64
            assert np.array_equal(result, inertia_tensor)

    def test_rejected_shape(self):
        with pytest.raises(TypeError):
            make_satellite_system(np.eye(2))


class TestFixedSizeVectorsAndMatrices:
    def test_vector5d_output(self):
        parameters = QLaw.Parameters(
            element_weights={
                COE.Element.SemiMajorAxis: (1.0, 100.0),
                COE.Element.Eccentricity: (1.0, 1e-3),
            },
        )

        control_weights = parameters.get_control_weights()

        assert isinstance(control_weights, np.ndarray)
        assert control_weights.shape == (5,)
        assert control_weights == pytest.approx([1.0, 1.0, 0.0, 0.0, 0.0])

    def test_vector6d_input_and_matrix53d_output(self):
        coe_vector = [7.0e6, 0.01, 0.5, 0.1, 0.2, 0.3]

        gravitational_parameter = EarthGravitationalModel.EGM2008.gravitational_parameter

        from_list = QLaw.compute_d_oe_d_f(coe_vector, gravitational_parameter)
        from_array = QLaw.compute_d_oe_d_f(np.array(coe_vector), gravitational_parameter)

        assert isinstance(from_list, np.ndarray)
        assert from_list.shape == (5, 3)
        assert np.array_equal(from_list, from_array)
