# Apache License 2.0

from datetime import datetime

import pytest

from ostk.mathematics.curve_fitting import Interpolator

from ostk.physics import Environment
from ostk.physics.environment.object.celestial import Earth
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.coordinate.spherical import AER

from ostk.astrodynamics import utilities
from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory.orbit.model import Tabulated as TabulatedOrbit


@pytest.fixture
def instant() -> Instant:
    return Instant.date_time(DateTime.parse("2024-01-29T00:00:00"), Scale.UTC)


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def position_1(frame: Frame) -> Position:
    return Position.meters([7000000.0, 0.0, 0.0], frame)


@pytest.fixture
def velocity_1(frame: Frame) -> Velocity:
    return Velocity.meters_per_second([0.0, 7500.0, 0.0], frame)


@pytest.fixture
def position_2(frame: Frame) -> Position:
    return Position.meters([7000100.0, 0.0, 0.0], frame)


@pytest.fixture
def velocity_2(frame: Frame) -> Velocity:
    return Velocity.meters_per_second([0.0, 7500.0, 0.0], frame)


@pytest.fixture
def state_1(
    instant: Instant,
    position_1: Position,
    velocity_1: Velocity,
) -> State:
    return State(instant=instant, position=position_1, velocity=velocity_1)


@pytest.fixture
def state_2(
    instant: Instant,
    position_2: Position,
    velocity_2: Velocity,
) -> State:
    return State(instant=instant, position=position_2, velocity=velocity_2)


@pytest.fixture
def candidate_states(state_1: State) -> list[State]:
    return [state_1, state_1, state_1]


@pytest.fixture
def reference_states(state_2: State) -> list[State]:
    return [state_2, state_2, state_2]


@pytest.fixture
def orbit(reference_states: list[State]) -> Orbit:
    return Orbit(
        model=TabulatedOrbit(
            states=reference_states,
            initial_revolution_number=1,
            interpolation_type=Interpolator.Type.Linear,
        ),
        celestial_object=Earth.default(),
    )


