# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.unit import Derived
from ostk.physics.unit import Length
from ostk.physics.unit import Angle
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Instant
from ostk.physics.coordinate import Frame
from ostk.physics import Environment
from ostk.physics.environment.object import Celestial

from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory.orbit.model import Kepler
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE
from ostk.astrodynamics.conjunction import CloseApproach
from ostk.astrodynamics.uncertainty import Covariance


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def earth(environment: Environment) -> Celestial:
    return environment.access_celestial_object_with_name("Earth")


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime(2024, 1, 1, 0, 0, 0), Scale.UTC)


@pytest.fixture
def gcrf_frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def object_1_state(
    instant: Instant,
    earth: Celestial,
) -> State:
    orbit = Orbit(
        model=Kepler(
            coe=COE(
                semi_major_axis=Length.kilometers(7000.0),
                eccentricity=0.0,
                inclination=Angle.degrees(0.0),
                raan=Angle.degrees(0.0),
                aop=Angle.degrees(0.0),
                true_anomaly=Angle.degrees(0.0),
            ),
            epoch=instant,
            celestial_object=earth,
            perturbation_type=Kepler.PerturbationType.No,
        ),
        celestial_object=earth,
    )
    return orbit.get_state_at(instant)


@pytest.fixture
def object_2_state(
    instant: Instant,
    earth: Celestial,
) -> State:
    orbit = Orbit(
        model=Kepler(
            coe=COE(
                semi_major_axis=Length.kilometers(7000.0),
                eccentricity=0.0,
                inclination=Angle.degrees(90.0),
                raan=Angle.degrees(0.0),
                aop=Angle.degrees(0.0),
                true_anomaly=Angle.degrees(1.0),
            ),
            epoch=instant,
            celestial_object=earth,
            perturbation_type=Kepler.PerturbationType.No,
        ),
        celestial_object=earth,
    )
    return orbit.get_state_at(instant)


@pytest.fixture
def object_1_covariance(
    instant: Instant,
    gcrf_frame: Frame,
) -> Covariance:
    return Covariance.from_position_velocity_sigmas(
        instant=instant,
        position_sigmas=[1.0, 1.0, 1.0],
        velocity_sigmas=[0.01, 0.01, 0.01],
        frame=gcrf_frame,
    )


@pytest.fixture
def object_2_covariance(
    instant: Instant,
    gcrf_frame: Frame,
) -> Covariance:
    return Covariance.from_position_velocity_sigmas(
        instant=instant,
        position_sigmas=[2.0, 2.0, 2.0],
        velocity_sigmas=[0.02, 0.02, 0.02],
        frame=gcrf_frame,
    )


@pytest.fixture
def object_1_state_with_covariance(
    object_1_state: State,
    object_1_covariance: Covariance,
) -> State:
    return object_1_state.with_covariance(object_1_covariance)


@pytest.fixture
def object_2_state_with_covariance(
    object_2_state: State,
    object_2_covariance: Covariance,
) -> State:
    return object_2_state.with_covariance(object_2_covariance)


@pytest.fixture
def close_approach(
    object_1_state: State,
    object_2_state: State,
) -> CloseApproach:
    return CloseApproach(
        object_1_state=object_1_state,
        object_2_state=object_2_state,
    )


@pytest.fixture
def close_approach_with_covariances(
    object_1_state_with_covariance: State,
    object_2_state_with_covariance: State,
) -> CloseApproach:
    return CloseApproach(
        object_1_state=object_1_state_with_covariance,
        object_2_state=object_2_state_with_covariance,
    )


