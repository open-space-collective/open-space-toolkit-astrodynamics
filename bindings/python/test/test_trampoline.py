# Apache License 2.0

"""Python subclasses of the trampolined C++ classes, called back from C++.

These exercise the NB_TRAMPOLINE / NB_OVERRIDE_* trampolines: C++ (a propagator, a thruster, a logical
condition) calls the Python overrides, and Python objects held by C++ shared pointers stay alive after the
last Python reference is dropped.
"""

import gc

import numpy as np

import pytest

from ostk.mathematics.geometry.d3.object import Composite
from ostk.mathematics.geometry.d3.object import Cuboid
from ostk.mathematics.geometry.d3.object import Point

from ostk.physics.coordinate import Frame
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.unit import Mass

from ostk.astrodynamics import Dynamics
from ostk.astrodynamics import EventCondition
from ostk.astrodynamics import GuidanceLaw
from ostk.physics.environment.object.celestial import Earth

from ostk.astrodynamics.dynamics import CentralBodyGravity
from ostk.astrodynamics.dynamics import PositionDerivative
from ostk.astrodynamics.dynamics import Thruster
from ostk.astrodynamics.event_condition import BooleanCondition
from ostk.astrodynamics.event_condition import LogicalCondition
from ostk.astrodynamics.flight.system import PropulsionSystem
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.trajectory import Propagator
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import NumericalSolver
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity


class ConstantAcceleration(Dynamics):
    def __init__(self, acceleration: list[float]):
        super().__init__("Constant Acceleration")
        self.acceleration = np.array(acceleration)
        self.call_count = 0

    def get_read_coordinate_subsets(self) -> list:
        return [CartesianPosition.default()]

    def get_write_coordinate_subsets(self) -> list:
        return [CartesianVelocity.default()]

    def compute_contribution(self, instant, state_vector, frame) -> np.ndarray:
        self.call_count += 1
        return self.acceleration


class AlongZ(GuidanceLaw):
    def __init__(self):
        super().__init__("Along Z")
        self.call_count = 0

    def calculate_thrust_acceleration_at(
        self,
        instant,
        position_coordinates,
        velocity_coordinates,
        thrust_acceleration,
        output_frame,
    ) -> np.ndarray:
        self.call_count += 1
        return np.array([0.0, 0.0, thrust_acceleration])


class AboveAltitude(EventCondition):
    def __init__(self, altitude: float):
        super().__init__(
            "Above altitude",
            lambda state: state.get_position().get_coordinates()[2],
            EventCondition.Target(altitude),
        )
        self.call_count = 0

    def is_satisfied(self, current_state, previous_state) -> bool:
        self.call_count += 1
        return current_state.get_position().get_coordinates()[2] > self.get_target().value


def make_state_at_z(z: float) -> State:
    return State(
        Instant.J2000(),
        [7.0e6, 0.0, z, 0.0, 7.5e3, 0.0],
        Frame.GCRF(),
        [CartesianPosition.default(), CartesianVelocity.default()],
    )


@pytest.fixture
def state() -> State:
    return State(
        Instant.J2000(),
        [7.0e6, 0.0, 0.0, 0.0, 7.5e3, 0.0],
        Frame.GCRF(),
        [CartesianPosition.default(), CartesianVelocity.default()],
    )


@pytest.fixture
def numerical_solver() -> NumericalSolver:
    return NumericalSolver(
        NumericalSolver.LogType.NoLog,
        NumericalSolver.StepperType.RungeKuttaFehlberg78,
        5.0,
        1.0e-12,
        1.0e-12,
    )


def base_dynamics() -> list[Dynamics]:
    return [PositionDerivative(), CentralBodyGravity(Earth.WGS84(0, 0))]


def make_propagator(numerical_solver: NumericalSolver) -> Propagator:
    # The Python dynamics is only referenced by the C++ propagator once this returns
    return Propagator(
        numerical_solver,
        base_dynamics() + [ConstantAcceleration([0.0, 0.0, 2.0])],
    )


def reference_state_at(
    state: State, numerical_solver: NumericalSolver, duration: Duration
) -> State:
    return Propagator(numerical_solver, base_dynamics()).calculate_state_at(
        state, state.get_instant() + duration
    )