class TestUtility:
    def test_lla_from_state(
        self,
        state: State,
    ):
        lla: LLA = utilities.lla_from_state(state)

        assert lla is not None
        assert isinstance(lla, LLA)
        assert lla.is_defined()

    def test_lla_from_position(
        self,
        instant: Instant,
        position: Position,
    ):
        lla: LLA = utilities.lla_from_position(position, instant)

        assert lla is not None
        assert isinstance(lla, LLA)
        assert lla.is_defined()

    def test_position_from_lla(
        self,
        lla: LLA,
    ):
        position: Position = utilities.position_from_lla(lla)

        assert position is not None
        assert isinstance(position, Position)
        assert position.is_defined()

    def test_compute_aer(
        self,
        instant: Instant,
        position: Position,
        environment: Environment,
    ):
        aer: AER = utilities.compute_aer(instant, position, position, environment)

        assert aer is not None
        assert isinstance(aer, AER)

    def test_compute_time_lla_aer_coordinates(
        self,
        state: State,
        position: Position,
        environment: Environment,
    ):
        time_lla_aer: tuple[datetime, float, float, float, float, float, float] = (
            utilities.compute_time_lla_aer_coordinates(state, position, environment)
        )

        assert time_lla_aer is not None
        assert len(time_lla_aer) == 7
        assert isinstance(time_lla_aer[0], datetime)
        for i in range(1, 7):
            assert isinstance(time_lla_aer[i], float)

    def test_compute_trajectory_geometry(
        self,
        interval: Interval,
        trajectory: Trajectory,
    ):
        output: list[LLA] = utilities.compute_trajectory_geometry(trajectory, interval)

        assert output is not None
        assert len(output) == 2
        assert isinstance(output[0], LLA)

    def test_compute_ground_track(
        self,
        interval: Interval,
        trajectory: Trajectory,
    ):
        output: list[LLA] = utilities.compute_ground_track(trajectory, interval)

        assert output is not None
        assert len(output) == 2
        assert isinstance(output[0], LLA)
        assert output[0].get_altitude().in_meters() <= 15.0

    def test_convert_state(self, state: State):
        output: tuple[
            str, float, float, float, float, float, float, float, float, float
        ] = utilities.convert_state(state)

        assert output is not None
        assert len(output) == 11
        assert isinstance(output[0], str)
        for i in range(1, 11):
            assert isinstance(output[i], float)

    def test_compute_residuals_identical_states(
        self,
        candidate_states: list[State],
    ) -> None:
        residuals = utilities.compute_residuals(
            candidate_states=candidate_states,
            reference_states=candidate_states,
        )

        assert len(residuals) == len(candidate_states)
        for residual in residuals:
            assert isinstance(residual, utilities.Residual)
            assert residual.dr == pytest.approx(0.0, abs=1e-10)
            assert residual.dv == pytest.approx(0.0, abs=1e-10)
            assert residual.dr_x == pytest.approx(0.0, abs=1e-10)
            assert residual.dr_y == pytest.approx(0.0, abs=1e-10)
            assert residual.dr_z == pytest.approx(0.0, abs=1e-10)
            assert residual.dv_x == pytest.approx(0.0, abs=1e-10)
            assert residual.dv_y == pytest.approx(0.0, abs=1e-10)
            assert residual.dv_z == pytest.approx(0.0, abs=1e-10)

    def test_compute_residuals_with_local_orbital_frame_factory(
        self,
        candidate_states: list[State],
        reference_states: list[State],
    ) -> None:
        residuals = utilities.compute_residuals(
            candidate_states=candidate_states,
            reference_states=reference_states,
            local_orbital_frame_factory_or_frame=LocalOrbitalFrameFactory.VNC(
                Frame.GCRF()
            ),
        )

        assert len(residuals) == len(candidate_states)
        # Expected difference: reference_states - candidate_states = 7000000 - 7000100 = -100m in cross track-direction
        for residual in residuals:
            assert isinstance(residual, utilities.Residual)
            assert residual.dr == pytest.approx(100.0, rel=1e-6)
            assert residual.dr_x == pytest.approx(0.0, rel=1e-6)
            assert residual.dr_y == pytest.approx(0.0, abs=1e-10)
            assert residual.dr_z == pytest.approx(-100.0, abs=1e-10)
            assert residual.dv == pytest.approx(0.0, abs=1e-10)

    def test_compute_residuals_for_orbit_identical_orbit_and_states(
        self,
        orbit: Orbit,
        reference_states: list[State],
    ) -> None:
        result = utilities.compute_residuals_for_orbit(
            candidate_orbit=orbit,
            reference_states=reference_states,
        )

        assert len(result) == len(reference_states)
        for entry in result:
            assert isinstance(entry, utilities.Residual)
            assert entry.timestamp is not None
            assert entry.dr_x is not None
            assert entry.dr_y is not None
            assert entry.dr_z is not None
            assert entry.dv_x is not None
            assert entry.dv_y is not None
            assert entry.dv_z is not None
            assert isinstance(entry.timestamp, datetime)

    def test_compute_residuals_for_orbit_identical_orbit_and_states_with_local_orbital_frame_factory(
        self,
        orbit: Orbit,
        reference_states: list[State],
    ) -> None:
        result = utilities.compute_residuals_for_orbit(
            candidate_orbit=orbit,
            reference_states=reference_states,
            local_orbital_frame_factory_or_frame=LocalOrbitalFrameFactory.VNC(
                Frame.GCRF()
            ),
        )

        assert len(result) == len(reference_states)
        for entry in result:
            assert isinstance(entry, utilities.Residual)
            assert entry.timestamp is not None
            assert entry.dr_x is not None
            assert entry.dr_y is not None
            assert entry.dr_z is not None
            assert entry.dv_x is not None
            assert entry.dv_y is not None
            assert entry.dv_z is not None
            assert isinstance(entry.timestamp, datetime)

    def test_compute_residuals_for_orbits_identical_orbits(
        self,
        orbit: Orbit,
        reference_states: list[State],
    ) -> None:
        result: list[utilities.Residual] = utilities.compute_residuals_for_orbits(
            candidate_orbit=orbit,
            reference_orbit=orbit,
            instants=[state.get_instant() for state in reference_states],
        )

        assert len(result) == len(reference_states)
        for entry in result:
            assert isinstance(entry, utilities.Residual)
            assert entry.timestamp is not None
            assert entry.dr is not None
            assert entry.dr_x is not None
            assert entry.dr_y is not None
            assert entry.dr_z is not None
            assert entry.dv is not None
            assert entry.dv_x is not None
            assert entry.dv_y is not None
            assert entry.dv_z is not None
            assert isinstance(entry.timestamp, datetime)
            assert isinstance(entry.dr, float)
            assert isinstance(entry.dv, float)

    def test_compute_residuals_for_orbits_with_local_orbital_frame_factory(
        self,
        orbit: Orbit,
        reference_states: list[State],
    ) -> None:
        result: list[utilities.Residual] = utilities.compute_residuals_for_orbits(
            candidate_orbit=orbit,
            reference_orbit=orbit,
            instants=[state.get_instant() for state in reference_states],
            local_orbital_frame_factory_or_frame=LocalOrbitalFrameFactory.VNC(
                Frame.GCRF()
            ),
        )

        assert len(result) == len(reference_states)
        for entry in result:
            assert isinstance(entry, utilities.Residual)
            assert entry.timestamp is not None
            assert entry.dr is not None
            assert entry.dr_x is not None
            assert entry.dr_y is not None
            assert entry.dr_z is not None
            assert entry.dv is not None
            assert entry.dv_x is not None
            assert entry.dv_y is not None
            assert entry.dv_z is not None
            assert isinstance(entry.timestamp, datetime)
            assert isinstance(entry.dr, float)
            assert isinstance(entry.dv, float)
