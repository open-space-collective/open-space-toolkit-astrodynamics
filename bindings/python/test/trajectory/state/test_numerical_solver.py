# Apache License 2.0

import pytest

import numpy as np
import math

from ostk.physics.time import Instant, Duration
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import (
    NumericalSolver,
    CoordinateBroker,
    CoordinateSubset,
)
from ostk.astrodynamics.event_condition import RealCondition


def oscillator(x, dxdt, _):
    dxdt[0] = x[1]
    dxdt[1] = -x[0]
    return dxdt


def get_state_vec(time: float) -> np.ndarray:
    return np.array([math.sin(time), math.cos(time)])


@pytest.fixture
def coordinate_subsets() -> list[CoordinateSubset]:
    return [CoordinateSubset("Subset", 2)]


@pytest.fixture
def coordinate_broker(
    coordinate_subsets: list[CoordinateSubset],
) -> CoordinateBroker:
    return CoordinateBroker(coordinate_subsets)


@pytest.fixture
def start_instant() -> Instant:
    return Instant.J2000()


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def initial_state(
    start_instant: Instant,
    frame: Frame,
    coordinate_broker: CoordinateBroker,
) -> State:
    return State(start_instant, get_state_vec(0.0), frame, coordinate_broker)


@pytest.fixture
def custom_condition() -> RealCondition:
    return RealCondition(
        "Custom",
        RealCondition.Criterion.StrictlyPositive,
        lambda state: (state.get_instant() - Instant.J2000()).in_seconds(),
        5.0,
    )


@pytest.fixture
def log_type() -> NumericalSolver.LogType:
    return NumericalSolver.LogType.NoLog


@pytest.fixture
def variable_size_stepper_type() -> NumericalSolver.StepperType:
    return NumericalSolver.StepperType.RungeKuttaCashKarp54


@pytest.fixture
def fixed_size_stepper_type() -> NumericalSolver.StepperType:
    return NumericalSolver.StepperType.RungeKutta4


@pytest.fixture
def initial_time_step() -> float:
    return 5.0


@pytest.fixture
def relative_tolerance() -> float:
    return 1.0e-15


@pytest.fixture
def absolute_tolerance() -> float:
    return 1.0e-15


@pytest.fixture
def state_logger() -> callable:
    def log_state(state: State) -> None:
        print(state.get_coordinates())

    return log_state


@pytest.fixture
def numerical_solver(
    log_type: NumericalSolver.LogType,
    variable_size_stepper_type: NumericalSolver.StepperType,
    initial_time_step: float,
    relative_tolerance: float,
    absolute_tolerance: float,
) -> NumericalSolver:
    return NumericalSolver(
        log_type=log_type,
        stepper_type=variable_size_stepper_type,
        time_step=initial_time_step,
        relative_tolerance=relative_tolerance,
        absolute_tolerance=absolute_tolerance,
    )


@pytest.fixture
def numerical_solver_conditional() -> NumericalSolver:
    return NumericalSolver.default_conditional()