class TestDynamicsTrampoline:
    def test_propagator_calls_python_dynamics(
        self, state: State, numerical_solver: NumericalSolver
    ):
        dynamics = ConstantAcceleration([0.0, 0.0, 2.0])

        propagator = Propagator(numerical_solver, base_dynamics() + [dynamics])

        end_state: State = propagator.calculate_state_at(
            state, state.get_instant() + Duration.seconds(10.0)
        )
        reference_state: State = reference_state_at(
            state, numerical_solver, Duration.seconds(10.0)
        )

        # A constant 2 m/s^2 along z adds 100 m and 20 m/s over 10 s
        assert dynamics.call_count > 0
        assert end_state.get_position().get_coordinates()[
            2
        ] - reference_state.get_position().get_coordinates()[2] == pytest.approx(
            100.0, abs=1e-2
        )
        assert end_state.get_velocity().get_coordinates()[
            2
        ] - reference_state.get_velocity().get_coordinates()[2] == pytest.approx(
            20.0, abs=1e-2
        )

    def test_python_dynamics_outlives_its_python_references(
        self, state: State, numerical_solver: NumericalSolver
    ):
        propagator = make_propagator(numerical_solver)

        gc.collect()

        dynamics = propagator.get_dynamics()[2]

        assert isinstance(dynamics, ConstantAcceleration)
        assert dynamics.get_name() == "Constant Acceleration"

        end_state: State = propagator.calculate_state_at(
            state, state.get_instant() + Duration.seconds(10.0)
        )
        reference_state: State = reference_state_at(
            state, numerical_solver, Duration.seconds(10.0)
        )

        assert dynamics.call_count > 0
        assert end_state.get_position().get_coordinates()[
            2
        ] - reference_state.get_position().get_coordinates()[2] == pytest.approx(
            100.0, abs=1e-2
        )

    def test_missing_override_raises(self):
        class Incomplete(Dynamics):
            pass

        with pytest.raises(RuntimeError):
            Incomplete("Incomplete").get_read_coordinate_subsets()


class TestGuidanceLawTrampoline:
    def test_thruster_calls_python_guidance_law(self):
        satellite_system = SatelliteSystem(
            Mass.kilograms(90.0),
            Composite(Cuboid.cube(Point(0.0, 0.0, 0.0), 1.0)),
            np.eye(3),
            1.0,
            2.1,
            PropulsionSystem(1.0, 1500.0),
        )

        guidance_law = AlongZ()

        thruster = Thruster(satellite_system, guidance_law)

        contribution = thruster.compute_contribution(
            Instant.J2000(),
            [7.0e6, 0.0, 0.0, 0.0, 7.5e3, 0.0, 100.0],
            Frame.GCRF(),
        )

        assert guidance_law.call_count == 1
        assert contribution[0:3] == pytest.approx([0.0, 0.0, 1.0 / 100.0])


class TestEventConditionTrampoline:
    def test_logical_condition_calls_python_event_condition(self):
        condition = AboveAltitude(10.0)

        logical_condition = LogicalCondition("Any", LogicalCondition.Type.Or, [condition])

        assert logical_condition.is_satisfied(make_state_at_z(20.0), make_state_at_z(0.0))
        assert not logical_condition.is_satisfied(
            make_state_at_z(5.0), make_state_at_z(0.0)
        )
        assert condition.call_count == 2


class TestBooleanResults:
    def test_callback_may_return_numpy_bool(self):
        # pybind11 converted the result of a Python callable with its truth value, so that numpy.bool_ works
        evaluator = lambda state: state.get_position().get_coordinates()[2] > 10.0

        assert isinstance(evaluator(make_state_at_z(20.0)), np.bool_)

        condition = BooleanCondition(
            "Above 10 m", BooleanCondition.Criterion.StrictlyPositive, evaluator, False
        )

        assert condition.is_satisfied(make_state_at_z(20.0), make_state_at_z(0.0))
        assert not condition.is_satisfied(make_state_at_z(5.0), make_state_at_z(0.0))

    def test_override_may_return_numpy_bool(self):
        condition = AboveAltitude(10.0)

        assert isinstance(
            condition.is_satisfied(make_state_at_z(20.0), make_state_at_z(0.0)), np.bool_
        )

        logical_condition = LogicalCondition(
            "And", LogicalCondition.Type.And, [condition]
        )

        assert logical_condition.is_satisfied(make_state_at_z(20.0), make_state_at_z(0.0))
