################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/test_numerical_solver.py
# @author         Antoine Paletta <antoine.paletta@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

import pytest

import numpy as np
import math

import ostk.physics as physics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Length = physics.units.Length
DateTime = physics.time.DateTime
Scale = physics.time.Scale
Duration = physics.time.Duration
Instant = physics.time.Instant

NumericalSolver = astrodynamics.NumericalSolver

################################################################################################################################################################

@pytest.fixture
def numerical_solver_default_inputs ():

    log_type = NumericalSolver.LogType.NoLog
    stepper_type = NumericalSolver.StepperType.RungeKuttaCashKarp54
    initial_time_step = 5.0
    relative_tolerance = 1.0e-15
    absolute_tolerance = 1.0e-15

    return log_type, stepper_type, initial_time_step, relative_tolerance, absolute_tolerance

@pytest.fixture
def numerical_solver (numerical_solver_default_inputs) -> NumericalSolver:

    return NumericalSolver(*numerical_solver_default_inputs)

################################################################################################################################################################

class TestNumericalSolver:

    def test_constructors (self, numerical_solver: NumericalSolver):

        assert numerical_solver is not None
        assert isinstance(numerical_solver, NumericalSolver)
        assert numerical_solver.is_defined()

        numericalsolver_2: NumericalSolver = NumericalSolver(numerical_solver)

        assert numericalsolver_2 is not None
        assert isinstance(numericalsolver_2, NumericalSolver)
        assert numericalsolver_2.is_defined()

    def test_comparators (self, numerical_solver: NumericalSolver):

        assert numerical_solver == numerical_solver
        assert (numerical_solver != numerical_solver) is False

    def test_get_types (self, numerical_solver_default_inputs, numerical_solver: NumericalSolver):

        (log_type, stepper_type, initial_time_step, relative_tolerance, absolute_tolerance) = numerical_solver_default_inputs

        assert numerical_solver.get_log_type() == log_type
        assert numerical_solver.get_stepper_type() == stepper_type
        assert numerical_solver.get_time_step() == initial_time_step
        assert numerical_solver.get_relative_tolerance() == relative_tolerance
        assert numerical_solver.get_absolute_tolerance() == absolute_tolerance

    def test_get_string_from_types (self):

        assert NumericalSolver.string_from_stepper_type(NumericalSolver.StepperType.RungeKuttaCashKarp54) == 'RungeKuttaCashKarp54'
        assert NumericalSolver.string_from_stepper_type(NumericalSolver.StepperType.RungeKuttaFehlberg78) == 'RungeKuttaFehlberg78'
        assert NumericalSolver.string_from_log_type(NumericalSolver.LogType.NoLog) == 'NoLog'
        assert NumericalSolver.string_from_log_type(NumericalSolver.LogType.LogConstant) == 'LogConstant'
        assert NumericalSolver.string_from_log_type(NumericalSolver.LogType.LogAdaptive) == 'LogAdaptive'

    def test_integrate_state_for_duration (self, numerical_solver: NumericalSolver):

        initial_state_vec = np.array([0., 1.])

        integration_duration = Duration.seconds(100.0)

        def oscillator (x, dxdt, t):
            dxdt[0] = x[1]
            dxdt[1] = -x[0]
            return dxdt

        prop_state_vector = numerical_solver.integrate_state_for_duration(initial_state_vec, integration_duration, oscillator)

        assert 5e-9 >= abs(prop_state_vector[0] - math.sin(integration_duration.in_seconds()))
        assert 5e-9 >= abs(prop_state_vector[1] - math.cos(integration_duration.in_seconds()))

    def test_integrate_state_from_instant_to_instant (self, numerical_solver: NumericalSolver):

        initial_state_vec = np.array([0., 1.])

        start_instant = Instant.J2000()
        end_instant = start_instant + Duration.seconds(100.0)

        def oscillator (x, dxdt, t):
            dxdt[0] = x[1]
            dxdt[1] = -x[0]
            return dxdt

        prop_state_vector = numerical_solver.integrate_state_from_instant_to_instant(initial_state_vec, start_instant, end_instant, oscillator)

        assert 5e-9 >= abs(prop_state_vector[0] - math.sin((end_instant - start_instant).in_seconds()))
        assert 5e-9 >= abs(prop_state_vector[1] - math.cos((end_instant - start_instant).in_seconds()))

################################################################################################################################################################
