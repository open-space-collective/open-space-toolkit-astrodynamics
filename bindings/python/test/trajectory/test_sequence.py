# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.geometry.d3.objects import Composite
from ostk.mathematics.geometry.d3.objects import Cuboid
from ostk.mathematics.geometry.d3.objects import Point

from ostk.physics import Environment
from ostk.physics.coordinate import Frame
from ostk.physics.environment.atmospheric import Earth as EarthAtmosphericModel
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.environment.magnetic import Earth as EarthMagneticModel
from ostk.physics.environment.objects.celestial_bodies import Earth
from ostk.physics.time import DateTime
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.time import Scale
from ostk.physics.units import Derived
from ostk.physics.units import Length
from ostk.physics.units import Mass

from ostk.astrodynamics.event_condition import COECondition
from ostk.astrodynamics.event_condition import InstantCondition
from ostk.astrodynamics.event_condition import RealCondition
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.flight.system import PropulsionSystem
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.dynamics.thruster import ConstantThrust
from ostk.astrodynamics.trajectory import LocalOrbitalFrameDirection
from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import Segment
from ostk.astrodynamics.trajectory import Sequence
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.state import CoordinatesBroker
from ostk.astrodynamics.trajectory.state import CoordinatesSubset
from ostk.astrodynamics.trajectory.state import NumericalSolver
from ostk.astrodynamics.trajectory.state.coordinates_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinates_subset import CartesianVelocity


@pytest.fixture
def propulsion_system() -> PropulsionSystem:
    return PropulsionSystem(
        1.0,
        1500.0,
    )


@pytest.fixture
def dry_mass() -> Mass:
    return Mass.kilograms(100.0)


@pytest.fixture
def wet_mass() -> Mass:
    return Mass.kilograms(10.0)


@pytest.fixture
def cross_sectional_surface_area() -> float:
    return 1.0


@pytest.fixture
def drag_coefficient() -> float:
    return 2.2


@pytest.fixture
def satellite_system(
    dry_mass: Mass,
    cross_sectional_surface_area: float,
    drag_coefficient: float,
    propulsion_system: PropulsionSystem,
) -> SatelliteSystem:
    satellite_geometry = Composite(
        Cuboid(
            Point(0.0, 0.0, 0.0),
            [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],
            [1.0, 0.0, 0.0],
        )
    )
    inertia_tensor = np.ndarray(shape=(3, 3))

    return SatelliteSystem(
        dry_mass,
        satellite_geometry,
        inertia_tensor,
        cross_sectional_surface_area,
        drag_coefficient,
        propulsion_system,
    )


@pytest.fixture
def environment() -> Environment:
    return Environment(
        Instant.J2000(),
        [
            Earth.from_models(
                EarthGravitationalModel(EarthGravitationalModel.Type.Spherical),
                EarthMagneticModel(EarthMagneticModel.Type.Undefined),
                EarthAtmosphericModel(EarthAtmosphericModel.Type.Exponential),
            )
        ],
    )


@pytest.fixture
def coordinates_broker() -> CoordinatesBroker:
    return CoordinatesBroker(
        [
            CartesianPosition.default(),
            CartesianVelocity.default(),
            CoordinatesSubset.mass(),
            CoordinatesSubset.surface_area(),
            CoordinatesSubset.drag_coefficient(),
        ]
    )


@pytest.fixture
def cartesian_coordinates() -> list[float]:
    return [
        717094.039086306,
        -6872433.2241124,
        46175.9696673281,
        -970.650826004612,
        -45.4598114773158,
        7529.82424886455,
    ]


@pytest.fixture
def state(
    cartesian_coordinates: list[float],
    dry_mass: Mass,
    wet_mass: Mass,
    cross_sectional_surface_area: float,
    drag_coefficient: float,
    coordinates_broker: CoordinatesBroker,
) -> State:
    frame: Frame = Frame.GCRF()
    instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)
    coordinates = [
        *cartesian_coordinates,
        dry_mass.in_kilograms() + wet_mass.in_kilograms(),
        cross_sectional_surface_area,
        drag_coefficient,
    ]

    return State(
        instant,
        [
            717094.039086306,
            -6872433.2241124,
            46175.9696673281,
            -970.650826004612,
            -45.4598114773158,
            7529.82424886455,
            dry_mass.in_kilograms() + wet_mass.in_kilograms(),
            cross_sectional_surface_area,
            drag_coefficient,
        ],
        frame,
        coordinates_broker,
    )


@pytest.fixture
def dynamics(environment: Environment) -> list:
    return Dynamics.from_environment(environment)


@pytest.fixture
def numerical_solver() -> NumericalSolver:
    return NumericalSolver.default_conditional()


@pytest.fixture
def duration() -> Duration:
    return Duration.minutes(5.0)


