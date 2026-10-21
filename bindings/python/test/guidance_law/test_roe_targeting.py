# Apache License 2.0

import pytest

import numpy as np

from ostk.physics import Environment
from ostk.physics.coordinate import Frame
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.unit import Angle
from ostk.physics.unit import Length

from ostk.astrodynamics import GuidanceLaw
from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.guidance_law import ROETargeting
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.orbit.model import Kepler
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE


@pytest.fixture
def epoch() -> Instant:
    return Instant.J2000()


@pytest.fixture
def coe() -> COE:
    return COE(
        Length.kilometers(6928.137),
        0.001,
        Angle.degrees(53.0),
        Angle.degrees(10.0),
        Angle.degrees(30.0),
        Angle.degrees(40.0),
    )


@pytest.fixture
def reference_trajectory(coe: COE, epoch: Instant) -> Orbit:
    environment = Environment.default()
    earth = environment.access_celestial_object_with_name("Earth")

    return Orbit(Kepler(coe, epoch, earth, Kepler.PerturbationType.No), earth)


@pytest.fixture
def parameters() -> ROETargeting.Parameters:
    return ROETargeting.Parameters(
        burn_spacing=Duration.days(1.0),
        maximum_burn_duration=Duration.minutes(10.0),
        thrust_acceleration=7.5e-5,
        horizon_burn_count=3,
        tolerance_vector=np.full(6, 20.0),
        fuel_weight=2.0,
        minimum_burn_duration=Duration.seconds(5.0),
        relative_semi_major_axis_rate=-1.0e-4,
        effectivity_threshold=0.8,
        burn_start_candidate_count=6,
    )


@pytest.fixture
def target_relative_orbital_elements() -> np.ndarray:
    return np.array([0.0, -5000.0, 0.0, 0.0, 0.0, 0.0])


@pytest.fixture
def roe_targeting(
    reference_trajectory: Orbit,
    parameters: ROETargeting.Parameters,
    target_relative_orbital_elements: np.ndarray,
) -> ROETargeting:
    return ROETargeting(
        reference_trajectory=reference_trajectory,
        parameters=parameters,
        target_relative_orbital_elements=target_relative_orbital_elements,
    )


@pytest.fixture
def state(reference_trajectory: Orbit, epoch: Instant) -> State:
    return reference_trajectory.get_state_at(epoch - Duration.seconds(10.0))


class TestROETargetingParameters:
    def test_constructors(self):
        parameters = ROETargeting.Parameters(
            burn_spacing=Duration.days(1.0),
            maximum_burn_duration=Duration.minutes(10.0),
            thrust_acceleration=7.5e-5,
        )

        assert isinstance(parameters, ROETargeting.Parameters)
        assert parameters.horizon_burn_count == 5
        assert np.array_equal(parameters.tolerance_vector, np.full(6, 10.0))
        assert parameters.fuel_weight == 1.0
        assert parameters.minimum_burn_duration == Duration.seconds(1.0)
        assert parameters.relative_semi_major_axis_rate == 0.0
        assert parameters.effectivity_threshold == 0.9
        assert parameters.burn_start_candidate_count == 36

    def test_constructors_failure(self):
        with pytest.raises(RuntimeError):
            ROETargeting.Parameters(
                burn_spacing=Duration.minutes(15.0),
                maximum_burn_duration=Duration.minutes(10.0),
                thrust_acceleration=7.5e-5,
            )

        with pytest.raises(RuntimeError):
            ROETargeting.Parameters(
                burn_spacing=Duration.days(1.0),
                maximum_burn_duration=Duration.minutes(10.0),
                thrust_acceleration=7.5e-5,
                effectivity_threshold=1.5,
            )

    def test_getters(self, parameters: ROETargeting.Parameters):
        assert parameters.burn_spacing == Duration.days(1.0)
        assert parameters.maximum_burn_duration == Duration.minutes(10.0)
        assert parameters.thrust_acceleration == 7.5e-5
        assert parameters.horizon_burn_count == 3
        assert np.array_equal(parameters.tolerance_vector, np.full(6, 20.0))
        assert parameters.fuel_weight == 2.0
        assert parameters.minimum_burn_duration == Duration.seconds(5.0)
        assert parameters.relative_semi_major_axis_rate == -1.0e-4
        assert parameters.effectivity_threshold == 0.8
        assert parameters.burn_start_candidate_count == 6


