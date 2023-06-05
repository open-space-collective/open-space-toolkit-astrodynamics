# Apache License 2.0

import pytest

from ostk.physics import Environment
from ostk.physics.units import Angle
from ostk.physics.units import Length
from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Interval
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.coordinate.spherical import AER

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics.trajectory import State


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def scale() -> Scale:
    return Scale.UTC


@pytest.fixture
def instant_1(scale: Scale) -> Instant:
    return Instant.date_time(DateTime(2023, 1, 8, 13, 45, 34), scale)


@pytest.fixture
def instant_2(scale: Scale) -> Instant:
    return Instant.date_time(DateTime(2023, 1, 8, 13, 46, 34), scale)


@pytest.fixture
def instant_3(scale: Scale) -> Instant:
    return Instant.date_time(DateTime(2023, 1, 8, 13, 47, 34), scale)


@pytest.fixture
def instant(instant_1: Instant) -> Instant:
    return instant_1


@pytest.fixture
def interval(instant_1: Instant, instant_2: Instant) -> Interval:
    return Interval(instant_1, instant_2, Interval.Type.Closed)


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def position(frame: Frame) -> Position:
    return Position.meters([7000000.0, 0.0, 0.0], frame)


@pytest.fixture
def velocity(frame: Frame) -> Velocity:
    return Velocity.meters_per_second([7600.0, 0.0, 0.0], frame)


@pytest.fixture
def state(instant: Instant, position: Position, velocity: Velocity) -> State:
    return State(instant, position, velocity)


@pytest.fixture
def latitude() -> Angle:
    return Angle(30.0, Angle.Unit.Degree)


@pytest.fixture
def longitude() -> Angle:
    return Angle(-111.0, Angle.Unit.Degree)


@pytest.fixture
def altitude() -> Length:
    return Length(10000.0, Length.Unit.Meter)


@pytest.fixture
def lla(latitude: Angle, longitude: Angle, altitude: Length) -> LLA:
    return LLA(latitude, longitude, altitude)


@pytest.fixture
def azimuth() -> Angle:
    return Angle(270.0, Angle.Unit.Degree)


@pytest.fixture
def elevation() -> Angle:
    return Angle(10.45, Angle.Unit.Degree)


@pytest.fixture
def range() -> Length:
    return Length(100000.0, Length.Unit.Meter)


@pytest.fixture
def aer(azimuth: Angle, elevation: Angle, range: Length) -> LLA:
    return AER(azimuth, elevation, range)


@pytest.fixture
def trajectory(position: Position) -> Trajectory:
    return Trajectory.position(position)
