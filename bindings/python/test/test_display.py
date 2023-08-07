# Apache License 2.0

from ostk.mathematics.objects import RealInterval

from ostk.physics import Environment
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.environment.objects import Celestial
from ostk.physics.time import DateTime
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.time import Interval
from ostk.physics.time import Scale
from ostk.physics.units import Length
from ostk.physics.units import Angle

from ostk.astrodynamics import Trajectory
from ostk.astrodynamics import display
from ostk.astrodynamics.access import Generator as AccessGenerator
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory.orbit.models import SGP4
from ostk.astrodynamics.trajectory.orbit.models.sgp4 import TLE


class TestDisplay:
    def test_accesses_plot(self, state: State):
        start_instant: Instant = Instant.date_time(
            DateTime(2023, 1, 3, 0, 0, 0),
            Scale.UTC,
        )
        duration: Duration = Duration.days(7.0)
        step: Duration = Duration.seconds(10.0)
        tolerance: Duration = Duration.seconds(1.0)

        ground_station_lla: LLA = LLA(
            Angle.degrees(70.0),
            Angle.degrees(160.0),
            Length.meters(0.0),
        )

        tle_1 = TLE(
            "1 55076U 23001BV  23146.17959645  .00004328  00000-0  23719-3 0  9993",
            "2 55076  97.4793 205.9529 0016244  89.9523 270.3571 15.14609100 21723",
        )

        tle_2 = TLE(
            "1 48915U 21059AN  23146.32782040  .00004955  00000-0  24678-3 0  9999",
            "2 48915  97.5954 279.7041 0010303 354.9434   5.1694 15.18004448105867",
        )

        environment: Environment = Environment.default()
        earth: Celestial = environment.access_celestial_object_with_name("Earth")

        ground_station_trajectory: Trajectory = Trajectory.position(
            Position.meters(
                ground_station_lla.to_cartesian(
                    earth.get_equatorial_radius(),
                    earth.get_flattening(),
                ),
                Frame.ITRF(),
            ),
        )

        stop_instant: Instant = start_instant + duration
        search_interval: RealInterval = Interval.closed(start_instant, stop_instant)

        orbit_1: Orbit = Orbit(SGP4(tle_1), earth)
        orbit_2: Orbit = Orbit(SGP4(tle_2), earth)

        generator = AccessGenerator(
            environment=environment,
            step=step,
            tolerance=tolerance,
        )

        accesses_1 = generator.compute_accesses(
            interval=search_interval,
            from_trajectory=ground_station_trajectory,
            to_trajectory=orbit_1,
        )

        assert len(accesses_1) > 0

        accesses_2 = generator.compute_accesses(
            interval=search_interval,
            from_trajectory=ground_station_trajectory,
            to_trajectory=orbit_2,
        )

        assert len(accesses_2) > 0

        accesses_plot = display.AccessesPlot(
            earth=earth,
            interval=search_interval,
            trajectory_step=Duration.minutes(5.0),
            access_step=Duration.seconds(10.0),
            ground_station_lla=ground_station_lla,
            color="green",
        )

        accesses_plot.add_satellite(
            trajectory=orbit_1,
            accesses=accesses_1,
            rgb=[180, 0, 0],
        )

        accesses_plot.add_satellite(
            trajectory=orbit_2,
            accesses=accesses_2,
            rgb=[0, 0, 180],
        )

        accesses_plot.show()
