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
import ostk.mathematics as mathematics

import ostk.astrodynamics as astrodynamics

################################################################################################################################################################

Length = physics.units.Length
DateTime = physics.time.DateTime
Scale = physics.time.Scale
Duration = physics.time.Duration
Instant = physics.time.Instant

NumericalSolver = astrodynamics.NumericalSolver

################################################################################################################################################################

def construct_numericalsolver_default ():

    numericalsolver: NumericalSolver = NumericalSolver(NumericalSolver.IntegrationLogType.No_log, NumericalSolver.IntegrationStepperType.Runge_Kutta_Cash_Karp_54, 5.0, 1.0e-15, 1.0e-15)

    assert numericalsolver is not None
    assert isinstance(numericalsolver, NumericalSolver)
    assert numericalsolver.is_defined()

    return numericalsolver

################################################################################################################################################################

def test_numericalsolver_constructors ():

    numericalsolver: NumericalSolver = NumericalSolver(NumericalSolver.IntegrationLogType.No_log, NumericalSolver.IntegrationStepperType.Runge_Kutta_Cash_Karp_54, 5.0, 1.0e-15, 1.0e-15)

    assert numericalsolver is not None
    assert isinstance(numericalsolver, NumericalSolver)
    assert numericalsolver.is_defined()

    numericalsolver: NumericalSolver = NumericalSolver(numericalsolver)

    assert numericalsolver is not None
    assert isinstance(numericalsolver, NumericalSolver)
    assert numericalsolver.is_defined()

################################################################################################################################################################

def test_numericalsolver_comparators ():

    numericalsolver: NumericalSolver = construct_numericalsolver_default()

    assert numericalsolver == numericalsolver
    assert (numericalsolver != numericalsolver) is False

################################################################################################################################################################

def test_numericalsolver_get_types ():

    numericalsolver: NumericalSolver = construct_numericalsolver_default()

    assert numericalsolver.get_integration_stepper_type() == NumericalSolver.IntegrationStepperType.Runge_Kutta_Cash_Karp_54
    assert numericalsolver.get_integration_log_type() == NumericalSolver.IntegrationLogType.No_log
    assert numericalsolver.get_time_step() == 5.0
    assert numericalsolver.get_relative_tolerance() == 1.0e-15
    assert numericalsolver.get_absolute_tolerance() == 1.0e-15

################################################################################################################################################################

def test_numericalsolver_get_string_from_types ():

    assert NumericalSolver.string_from_integration_stepper_type(NumericalSolver.IntegrationStepperType.Runge_Kutta_Cash_Karp_54) == 'RungeKuttaCashKarp54'

    assert NumericalSolver.string_from_integration_stepper_type(NumericalSolver.IntegrationStepperType.Runge_Kutta_Fehlberg_78) == 'RungeKuttaFehlberg78'

    assert NumericalSolver.string_from_integration_log_type(NumericalSolver.IntegrationLogType.No_log) == 'NoLog'

    assert NumericalSolver.string_from_integration_log_type(NumericalSolver.IntegrationLogType.Log_constant) == 'LogConstant'

    assert NumericalSolver.string_from_integration_log_type(NumericalSolver.IntegrationLogType.Log_adaptive) == 'LogAdaptive'

################################################################################################################################################################

def test_numericalsolver_integrate_state_for_duration ():

    numericalsolver: NumericalSolver = NumericalSolver(NumericalSolver.IntegrationLogType.No_log, NumericalSolver.IntegrationStepperType.Runge_Kutta_Cash_Karp_54, 5.0, 1.0e-15, 1.0e-15)

    initial_state_vec = np.array([0., 1.])

    integration_duration = Duration.seconds(100.0)

    def oscillator (x, dxdt, t):
        dxdt[0] = x[1]
        dxdt[1] = -x[0]
        return dxdt

    prop_state_vector = numericalsolver.integrate_state_for_duration(initial_state_vec, integration_duration, oscillator)

    assert 5e-9 >= abs(prop_state_vector[0] - math.sin(integration_duration.in_seconds()))
    assert 5e-9 >= abs(prop_state_vector[1] - math.cos(integration_duration.in_seconds()))

################################################################################################################################################################

def test_numericalsolver_integrate_state_from_instant_to_instant ():

    numericalsolver: NumericalSolver = NumericalSolver(NumericalSolver.IntegrationLogType.No_log, NumericalSolver.IntegrationStepperType.Runge_Kutta_Cash_Karp_54, 5.0, 1.0e-15, 1.0e-15)

    initial_state_vec = np.array([0., 1.])

    start_instant = Instant.J2000()
    end_instant = start_instant + Duration.seconds(100.0)

    def oscillator (x, dxdt, t):
        dxdt[0] = x[1]
        dxdt[1] = -x[0]
        return dxdt

    prop_state_vector = numericalsolver.integrate_state_from_instant_to_instant(initial_state_vec, start_instant, end_instant, oscillator)

    assert 5e-9 >= abs(prop_state_vector[0] - math.sin( (end_instant - start_instant).in_seconds() ))
    assert 5e-9 >= abs(prop_state_vector[1] - math.cos( (end_instant - start_instant).in_seconds() ))

################################################################################################################################################################
