################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/test/test_numericalsolver.py
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
def numericalsolver_default_inputs_fix ():

    integration_log_type = NumericalSolver.IntegrationLogType.No_log
    integration_stepper_type = NumericalSolver.IntegrationStepperType.Runge_Kutta_Cash_Karp_54
    initial_time_step = 5.0
    relative_tolerance = 1.0e-15
    absolute_tolerance = 1.0e-15

    return integration_log_type, integration_stepper_type, initial_time_step, relative_tolerance, absolute_tolerance

@pytest.fixture
def numericalsolver_fix (numericalsolver_default_inputs_fix) -> NumericalSolver:
    integration_log_type, integration_stepper_type, initial_time_step, relative_tolerance, absolute_tolerance = numericalsolver_default_inputs_fix

    numerical_solver: NumericalSolver = NumericalSolver(integration_log_type, integration_stepper_type, initial_time_step, relative_tolerance, absolute_tolerance)

    return numerical_solver

################################################################################################################################################################

class TestNumericalSolver:

    def test_constructors (self, numericalsolver_fix: NumericalSolver):

        assert numericalsolver_fix is not None
        assert isinstance(numericalsolver_fix, NumericalSolver)
        assert numericalsolver_fix.is_defined()

        numericalsolver_2: NumericalSolver = NumericalSolver(numericalsolver_fix)

        assert numericalsolver_2 is not None
        assert isinstance(numericalsolver_2, NumericalSolver)
        assert numericalsolver_2.is_defined()

    def test_comparators (self, numericalsolver_fix: NumericalSolver):

        assert numericalsolver_fix == numericalsolver_fix
        assert (numericalsolver_fix != numericalsolver_fix) is False

    def test_get_types (self, numericalsolver_default_inputs_fix, numericalsolver_fix: NumericalSolver):
        integration_log_type, integration_stepper_type, initial_time_step, relative_tolerance, absolute_tolerance = numericalsolver_default_inputs_fix

        assert numericalsolver_fix.get_integration_log_type() == integration_log_type
        assert numericalsolver_fix.get_integration_stepper_type() == integration_stepper_type
        assert numericalsolver_fix.get_time_step() == initial_time_step
        assert numericalsolver_fix.get_relative_tolerance() == relative_tolerance
        assert numericalsolver_fix.get_absolute_tolerance() == absolute_tolerance

    def test_get_string_from_types (self):

        assert NumericalSolver.string_from_integration_stepper_type(NumericalSolver.IntegrationStepperType.Runge_Kutta_Cash_Karp_54) == 'RungeKuttaCashKarp54'

        assert NumericalSolver.string_from_integration_stepper_type(NumericalSolver.IntegrationStepperType.Runge_Kutta_Fehlberg_78) == 'RungeKuttaFehlberg78'

        assert NumericalSolver.string_from_integration_log_type(NumericalSolver.IntegrationLogType.No_log) == 'NoLog'

        assert NumericalSolver.string_from_integration_log_type(NumericalSolver.IntegrationLogType.Log_constant) == 'LogConstant'

        assert NumericalSolver.string_from_integration_log_type(NumericalSolver.IntegrationLogType.Log_adaptive) == 'LogAdaptive'

    def test_integrate_state_for_duration (self, numericalsolver_fix: NumericalSolver):

        initial_state_vec = np.array([0., 1.])

        integration_duration = Duration.seconds(100.0)

        def oscillator (x, dxdt, t):
            dxdt[0] = x[1]
            dxdt[1] = -x[0]
            return dxdt

        prop_state_vector = numericalsolver_fix.integrate_state_for_duration(initial_state_vec, integration_duration, oscillator)

        assert 5e-9 >= abs(prop_state_vector[0] - math.sin(integration_duration.in_seconds()))
        assert 5e-9 >= abs(prop_state_vector[1] - math.cos(integration_duration.in_seconds()))

    def test_integrate_state_from_instant_to_instant (self, numericalsolver_fix: NumericalSolver):

        initial_state_vec = np.array([0., 1.])

        start_instant = Instant.J2000()
        end_instant = start_instant + Duration.seconds(100.0)

        def oscillator (x, dxdt, t):
            dxdt[0] = x[1]
            dxdt[1] = -x[0]
            return dxdt

        prop_state_vector = numericalsolver_fix.integrate_state_from_instant_to_instant(initial_state_vec, start_instant, end_instant, oscillator)

        assert 5e-9 >= abs(prop_state_vector[0] - math.sin( (end_instant - start_instant).in_seconds() ))
        assert 5e-9 >= abs(prop_state_vector[1] - math.cos( (end_instant - start_instant).in_seconds() ))

################################################################################################################################################################
