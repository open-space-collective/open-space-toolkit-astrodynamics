# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.curve_fitting import Interpolator
from ostk.mathematics.geometry.d3.object import Cuboid
from ostk.mathematics.geometry.d3.object import Composite
from ostk.mathematics.geometry.d3.object import Point

from ostk.physics import Environment
from ostk.physics.unit import Mass
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.coordinate import Frame
from ostk.physics.environment.object.celestial import Earth, Sun
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.environment.magnetic import Earth as EarthMagneticModel
from ostk.physics.environment.atmospheric import Earth as EarthAtmosphericModel

from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import LocalOrbitalFrameDirection
from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianAcceleration
from ostk.astrodynamics.trajectory.state import CoordinateBroker
from ostk.astrodynamics.trajectory.state import NumericalSolver
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics import Thruster
from ostk.astrodynamics.dynamics import CentralBodyGravity
from ostk.astrodynamics.dynamics import PositionDerivative
from ostk.astrodynamics.dynamics import AtmosphericDrag
from ostk.astrodynamics.event_condition import InstantCondition
from ostk.astrodynamics.guidance_law import ConstantThrust
from ostk.astrodynamics.flight import Maneuver
from ostk.astrodynamics.flight.system import PropulsionSystem
from ostk.astrodynamics.flight.system import SatelliteSystem
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Propagator


@pytest.fixture
def propulsion_system() -> PropulsionSystem:
    return PropulsionSystem(
        1.0,
        150.0,
    )


@pytest.fixture
def satellite_system(propulsion_system: PropulsionSystem) -> SatelliteSystem:
    mass = Mass(90.0, Mass.Unit.Kilogram)
    satellite_geometry = Composite(
        Cuboid(
            Point(0.0, 0.0, 0.0),
            [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],
            [1.0, 0.0, 0.0],
        )
    )
    inertia_tensor = np.identity(3)
    surface_area = 0.8
    drag_coefficient = 0.0

    return SatelliteSystem(
        mass,
        satellite_geometry,
        inertia_tensor,
        surface_area,
        drag_coefficient,
        propulsion_system,
    )


@pytest.fixture
def earth() -> Earth:
    return Earth.from_models(
        EarthGravitationalModel(EarthGravitationalModel.Type.EGM96),
        EarthMagneticModel(EarthMagneticModel.Type.Undefined),
        EarthAtmosphericModel(EarthAtmosphericModel.Type.Exponential),
    )


@pytest.fixture
def environment(earth) -> Environment:
    sun = Sun.default()

    return Environment(Instant.J2000(), [earth, sun])


@pytest.fixture
def coordinate_broker_7d():
    return CoordinateBroker(
        [
            CartesianPosition.default(),
            CartesianVelocity.default(),
            CoordinateSubset.mass(),
        ]
    )


@pytest.fixture
def coordinate_broker_9d():
    return CoordinateBroker(
        [
            CartesianPosition.default(),
            CartesianVelocity.default(),
            CoordinateSubset.mass(),
            CoordinateSubset.surface_area(),
            CoordinateSubset.drag_coefficient(),
        ]
    )


@pytest.fixture
def state(
    satellite_system: SatelliteSystem, coordinate_broker_7d: CoordinateBroker
) -> State:
    instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    propellant_mass: float = 10.0

    coordinates: list = [
        7500000.0,
        0.0,
        0.0,
        0.0,
        5335.865450622126,
        5335.865450622126,
        satellite_system.get_mass().in_kilograms() + propellant_mass,
    ]

    return State(instant, coordinates, Frame.GCRF(), coordinate_broker_7d)


@pytest.fixture
def state_low_altitude(
    satellite_system: SatelliteSystem, coordinate_broker_9d: CoordinateBroker
) -> State:
    instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    propellant_mass: float = 10.0
    area: float = satellite_system.get_cross_sectional_surface_area()
    cd: float = satellite_system.get_drag_coefficient()

    coordinates: list = [
        7000000.0,
        0.0,
        0.0,
        0.0,
        5335.865450622126,
        5335.865450622126,
        satellite_system.get_mass().in_kilograms() + propellant_mass,
        area,
        cd,
    ]

    return State(instant, coordinates, Frame.GCRF(), coordinate_broker_9d)


