# Apache License 2.0

import pytest

import math

import numpy as np

from ostk.mathematics.curve_fitting import Interpolator
from ostk.mathematics.geometry.d3.object import Cuboid
from ostk.mathematics.geometry.d3.object import Composite
from ostk.mathematics.geometry.d3.object import Point

from ostk.physics import Environment
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.environment.object.celestial import Earth, Sun
from ostk.physics.environment.gravitational import Earth as EarthGravitationalModel
from ostk.physics.environment.magnetic import Earth as EarthMagneticModel
from ostk.physics.environment.atmospheric import Earth as EarthAtmosphericModel
from ostk.physics.unit import Mass
from ostk.physics.unit import Length
from ostk.physics.unit import Angle

from ostk.astrodynamics.trajectory import LocalOrbitalFrameFactory
from ostk.astrodynamics.trajectory import LocalOrbitalFrameDirection

from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
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
from ostk.astrodynamics.trajectory.state import CoordinateSubset, CoordinateBroker
from ostk.astrodynamics.trajectory.state.coordinate_subset import (
    CartesianPosition,
    CartesianVelocity,
)
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import Propagator
from ostk.astrodynamics.trajectory.orbit.model import SGP4
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import TLE
from ostk.astrodynamics.trajectory.orbit.model import Tabulated
from ostk.astrodynamics.trajectory.orbit.model import Propagated
from ostk.astrodynamics.trajectory.orbit.model import Kepler
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE


from .memory_leak_tracker import track_memory_usage


@pytest.fixture
def epoch() -> Instant:
    return Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)


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
def state(
    epoch: Instant,
    satellite_system: SatelliteSystem,
    coordinate_broker_7d: CoordinateBroker,
) -> State:

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

    return State(epoch, coordinates, Frame.GCRF(), coordinate_broker_7d)


@pytest.fixture
def central_body_gravity() -> CentralBodyGravity:
    return CentralBodyGravity(Earth.WGS84(2, 0))


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
        NumericalSolver.StepperType.RungeKuttaDopri5,
        5.0,
        1.0e-15,
        1.0e-15,
    )


@pytest.fixture
def propagator(numerical_solver: NumericalSolver, dynamics: list[Dynamics]) -> Propagator:
    return Propagator(numerical_solver, dynamics)


@pytest.fixture
def tle() -> TLE:
    return TLE(
        "1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994",
        "2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316",
    )


@pytest.fixture
def sgp4_model(tle: TLE) -> SGP4:
    return SGP4(tle)


@pytest.fixture
def kepler_model(earth: Earth) -> Kepler:
    a = Length.kilometers(7000.0)
    e = 0.1
    i = Angle.degrees(35.0)
    raan = Angle.degrees(40.0)
    aop = Angle.degrees(50.0)
    nu = Angle.degrees(60.0)

    coe = COE(a, e, i, raan, aop, nu)

    epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

    return Kepler(coe, epoch, earth, Kepler.PerturbationType.No)


@pytest.fixture
def propagated_model(propagator: Propagator, state: State) -> Propagated:
    return Propagated(propagator, state, 5)


def get_state_vec(time: float) -> np.ndarray:
    return np.array([math.sin(time), math.cos(time)])


@pytest.fixture
def oscillator_initial_state(
    epoch: Instant,
) -> State:
    return State(epoch, get_state_vec(0.0), Frame.GCRF(), [CoordinateSubset("Subset", 2)])


class TestMemoryLeak:
    def test_leak_SGP4(
        self,
        tle: TLE,
        num_instants: float = 100000.0,
    ):
        epoch = Instant.date_time(DateTime(2018, 1, 1, 0, 0, 0), Scale.UTC)

        instants: list[Instant] = Interval.closed(
            epoch, epoch + Duration.minutes(num_instants)
        ).generate_grid(Duration.seconds(60.0))

        @track_memory_usage
        def call_function_of_interest():
            model = SGP4(tle)

        call_function_of_interest()

        # for instant in instants:
        #     model.calculate_state_at(instant)

    def test_leak_propagator(
        self,
        epoch: Instant,
        tle: TLE,
        sgp4_model: SGP4,
        kepler_model: Kepler,
        state: State,
        propagator: Propagator,
        propagated_model: Propagated,
        num_instants: float = 100.0,
    ):

        interval = Interval.closed(epoch, epoch + Duration.minutes(num_instants))
        instants = interval.generate_grid(Duration.seconds(60.0))

        # @track_memory_usage
        # def call_function_of_interest():
        #     interval.generate_grid(Duration.seconds(60.0))

        # @track_memory_usage
        # def call_function_of_interest():
        #     for instant in instants:
        #         sgp4_model.calculate_state_at(instant)

        # @track_memory_usage
        # def call_function_of_interest():
        #     for instant in instants:
        #         kepler_model.calculate_state_at(instant)

        # @track_memory_usage
        # def call_function_of_interest():
        #     for instant in instants:
        #         propagator.calculate_state_at(state, instant)
        @track_memory_usage
        def call_function_of_interest():
            propagator.calculate_states_at(state, instants)

        # @track_memory_usage
        # def call_function_of_interest():
        #     for instant in instants:
        #         propagated_model.calculate_state_at(instant)
        # @track_memory_usage
        # def call_function_of_interest():
        #     propagated_model.calculate_states_at(instants)

        call_function_of_interest()

    def test_leak_numerical_solver(
        self,
        epoch: Instant,
        state: State,
        num_instants: float = 1000.0,
    ):

        mu = 3.986004415e14

        interval = Interval.closed(epoch, epoch + Duration.minutes(num_instants))
        instants = interval.generate_grid(Duration.seconds(60.0))

        numerical_solver = NumericalSolver(
            NumericalSolver.LogType.NoLog,
            NumericalSolver.StepperType.RungeKuttaDopri5,
            5.0,
            1.0e-15,
            1.0e-15,
        )

        def central_body_dynamic(x, dxdt, _):
            position_mag = math.sqrt(x[0] ** 2 + x[1] ** 2 + x[2] ** 2)

            dxdt[0] = x[3]
            dxdt[1] = x[4]
            dxdt[2] = x[5]
            dxdt[3] = -mu * x[0] / position_mag**3
            dxdt[4] = -mu * x[1] / position_mag**3
            dxdt[5] = -mu * x[2] / position_mag**3
            return dxdt

        @track_memory_usage
        def call_function_of_interest():
            numerical_solver.integrate_time(state, instants, central_body_dynamic)

        call_function_of_interest()
