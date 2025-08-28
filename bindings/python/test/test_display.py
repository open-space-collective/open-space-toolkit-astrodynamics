# Apache License 2.0

import pytest

from ostk.physics import Environment
from ostk.physics.coordinate import Position
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.environment.object import Celestial
from ostk.physics.time import DateTime
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Scale
from ostk.physics.unit import Length
from ostk.physics.unit import Angle

from ostk.astrodynamics import display
from ostk.astrodynamics.access import Generator as AccessGenerator
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.orbit.model import SGP4
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import TLE
from ostk.astrodynamics import Access
from ostk.astrodynamics.access import AccessTarget
from ostk.astrodynamics.access import VisibilityCriterion


@pytest.fixture
def environment() -> Environment:
    return Environment.default()


@pytest.fixture
def earth(environment: Environment) -> Celestial:
    return environment.access_celestial_object_with_name("Earth")


@pytest.fixture
def search_interval() -> Interval:
    start_instant: Instant = Instant.date_time(
        DateTime(2023, 1, 3, 0, 0, 0),
        Scale.UTC,
    )
    duration: Duration = Duration.hours(1.0)
    stop_instant: Instant = start_instant + duration
    return Interval.closed(start_instant, stop_instant)


@pytest.fixture
def access_plot() -> display.AccessesPlot:
    return display.AccessesPlot(
        earth=earth,
        interval=search_interval,
        trajectory_step=Duration.minutes(5.0),
        access_step=Duration.seconds(10.0),
    )


@pytest.fixture
def satellite_1_trajectory(earth: Celestial) -> Orbit:
    tle_1 = TLE(
        "1 55076U 23001BV  23146.17959645  .00004328  00000-0  23719-3 0  9993",
        "2 55076  97.4793 205.9529 0016244  89.9523 270.3571 15.14609100 21723",
    )
    return Orbit(SGP4(tle_1), earth)


@pytest.fixture
def satellite_2_trajectory(earth: Celestial) -> Orbit:
    tle_2 = TLE(
        "1 48915U 21059AN  23146.32782040  .00004955  00000-0  24678-3 0  9999",
        "2 48915  97.5954 279.7041 0010303 354.9434   5.1694 15.18004448105867",
    )
    return Orbit(SGP4(tle_2), earth)


@pytest.fixture
def ground_station_lla() -> LLA:
    return LLA(
        Angle.degrees(70.0),
        Angle.degrees(160.0),
        Length.meters(0.0),
    )


@pytest.fixture
def access_target(
    environment: Environment,
    ground_station_lla: LLA,
) -> AccessTarget:
    return AccessTarget.from_position(
        VisibilityCriterion.from_line_of_sight(environment),
        Position.from_lla(
            ground_station_lla,
            environment.access_celestial_object_with_name("Earth"),
        ),
    )


@pytest.fixture
def access_generator(environment: Environment) -> AccessGenerator:
    return AccessGenerator(
        environment=environment,
        step=Duration.seconds(10.0),
        tolerance=Duration.seconds(1.0),
    )


@pytest.fixture
def satellite_1_accesses(
    satellite_1_trajectory: Orbit,
    environment: Environment,
    search_interval: Interval,
    access_target: AccessTarget,
    access_generator: AccessGenerator,
) -> list[Access]:
    return access_generator.compute_accesses(
        interval=search_interval,
        access_target=access_target,
        to_trajectory=satellite_1_trajectory,
    )


@pytest.fixture
def satellite_2_accesses(
    satellite_2_trajectory: Orbit,
    environment: Environment,
    search_interval: Interval,
    access_target: AccessTarget,
    access_generator: AccessGenerator,
) -> list[Access]:
    return access_generator.compute_accesses(
        interval=search_interval,
        access_target=access_target,
        to_trajectory=satellite_2_trajectory,
    )


class TestDisplay:
    def test_accesses_plot(
        self,
        environment: Environment,
        earth: Celestial,
        search_interval: Interval,
        access_target: AccessTarget,
        ground_station_lla: LLA,
        satellite_1_trajectory: Orbit,
        satellite_2_trajectory: Orbit,
        satellite_1_accesses: list[Access],
        satellite_2_accesses: list[Access],
    ):

        accesses_plot = display.AccessesPlot(
            earth=earth,
            interval=search_interval,
            trajectory_step=Duration.minutes(5.0),
            access_step=Duration.seconds(10.0),
        )

        accesses_plot.add_satellite(
            trajectory=satellite_1_trajectory,
            rgb=[180, 0, 0],
        )

        accesses_plot.add_satellite(
            trajectory=satellite_2_trajectory,
            rgb=[0, 0, 180],
        )

        accesses_plot.add_accesses(
            trajectory=satellite_1_trajectory,
            accesses=satellite_1_accesses,
            rgb=[180, 0, 0],
        )

        accesses_plot.add_accesses(
            trajectory=satellite_2_trajectory,
            accesses=satellite_2_accesses,
            rgb=[0, 0, 180],
        )

        accesses_plot.add_ground_station(
            ground_station_lla,
            color="green",
        )

    def test_accesses_plot_backwards_compatibility(
        self,
        environment: Environment,
        earth: Celestial,
        search_interval: Interval,
        access_target: AccessTarget,
        satellite_1_trajectory: Orbit,
        satellite_2_trajectory: Orbit,
        satellite_1_accesses: list[Access],
        satellite_2_accesses: list[Access],
        ground_station_lla: LLA,
    ):
        with pytest.warns(
            DeprecationWarning,
            match="Ground station and color are deprecated",
        ):
            accesses_plot = display.AccessesPlot(
                earth=earth,
                interval=search_interval,
                trajectory_step=Duration.minutes(5.0),
                access_step=Duration.seconds(10.0),
                ground_station_lla=ground_station_lla,
                color="green",
            )

        with pytest.warns(
            DeprecationWarning,
            match="Passing accesses to add_satellite\(\) is deprecated.*",
        ):
            accesses_plot.add_satellite(
                trajectory=satellite_1_trajectory,
                accesses=satellite_1_accesses,
                rgb=[180, 0, 0],
            )

            accesses_plot.add_satellite(
                trajectory=satellite_2_trajectory,
                accesses=satellite_2_accesses,
                rgb=[0, 0, 180],
            )