@pytest.fixture
def central_body_gravity() -> CentralBodyGravity:
    return CentralBodyGravity(Earth.WGS84(20, 0))


@pytest.fixture
def atmospheric_drag(environment, satellite_system) -> AtmosphericDrag:
    return AtmosphericDrag(environment.access_celestial_object_with_name("Earth"))


@pytest.fixture
def position_derivative() -> PositionDerivative:
    return PositionDerivative()


@pytest.fixture
def local_orbital_frame_direction() -> LocalOrbitalFrameDirection:
    return LocalOrbitalFrameDirection(
        [1.0, 0.0, 0.0],
        LocalOrbitalFrameFactory.VNC(Frame.GCRF()),
    )


@pytest.fixture
def thrust_dynamics(
    satellite_system: SatelliteSystem,
    local_orbital_frame_direction: LocalOrbitalFrameDirection,
) -> Thruster:
    return Thruster(
        satellite_system=satellite_system,
        guidance_law=ConstantThrust(local_orbital_frame_direction),
    )


@pytest.fixture
def dynamics(
    position_derivative: PositionDerivative,
    central_body_gravity: CentralBodyGravity,
) -> list:
    return [position_derivative, central_body_gravity]


@pytest.fixture
def numerical_solver() -> NumericalSolver:
    return NumericalSolver(
        NumericalSolver.LogType.NoLog,
        NumericalSolver.StepperType.RungeKuttaFehlberg78,
        5.0,
        1.0e-15,
        1.0e-15,
    )


@pytest.fixture
def conditional_numerical_solver() -> NumericalSolver:
    return NumericalSolver(
        NumericalSolver.LogType.NoLog,
        NumericalSolver.StepperType.RungeKuttaDopri5,
        5.0,
        1.0e-15,
        1.0e-15,
    )


@pytest.fixture
def event_condition(state: State) -> InstantCondition:
    return InstantCondition(
        InstantCondition.Criterion.StrictlyPositive,
        state.get_instant() + Duration.seconds(42.0),
    )


@pytest.fixture
def maneuver_states(frame: Frame) -> list[State]:
    return [
        State(
            Instant.J2000() + Duration.seconds(float(i)),
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, -1e-3],
            frame,
            [
                CartesianPosition.default(),
                CartesianVelocity.default(),
                CartesianAcceleration.thrust_acceleration(),
                CoordinateSubset.mass_flow_rate(),
            ],
        )
        for i in range(0, 100, 10)
    ]


@pytest.fixture
def maneuver(
    maneuver_states: list[State],
) -> Maneuver:
    return Maneuver(states=maneuver_states)


@pytest.fixture
def propagator(numerical_solver: NumericalSolver, dynamics: list[Dynamics]) -> Propagator:
    return Propagator(numerical_solver, dynamics)


@pytest.fixture
def propagator_with_maneuvers(
    numerical_solver: NumericalSolver,
    dynamics: list[Dynamics],
    maneuver: Maneuver,
) -> Propagator:
    return Propagator(numerical_solver, dynamics, [maneuver], Interpolator.Type.Linear)