@pytest.fixture
def instant_condition(state: State, duration: Duration) -> InstantCondition:
    return InstantCondition(
        RealCondition.Criterion.AnyCrossing, state.get_instant() + duration
    )


@pytest.fixture
def sma() -> Length:
    return Length.kilometers(6907.000)


@pytest.fixture
def gravitational_parameter() -> Derived:
    return EarthGravitationalModel.spherical.gravitational_parameter


@pytest.fixture
def constant_thrust(satellite_system: SatelliteSystem) -> ConstantThrust:
    return ConstantThrust(
        satellite_system=satellite_system,
        thrust_direction=LocalOrbitalFrameDirection(
            vector=[1.0, 0.0, 0.0],
            local_orbital_frame_factory=LocalOrbitalFrameFactory.VNC(Frame.GCRF()),
        ),
    )


@pytest.fixture
def sma_condition(sma: Length, gravitational_parameter: Derived) -> COECondition:
    return COECondition.semi_major_axis(
        criterion=RealCondition.Criterion.AnyCrossing,
        frame=Frame.GCRF(),
        semi_major_axis=sma,
        gravitational_parameter=gravitational_parameter,
    )


@pytest.fixture
def coast_duration_segment(
    instant_condition: InstantCondition,
    dynamics: list[Dynamics],
    numerical_solver: NumericalSolver,
):
    return Segment.coast(
        name="duration coast",
        event_condition=instant_condition,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
    )


@pytest.fixture
def coast_sma_segment(
    sma_condition: COECondition,
    dynamics: list[Dynamics],
    numerical_solver: NumericalSolver,
):
    return Segment.coast(
        name="sma coast",
        event_condition=sma_condition,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
    )


@pytest.fixture
def thrust_segment(
    sma_condition: RealCondition,
    constant_thrust: ConstantThrust,
    dynamics: list[Dynamics],
    numerical_solver: NumericalSolver,
):
    return Segment.maneuver(
        name="duration thrust",
        event_condition=sma_condition,
        thruster_dynamics=constant_thrust,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
    )


@pytest.fixture
def segments(
    coast_duration_segment: Segment,
    thrust_segment: Segment,
) -> list[Segment]:
    return [coast_duration_segment, thrust_segment]


@pytest.fixture
def maximum_propagation_duration() -> Duration:
    return Duration.days(2.0)


@pytest.fixture
def repetition_count() -> int:
    return 1


@pytest.fixture
def sequence(
    segments: list[Segment],
    repetition_count: int,
    numerical_solver: NumericalSolver,
    dynamics: list,
    maximum_propagation_duration: Duration,
):
    return Sequence(
        segments=segments,
        repetition_count=repetition_count,
        dynamics=dynamics,
        numerical_solver=numerical_solver,
        maximum_propagation_duration=maximum_propagation_duration,
    )


class TestSequence:
    def test_get_segments(
        self,
        sequence: Sequence,
        segments: list[Segment],
    ):
        assert len(sequence.get_segments()) == len(segments)

    def test_get_numerical_solver(
        self,
        sequence: Sequence,
        numerical_solver: NumericalSolver,
    ):
        assert sequence.get_numerical_solver() == numerical_solver

    def test_get_dynamics(
        self,
        sequence: Sequence,
        dynamics: list,
    ):
        assert len(sequence.get_dynamics()) == len(dynamics)

    def test_get_maximum_propagation_duration(
        self,
        sequence: Sequence,
        maximum_propagation_duration: Duration,
    ):
        assert sequence.get_maximum_propagation_duration() == maximum_propagation_duration

    def test_add_segment(
        self,
        sequence: Sequence,
        coast_duration_segment: Segment,
    ):
        segments_count: int = len(sequence.get_segments())

        sequence.add_segment(coast_duration_segment)

        assert len(sequence.get_segments()) == segments_count + 1

        segments_count = len(sequence.get_segments())

        sequence.add_segment([coast_duration_segment, coast_duration_segment])

        assert len(sequence.get_segments()) == segments_count + 2

    def test_add_coast_segment(
        self,
        sequence: Sequence,
        instant_condition: InstantCondition,
    ):
        segments_count: int = len(sequence.get_segments())

        sequence.add_coast_segment(instant_condition)

        assert len(sequence.get_segments()) == segments_count + 1

    def test_add_maneuver_segment(
        self,
        sequence: Sequence,
        instant_condition: InstantCondition,
        constant_thrust: ConstantThrust,
    ):
        segments_count: int = len(sequence.get_segments())

        sequence.add_maneuver_segment(instant_condition, constant_thrust)

        assert len(sequence.get_segments()) == segments_count + 1

    def test_solve(self, state: State, sequence: Sequence, segments: list[Segment]):
        solution = sequence.solve(state)

        assert len(solution.segment_solutions) == len(segments)

        assert solution.get_states() is not None
        assert solution.execution_is_complete