class TestNumericalSolver:
    def test_constructors(self, numerical_solver: NumericalSolver):
        assert numerical_solver is not None
        assert isinstance(numerical_solver, NumericalSolver)
        assert numerical_solver.is_defined()

    def test_comparators(self, numerical_solver: NumericalSolver):
        assert numerical_solver == numerical_solver
        assert (numerical_solver != numerical_solver) is False

    def test_get_types(
        self,
        log_type: NumericalSolver.LogType,
        variable_size_stepper_type: NumericalSolver.StepperType,
        initial_time_step: float,
        relative_tolerance: float,
        absolute_tolerance: float,
        numerical_solver: NumericalSolver,
    ):
        assert numerical_solver.get_log_type() == log_type
        assert numerical_solver.get_stepper_type() == variable_size_stepper_type
        assert numerical_solver.get_time_step() == initial_time_step
        assert numerical_solver.get_relative_tolerance() == relative_tolerance
        assert numerical_solver.get_absolute_tolerance() == absolute_tolerance
        assert numerical_solver.get_root_solver() is not None
        assert numerical_solver.get_observed_states() is not None

    def test_get_string_from_types(self):
        assert (
            NumericalSolver.string_from_stepper_type(
                NumericalSolver.StepperType.RungeKutta4
            )
            == "RungeKutta4"
        )
        assert (
            NumericalSolver.string_from_stepper_type(
                NumericalSolver.StepperType.RungeKuttaCashKarp54
            )
            == "RungeKuttaCashKarp54"
        )
        assert (
            NumericalSolver.string_from_stepper_type(
                NumericalSolver.StepperType.RungeKuttaFehlberg78
            )
            == "RungeKuttaFehlberg78"
        )
        assert (
            NumericalSolver.string_from_log_type(NumericalSolver.LogType.NoLog) == "NoLog"
        )
        assert (
            NumericalSolver.string_from_log_type(NumericalSolver.LogType.LogConstant)
            == "LogConstant"
        )
        assert (
            NumericalSolver.string_from_log_type(NumericalSolver.LogType.LogAdaptive)
            == "LogAdaptive"
        )

    def test_integrate_time(
        self,
        initial_state: State,
        numerical_solver: NumericalSolver,
    ):
        duration_seconds: float = 100.0
        end_instant: Instant = initial_state.get_instant() + Duration.seconds(
            duration_seconds
        )

        state_vector: np.ndarray = numerical_solver.integrate_time(
            initial_state, end_instant, oscillator
        ).get_coordinates()

        assert 5e-9 >= abs(state_vector[0] - math.sin(duration_seconds))
        assert 5e-9 >= abs(state_vector[1] - math.cos(duration_seconds))

        end_instants: list[Instant] = [
            initial_state.get_instant() + Duration.seconds(duration)
            for duration in np.arange(600.0, 1000.0, 50.0)
        ]
        states: list[State] = numerical_solver.integrate_time(
            initial_state, end_instants, oscillator
        )

        for state, end_instant in zip(states, end_instants):
            state_vector: np.ndarray = state.get_coordinates()

            assert 5e-9 >= abs(
                state_vector[0]
                - math.sin((end_instant - initial_state.get_instant()).in_seconds())
            )
            assert 5e-9 >= abs(
                state_vector[1]
                - math.cos((end_instant - initial_state.get_instant()).in_seconds())
            )

    def test_integrate_time_with_condition(
        self,
        initial_state: State,
        numerical_solver_conditional: NumericalSolver,
        custom_condition: RealCondition,
    ):
        end_time: float = initial_state.get_instant() + Duration.seconds(100.0)

        condition_solution = numerical_solver_conditional.integrate_time(
            initial_state, end_time, oscillator, custom_condition
        )

        assert condition_solution.condition_is_satisfied
        assert (
            condition_solution.iteration_count
            < numerical_solver_conditional.get_root_solver().get_maximum_iteration_count()
        )
        assert condition_solution.root_solver_has_converged

        state_vector = condition_solution.state.get_coordinates()
        time = (
            condition_solution.state.get_instant() - initial_state.get_instant()
        ).in_seconds()

        assert abs(float(time - custom_condition.get_target().value)) < 1e-6

        assert 5e-9 >= abs(state_vector[0] - math.sin(time))
        assert 5e-9 >= abs(state_vector[1] - math.cos(time))

    def test_integrate_conditional_with_logger(
        self,
        initial_state: State,
        state_logger: callable,
        custom_condition: RealCondition,
        capsys,
    ):
        numerical_solver: NumericalSolver = NumericalSolver.conditional(
            5.0,
            1.0e-15,
            1.0e-15,
            state_logger,
        )
        end_time: float = initial_state.get_instant() + Duration.seconds(10.0)

        numerical_solver.integrate_time(
            initial_state, end_time, oscillator, custom_condition
        )

        captured = capsys.readouterr()

        assert captured.out != ""

    def test_default(self):
        assert NumericalSolver.default() is not None

    def test_undefined(self):
        assert NumericalSolver.undefined() is not None
        assert NumericalSolver.undefined().is_defined() is False

    def test_fixed_step_size(
        self,
        fixed_size_stepper_type: NumericalSolver.StepperType,
        variable_size_stepper_type: NumericalSolver.StepperType,
        initial_time_step: float,
    ):
        assert (
            NumericalSolver.fixed_step_size(fixed_size_stepper_type, initial_time_step)
            is not None
        )
        with pytest.raises(Exception):
            NumericalSolver.fixed_step_size(variable_size_stepper_type, initial_time_step)

    def test_default_conditional(self, state_logger):
        assert NumericalSolver.default_conditional() is not None
        assert NumericalSolver.default_conditional(state_logger) is not None

    def test_conditional(
        self,
        initial_time_step: float,
        relative_tolerance: float,
        absolute_tolerance: float,
        state_logger,
    ):
        assert (
            NumericalSolver.conditional(
                initial_time_step, relative_tolerance, absolute_tolerance, state_logger
            )
            is not None
        )