class TestROETargeting:
    def test_constructors(
        self,
        reference_trajectory: Orbit,
        parameters: ROETargeting.Parameters,
    ):
        roe_targeting = ROETargeting(
            reference_trajectory=reference_trajectory,
            parameters=parameters,
        )

        assert isinstance(roe_targeting, ROETargeting)
        assert isinstance(roe_targeting, GuidanceLaw)

    def test_str(self, roe_targeting: ROETargeting):
        assert str(roe_targeting) is not None
        assert repr(roe_targeting) is not None

    def test_getters(
        self,
        roe_targeting: ROETargeting,
        target_relative_orbital_elements: np.ndarray,
    ):
        assert isinstance(roe_targeting.access_reference_trajectory(), Trajectory)
        assert isinstance(roe_targeting.get_parameters(), ROETargeting.Parameters)
        assert np.array_equal(
            roe_targeting.get_target_relative_orbital_elements(),
            target_relative_orbital_elements,
        )

    def test_calculate_thrust_acceleration_at(
        self,
        roe_targeting: ROETargeting,
        state: State,
    ):
        acceleration: np.ndarray = roe_targeting.calculate_thrust_acceleration_at(
            instant=state.get_instant(),
            position_coordinates=state.get_position().get_coordinates(),
            velocity_coordinates=state.get_velocity().get_coordinates(),
            thrust_acceleration=7.5e-5,
            output_frame=Frame.GCRF(),
        )

        assert isinstance(acceleration, np.ndarray)
        assert acceleration.shape == (3,)

    def test_compute_relative_orbital_elements(
        self,
        roe_targeting: ROETargeting,
        state: State,
    ):
        relative_orbital_elements: np.ndarray = (
            roe_targeting.compute_relative_orbital_elements(state)
        )

        assert isinstance(relative_orbital_elements, np.ndarray)
        assert relative_orbital_elements.shape == (6,)

    def test_compute_plan(
        self,
        roe_targeting: ROETargeting,
        state: State,
    ):
        plan: ROETargeting.Plan = roe_targeting.compute_plan(state)

        assert isinstance(plan, ROETargeting.Plan)
        assert isinstance(plan.epoch, Instant)
        assert float(plan.reference_semi_major_axis) > 0.0
        assert plan.relative_orbital_elements.shape == (6,)
        assert len(plan.burn_intervals) == 3
        assert all(isinstance(interval, Interval) for interval in plan.burn_intervals)
        assert len(plan.delta_velocities) == 3
        assert all(
            delta_velocity.shape == (3,) for delta_velocity in plan.delta_velocities
        )
        assert len(plan.prediction_instants) == 6
        assert len(plan.predicted_relative_orbital_elements) == 6
        assert float(plan.cost) >= 0.0
        assert plan.iteration_count >= 1

        delayed_plan: ROETargeting.Plan = roe_targeting.compute_plan(
            state=state, first_burn_delay=Duration.minutes(30.0)
        )

        assert isinstance(delayed_plan, ROETargeting.Plan)

    def test_commit_maneuver_at(
        self,
        roe_targeting: ROETargeting,
        state: State,
    ):
        committed_maneuver = roe_targeting.commit_maneuver_at(
            instant=state.get_instant(),
            position_coordinates=state.get_position().get_coordinates(),
            velocity_coordinates=state.get_velocity().get_coordinates(),
            frame=Frame.GCRF(),
        )

        assert committed_maneuver is None or isinstance(
            committed_maneuver, GuidanceLaw.CommittedManeuver
        )

    def test_compute_effectivity(
        self,
        roe_targeting: ROETargeting,
        state: State,
    ):
        effectivity: float = roe_targeting.compute_effectivity(state)

        assert 0.0 <= float(effectivity) <= 1.0

    def test_compute_relative_orbital_elements_from_coe(self, coe: COE):
        relative_orbital_elements: np.ndarray = (
            ROETargeting.compute_relative_orbital_elements_from_coe(coe, coe)
        )

        assert relative_orbital_elements.shape == (6,)

    def test_compute_state_transition_matrix(self, coe: COE):
        state_transition_matrix: np.ndarray = (
            ROETargeting.compute_state_transition_matrix(coe, Duration.hours(1.0))
        )

        assert state_transition_matrix.shape == (6, 6)

    def test_compute_control_input_matrix(self):
        control_input_matrix: np.ndarray = ROETargeting.compute_control_input_matrix(
            6928137.0, 0.1, 0.2
        )

        assert control_input_matrix.shape == (6, 3)
