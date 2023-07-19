# Apache License 2.0

import pytest

import numpy as np
import math

from ostk.astrodynamics import NumericalSolver


def oscillator(x, dxdt, _):
    dxdt[0] = x[1]
    dxdt[1] = -x[0]
    return dxdt


@pytest.fixture
def initial_state_vec() -> np.ndarray:
    return np.array([0.0, 1.0])


@pytest.fixture
def numerical_solver_default_inputs() -> (
    tuple[NumericalSolver.LogType, NumericalSolver.StepperType, float, float, float]
):
    log_type = NumericalSolver.LogType.NoLog
    stepper_type = NumericalSolver.StepperType.RungeKuttaCashKarp54
    initial_time_step = 5.0
    relative_tolerance = 1.0e-15
    absolute_tolerance = 1.0e-15

    return (
        log_type,
        stepper_type,
        initial_time_step,
        relative_tolerance,
        absolute_tolerance,
    )


@pytest.fixture
def numerical_solver(numerical_solver_default_inputs) -> NumericalSolver:
    return NumericalSolver(*numerical_solver_default_inputs)


class TestNumericalSolver:
    def test_constructors(self, numerical_solver: NumericalSolver):
        assert numerical_solver is not None
        assert isinstance(numerical_solver, NumericalSolver)
        assert numerical_solver.is_defined()

        numericalsolver_2: NumericalSolver = NumericalSolver(numerical_solver)

        assert numericalsolver_2 is not None
        assert isinstance(numericalsolver_2, NumericalSolver)
        assert numericalsolver_2.is_defined()

    def test_comparators(self, numerical_solver: NumericalSolver):
        assert numerical_solver == numerical_solver
        assert (numerical_solver != numerical_solver) is False

    def test_get_types(
        self,
        numerical_solver_default_inputs: tuple[
            NumericalSolver.LogType, NumericalSolver.StepperType, float, float, float
        ],
        numerical_solver: NumericalSolver,
    ):
        (
            log_type,
            stepper_type,
            initial_time_step,
            relative_tolerance,
            absolute_tolerance,
        ) = numerical_solver_default_inputs

        assert numerical_solver.get_log_type() == log_type
        assert numerical_solver.get_stepper_type() == stepper_type
        assert numerical_solver.get_time_step() == initial_time_step
        assert numerical_solver.get_relative_tolerance() == relative_tolerance
        assert numerical_solver.get_absolute_tolerance() == absolute_tolerance

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

    def test_integrate_state_for_durations(
        self, numerical_solver: NumericalSolver, initial_state_vec: np.ndarray
    ):
        integration_duration: float = 100.0

        state_vector, _ = numerical_solver.integrate_state_for_durations(
            initial_state_vec, integration_duration, oscillator
        )

        assert 5e-9 >= abs(state_vector[0] - math.sin(integration_duration))
        assert 5e-9 >= abs(state_vector[1] - math.cos(integration_duration))

        integration_durations = np.arange(100.0, 1000.0, 50.0)
        solutions = numerical_solver.integrate_state_for_durations(
            initial_state_vec, integration_durations, oscillator
        )

        for solution, integration_duration in zip(solutions, integration_durations):
            state_vector, _ = solution

            assert 5e-9 >= abs(state_vector[0] - math.sin(integration_duration))
            assert 5e-9 >= abs(state_vector[1] - math.cos(integration_duration))

    def test_integrate_state_to_times(
        self, numerical_solver: NumericalSolver, initial_state_vec: np.ndarray
    ):
        start_time: float = 500.0
        end_time: float = start_time + 100.0

        state_vector, _ = numerical_solver.integrate_state_to_times(
            initial_state_vec, start_time, end_time, oscillator
        )

        assert 5e-9 >= abs(state_vector[0] - math.sin(end_time - start_time))
        assert 5e-9 >= abs(state_vector[1] - math.cos(end_time - start_time))

        end_times = np.arange(600.0, 1000.0, 50.0)
        solutions = numerical_solver.integrate_state_to_times(
            initial_state_vec, start_time, end_times, oscillator
        )

        for solution, end_time in zip(solutions, end_times):
            state_vector, _ = solution

            assert 5e-9 >= abs(state_vector[0] - math.sin(end_time - start_time))
            assert 5e-9 >= abs(state_vector[1] - math.cos(end_time - start_time))

    def test_default(self):
        assert NumericalSolver.default() is not None

    def test_undefined(self):
        assert NumericalSolver.undefined() is not None
        assert NumericalSolver.undefined().is_defined() is False