class TestCloseApproach:
    def test_constructor_success(
        self,
        object_1_state: State,
        object_2_state: State,
    ):
        close_approach = CloseApproach(
            object_1_state=object_1_state,
            object_2_state=object_2_state,
        )

        assert close_approach is not None
        assert isinstance(close_approach, CloseApproach)

    def test_constructor_success_with_covariances(
        self,
        object_1_state_with_covariance: State,
        object_2_state_with_covariance: State,
    ):
        close_approach = CloseApproach(
            object_1_state=object_1_state_with_covariance,
            object_2_state=object_2_state_with_covariance,
        )

        assert close_approach is not None
        assert isinstance(close_approach, CloseApproach)
        assert close_approach.is_defined() is True

        relative_speed = close_approach.get_relative_speed()

        assert relative_speed is not None
        assert isinstance(relative_speed, Derived)
        assert relative_speed.in_unit(Derived.Unit.meter_per_second()) > 0.0
        assert close_approach.get_object_1_state().has_covariance() is True
        assert close_approach.get_object_2_state().has_covariance() is True

    def test_is_defined_success(
        self,
        close_approach: CloseApproach,
    ):
        assert close_approach.is_defined() is True

    def test_undefined_success(self):
        close_approach = CloseApproach.undefined()

        assert close_approach is not None
        assert isinstance(close_approach, CloseApproach)
        assert close_approach.is_defined() is False

    def test_get_object_1_state_success(
        self,
        close_approach: CloseApproach,
        object_1_state: State,
    ):
        state = close_approach.get_object_1_state()

        assert state is not None
        assert isinstance(state, State)
        assert state.get_instant() == object_1_state.get_instant()

    def test_get_object_2_state_success(
        self,
        close_approach: CloseApproach,
        object_2_state: State,
    ):
        state = close_approach.get_object_2_state()

        assert state is not None
        assert isinstance(state, State)
        assert state.get_instant() == object_2_state.get_instant()

    def test_scale_success(
        self,
        close_approach_with_covariances: CloseApproach,
        object_1_covariance: Covariance,
        object_2_covariance: Covariance,
    ):
        scaled_close_approach = close_approach_with_covariances.scale(
            scale_factor_1=2.0,
            scale_factor_2=4.0,
        )

        assert scaled_close_approach is not None
        assert isinstance(scaled_close_approach, CloseApproach)
        assert scaled_close_approach.is_defined() is True
        assert np.allclose(
            scaled_close_approach.get_object_1_state().get_covariance().get_coordinates(),
            object_1_covariance.scale(2.0).get_coordinates(),
        )
        assert np.allclose(
            scaled_close_approach.get_object_2_state().get_covariance().get_coordinates(),
            object_2_covariance.scale(4.0).get_coordinates(),
        )

        scaled_object_1_only = close_approach_with_covariances.scale(scale_factor_1=2.0)

        assert scaled_object_1_only.is_defined() is True
        assert np.allclose(
            scaled_object_1_only.get_object_1_state().get_covariance().get_coordinates(),
            object_1_covariance.scale(2.0).get_coordinates(),
        )
        assert np.allclose(
            scaled_object_1_only.get_object_2_state().get_covariance().get_coordinates(),
            object_2_covariance.get_coordinates(),
        )

        scaled_object_2_only = close_approach_with_covariances.scale(
            scale_factor_1=None,
            scale_factor_2=4.0,
        )

        assert scaled_object_2_only.is_defined() is True
        assert np.allclose(
            scaled_object_2_only.get_object_1_state().get_covariance().get_coordinates(),
            object_1_covariance.get_coordinates(),
        )
        assert np.allclose(
            scaled_object_2_only.get_object_2_state().get_covariance().get_coordinates(),
            object_2_covariance.scale(4.0).get_coordinates(),
        )

        unscaled_close_approach = close_approach_with_covariances.scale()

        assert unscaled_close_approach.is_defined() is True
        assert (
            unscaled_close_approach.get_object_1_state()
            == close_approach_with_covariances.get_object_1_state()
        )
        assert (
            unscaled_close_approach.get_object_2_state()
            == close_approach_with_covariances.get_object_2_state()
        )

    def test_scale_success_no_covariance(
        self,
        close_approach: CloseApproach,
    ):
        unscaled_close_approach = close_approach.scale()

        assert unscaled_close_approach.is_defined() is True
        assert (
            unscaled_close_approach.get_object_1_state()
            == close_approach.get_object_1_state()
        )
        assert (
            unscaled_close_approach.get_object_2_state()
            == close_approach.get_object_2_state()
        )

    def test_swap_success(
        self,
        close_approach_with_covariances: CloseApproach,
        object_1_state_with_covariance: State,
        object_2_state_with_covariance: State,
        object_1_covariance: Covariance,
        object_2_covariance: Covariance,
    ):
        swapped_close_approach = close_approach_with_covariances.swap()

        assert swapped_close_approach is not None
        assert isinstance(swapped_close_approach, CloseApproach)
        assert swapped_close_approach.is_defined() is True
        assert (
            swapped_close_approach.get_object_1_state() == object_2_state_with_covariance
        )
        assert (
            swapped_close_approach.get_object_2_state() == object_1_state_with_covariance
        )
        assert np.allclose(
            swapped_close_approach.get_object_1_state()
            .get_covariance()
            .get_coordinates(),
            object_2_covariance.get_coordinates(),
        )
        assert np.allclose(
            swapped_close_approach.get_object_2_state()
            .get_covariance()
            .get_coordinates(),
            object_1_covariance.get_coordinates(),
        )
        assert (
            swapped_close_approach.swap().get_object_1_state()
            == close_approach_with_covariances.get_object_1_state()
        )
        assert (
            swapped_close_approach.swap().get_object_2_state()
            == close_approach_with_covariances.get_object_2_state()
        )

    def test_get_instant_success(
        self,
        close_approach: CloseApproach,
        instant: Instant,
    ):
        ca_instant = close_approach.get_instant()

        assert ca_instant is not None
        assert isinstance(ca_instant, Instant)
        assert ca_instant == instant

    def test_get_miss_distance_success(
        self,
        close_approach: CloseApproach,
    ):
        miss_distance = close_approach.get_miss_distance()

        assert miss_distance is not None
        assert isinstance(miss_distance, Length)
        assert miss_distance.in_meters() > 0.0

    def test_get_relative_state_success(
        self,
        close_approach: CloseApproach,
    ):
        relative_state = close_approach.get_relative_state()

        assert relative_state is not None
        assert isinstance(relative_state, State)

    def test_get_relative_speed_success(
        self,
        close_approach: CloseApproach,
    ):
        relative_speed = close_approach.get_relative_speed()

        assert relative_speed is not None
        assert isinstance(relative_speed, Derived)
        assert relative_speed.in_unit(Derived.Unit.meter_per_second()) > 0.0

    def test_get_encounter_frame_success(
        self,
        close_approach: CloseApproach,
        gcrf_frame: Frame,
    ):
        encounter_frame = close_approach.get_encounter_frame()

        assert encounter_frame is not None
        assert isinstance(encounter_frame, Frame)
        assert encounter_frame.is_defined() is True
        assert encounter_frame.access_parent() == gcrf_frame
        assert close_approach.get_encounter_frame(frame=gcrf_frame) == encounter_frame

    def test_compute_miss_distance_components_in_frame_success(
        self,
        close_approach: CloseApproach,
        gcrf_frame: Frame,
    ):
        components = close_approach.compute_miss_distance_components_in_frame(
            frame=gcrf_frame
        )

        assert components is not None
        assert isinstance(components, tuple)
        assert len(components) == 3

        # Check each component is a Length
        for component in components:
            assert isinstance(component, Length)

    def test_compute_miss_distance_components_in_local_orbital_frame_success(
        self,
        close_approach: CloseApproach,
        gcrf_frame: Frame,
    ):
        qsw_factory = LocalOrbitalFrameFactory.QSW(gcrf_frame)

        components = (
            close_approach.compute_miss_distance_components_in_local_orbital_frame(
                local_orbital_frame_factory=qsw_factory
            )
        )

        assert components is not None
        assert isinstance(components, tuple)
        assert len(components) == 3

        # Check each component is a Length
        for component in components:
            assert isinstance(component, Length)

    def test_string_representation_success(
        self,
        close_approach: CloseApproach,
    ):
        string_repr = str(close_approach)

        assert string_repr is not None
        assert isinstance(string_repr, str)
        assert len(string_repr) > 0
