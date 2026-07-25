# Apache License 2.0

import pytest

import numpy as np

from ostk.core.type import Real

from ostk.physics.coordinate import Frame, Position, Velocity
from ostk.physics.time import Instant
from ostk.physics.unit import Angle, Derived, Length

from ostk.astrodynamics.guidance_law import QLaw, EffectivityProvider
from ostk.astrodynamics.guidance_law.effectivity_provider import (
    QLaw as QLawProvider,
    Composite,
)
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE


@pytest.fixture
def gravitational_parameter() -> Derived:
    return Derived(398600.49 * 1e9, Derived.Unit.meter_cubed_per_second_squared())


@pytest.fixture
def target_coe() -> COE:
    return COE(
        Length.meters(42000.0e3),
        Real(0.01),
        Angle.degrees(Real(0.05)),
        Angle.degrees(Real(0.0)),
        Angle.degrees(Real(0.0)),
        Angle.degrees(Real(0.0)),
    )


@pytest.fixture
def current_state(gravitational_parameter: Derived) -> State:
    coe = COE(
        Length.meters(7000.0e3),
        Real(0.01),
        Angle.degrees(Real(0.05)),
        Angle.degrees(Real(0.0)),
        Angle.degrees(Real(0.0)),
        Angle.radians(Real(0.5)),  # off-sample
    )
    position, velocity = coe.get_cartesian_state(gravitational_parameter, Frame.GCRF())
    return State(Instant.J2000(), position, velocity)


@pytest.fixture
def q_law(target_coe: COE, gravitational_parameter: Derived) -> QLaw:
    parameters = QLaw.Parameters(
        {
            COE.Element.SemiMajorAxis: (1.0, 100.0),
            COE.Element.Eccentricity: (1.0, 1e-4),
            COE.Element.Inclination: (1.0, 1e-4),
            COE.Element.Raan: (1.0, 1e-4),
            COE.Element.Aop: (1.0, 1e-4),
        }
    )

    return QLaw(
        target_coe,
        gravitational_parameter,
        parameters,
        QLaw.COEDomain.Osculating,
        QLaw.GradientStrategy.Analytical,
    )


class TestQLawProvider:
    def test_constructor(self, q_law: QLaw):
        provider = QLawProvider(q_law)
        assert provider is not None
        assert isinstance(provider, QLawProvider)
        assert isinstance(provider, EffectivityProvider)

    def test_thresholds(self, q_law: QLaw):
        provider = QLawProvider(
            q_law,
            absolute_effectivity_threshold=Real(0.1),
            relative_effectivity_threshold=Real(0.2),
        )
        assert provider.get_absolute_effectivity_threshold() == Real(0.1)
        assert provider.get_relative_effectivity_threshold() == Real(0.2)

    def test_access_q_law(self, q_law: QLaw):
        provider = QLawProvider(q_law)
        assert provider.access_q_law() is not None

    def test_is_effective_no_thresholds_passes(self, q_law: QLaw, current_state: State):
        provider = QLawProvider(q_law)
        assert provider.is_effective(
            current_state, np.array([1.0, 0.0, 0.0]), 1.0 / 300.0
        )

    def test_is_effective_strict_threshold_rejects(
        self, q_law: QLaw, current_state: State
    ):
        provider = QLawProvider(q_law, relative_effectivity_threshold=Real(1.0))
        assert not provider.is_effective(
            current_state, np.array([1.0, 0.0, 0.0]), 1.0 / 300.0
        )

    def test_compute_score(self, q_law: QLaw, current_state: State):
        provider = QLawProvider(q_law)

        score_prograde = provider.compute_score(
            current_state, np.array([1.0, 0.0, 0.0]), 1.0 / 300.0
        )
        score_retrograde = provider.compute_score(
            current_state, np.array([-1.0, 0.0, 0.0]), 1.0 / 300.0
        )

        # Raising orbit → prograde dominates retrograde.
        assert score_prograde > score_retrograde


class _AlwaysProvider(EffectivityProvider):
    """Test stub: configurable gate and score."""

    def __init__(self, name: str, gate: bool, score: Real):
        super().__init__(name)
        self._gate = gate
        self._score = score

    def is_effective(self, state, direction, thrust_acceleration):
        return self._gate

    def compute_score(self, state, direction, thrust_acceleration):
        return self._score


class TestComposite:
    def test_constructor_rejects_empty(self):
        with pytest.raises(Exception):
            Composite([])

    def test_is_effective_and_of_subgates(self, current_state: State):
        passing = Composite(
            [
                _AlwaysProvider("a", True, Real(0.5)),
                _AlwaysProvider("b", True, Real(0.7)),
            ]
        )
        assert passing.is_effective(current_state, np.array([1.0, 0.0, 0.0]), 1.0 / 300.0)

        failing = Composite(
            [
                _AlwaysProvider("a", True, Real(0.5)),
                _AlwaysProvider("b", False, Real(0.7)),
            ]
        )
        assert not failing.is_effective(
            current_state, np.array([1.0, 0.0, 0.0]), 1.0 / 300.0
        )

    def test_compute_score_delegates_to_primary(self, current_state: State):
        providers = [
            _AlwaysProvider("a", True, Real(0.2)),
            _AlwaysProvider("b", True, Real(0.8)),
        ]

        primary_first = Composite(providers)
        assert primary_first.compute_score(
            current_state, np.array([1.0, 0.0, 0.0]), 1.0 / 300.0
        ) == Real(0.2)

        primary_second = Composite(providers, primary_index=1)
        assert primary_second.compute_score(
            current_state, np.array([1.0, 0.0, 0.0]), 1.0 / 300.0
        ) == Real(0.8)
        assert primary_second.get_primary_index() == 1

    def test_q_law_and_python_provider_compose(self, q_law: QLaw, current_state: State):
        # Realistic-shape composition: a QLaw provider gated alongside a Python-defined gate.
        composite = Composite(
            [
                QLawProvider(q_law),
                _AlwaysProvider("custom_gate", True, Real.undefined()),
            ]
        )
        assert composite.is_effective(
            current_state, np.array([1.0, 0.0, 0.0]), 1.0 / 300.0
        )

        # Same composition but the Python gate denies → composite denies.
        denying = Composite(
            [
                QLawProvider(q_law),
                _AlwaysProvider("custom_gate", False, Real.undefined()),
            ]
        )
        assert not denying.is_effective(
            current_state, np.array([1.0, 0.0, 0.0]), 1.0 / 300.0
        )