class TestPropagator:
    def test_constructors(
        self, propagator: Propagator, propagator_with_maneuvers: Propagator
    ):
        assert propagator is not None
        assert isinstance(propagator, Propagator)
        assert propagator.is_defined()

        assert propagator_with_maneuvers is not None
        assert isinstance(propagator_with_maneuvers, Propagator)
        assert propagator_with_maneuvers.is_defined()

    def test_comparators(
        self, propagator: Propagator, propagator_with_maneuvers: Propagator
    ):
        assert propagator == propagator
        assert propagator != propagator_with_maneuvers

    def test_access_numerical_solver(
        self, propagator: Propagator, numerical_solver: NumericalSolver
    ):
        assert propagator.access_numerical_solver() == numerical_solver

    def test_get_dynamics(self, propagator: Propagator, dynamics: list):
        assert propagator.get_dynamics() == dynamics

    def test_set_dynamics(self, propagator: Propagator, dynamics: list):
        assert len(propagator.get_dynamics()) == 2

        propagator.set_dynamics(dynamics + dynamics)

        assert len(propagator.get_dynamics()) == 4

    def test_add_dynamics(
        self, propagator: Propagator, central_body_gravity: CentralBodyGravity
    ):
        assert len(propagator.get_dynamics()) == 2

        propagator.add_dynamics(central_body_gravity)
        propagator.add_dynamics(central_body_gravity)

        assert len(propagator.get_dynamics()) == 4

    def test_clear_dynamics(self, propagator: Propagator):
        assert len(propagator.get_dynamics()) >= 1

        propagator.clear_dynamics()

        assert len(propagator.get_dynamics()) == 0

    def test_add_maneuver(
        self,
        propagator: Propagator,
        maneuver: Maneuver,
    ):
        assert len(propagator.get_dynamics()) == 2

        propagator.add_maneuver(maneuver)

        assert len(propagator.get_dynamics()) == 3

    def test_calculate_state_at(self, propagator: Propagator, state: State):
        instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC)

        propagator_state = propagator.calculate_state_at(state, instant)

        propagator_state_position_ref = np.array(
            [6265892.25765909, 3024770.94961259, 3024359.72137468]
        )
        propagator_state_velocity_ref = np.array(
            [-3974.49168221, 4468.16996776, 4466.19232746]
        )

        propagator_state_position = propagator_state.get_position().get_coordinates()
        propagator_state_velocity = propagator_state.get_velocity().get_coordinates()

        assert all(
            [
                round(propagator_state_position[i], 8)
                == round(propagator_state_position_ref[i], 8)
                for i in range(0, len(propagator_state_position_ref))
            ]
        )
        assert all(
            [
                round(propagator_state_velocity[i], 8)
                == round(propagator_state_velocity_ref[i], 8)
                for i in range(0, len(propagator_state_velocity_ref))
            ]
        )
        assert propagator_state.get_instant() == instant

    def test_calculate_state_to_condition(
        self,
        conditional_numerical_solver: NumericalSolver,
        dynamics: list[Dynamics],
        state: State,
        event_condition: InstantCondition,
    ):
        propagator: Propagator = Propagator(conditional_numerical_solver, dynamics)

        instant: Instant = Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC)

        solution = propagator.calculate_state_to_condition(
            state=state,
            instant=instant,
            event_condition=event_condition,
        )

        assert solution.condition_is_satisfied
        assert pytest.approx(42.0, abs=1e-3) == float(
            (solution.state.get_instant() - state.get_instant()).in_seconds()
        )

    def test_calculate_states_at(self, propagator: Propagator, state: State):
        instant_array = [
            Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 20, 0), Scale.UTC),
        ]

        _ = propagator.calculate_states_at(state, instant_array)

        with pytest.raises(RuntimeError):
            instant_array.reverse()
            propagator.calculate_states_at(state, instant_array)

    def test_calculate_states_at_with_drag(
        self,
        numerical_solver: NumericalSolver,
        dynamics: list[Dynamics],
        atmospheric_drag: AtmosphericDrag,
        state_low_altitude: State,
    ):
        propagator: Propagator = Propagator(
            numerical_solver,
            dynamics + [atmospheric_drag],
        )

        instant_array = [
            Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 20, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 30, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 40, 0), Scale.UTC),
        ]

        _ = propagator.calculate_states_at(state_low_altitude, instant_array)

    def test_calculate_states_at_with_thrust(
        self,
        numerical_solver: NumericalSolver,
        dynamics: list[Dynamics],
        thrust_dynamics: Thruster,
        state: State,
    ):
        propagator: Propagator = Propagator(
            numerical_solver,
            dynamics + [thrust_dynamics],
        )

        instant_array = [
            Instant.date_time(DateTime(2018, 1, 1, 0, 10, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 20, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 30, 0), Scale.UTC),
            Instant.date_time(DateTime(2018, 1, 1, 0, 40, 0), Scale.UTC),
        ]

        _ = propagator.calculate_states_at(state, instant_array)

    def test_from_environment(
        self,
        numerical_solver: NumericalSolver,
        environment: Environment,
    ):
        assert Propagator.from_environment(numerical_solver, environment) is not None

    def test_default(self, environment: Environment):
        assert Propagator.default()
        assert Propagator.default(environment) is not None
