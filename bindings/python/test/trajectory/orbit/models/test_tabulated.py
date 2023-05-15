# Apache License 2.0

import pytest

import numpy as np

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Scale
from ostk.physics.time import Duration
from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Velocity
from ostk.physics.coordinate import Frame
from ostk.physics import Environment

from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.orbit.models import Tabulated


@pytest.fixture
def earth():
    return Environment.default().access_celestial_object_with_name("Earth")


@pytest.fixture
def reference_states() -> list[State]:
    data = [
        [
            "2023-04-18T08:28:32.744064",
            -3310024.847608758,
            -6052528.239971979,
            7626.534641683145,
            -855.2088930507367,
            495.9541573743367,
            7536.520188134902,
        ],
        [
            "2023-04-18T08:28:33.744064",
            -3310878.047628086,
            -6052028.610429457,
            15163.0493833017,
            -851.1838953959017,
            503.31279367207185,
            7536.506322816037,
        ],
        [
            "2023-04-18T08:28:34.744064",
            -3311727.222133691,
            -6051521.622540367,
            22699.545682944226,
            -847.1578341126227,
            510.67087060642456,
            7536.483268627912,
        ],
        [
            "2023-04-18T08:28:35.744064",
            -3312572.370064364,
            -6051007.276868478,
            30236.014351773065,
            -843.1307141262514,
            518.0283791399937,
            7536.451025467619,
        ],
        [
            "2023-04-18T08:28:36.744064",
            -3313413.490363804,
            -6050485.573986613,
            37772.44620082258,
            -839.1025403636198,
            525.3853102358,
            7536.40959324377,
        ],
        [
            "2023-04-18T08:28:37.744064",
            -3314250.5819806806,
            -6049956.514476612,
            45308.83204108345,
            -835.0733177529921,
            532.7416548573802,
            7536.358971876497,
        ],
        [
            "2023-04-18T08:28:38.744064",
            -3315083.6438685474,
            -6049420.098929363,
            52845.16268340493,
            -831.0430512241197,
            540.0974039686765,
            7536.299161297429,
        ],
        [
            "2023-04-18T08:28:39.744064",
            -3315912.6749859042,
            -6048876.327944793,
            60381.42893867038,
            -827.0117457081434,
            547.4525485342174,
            7536.23016144972,
        ],
        [
            "2023-04-18T08:28:40.744064",
            -3316737.6742961914,
            -6048325.2021318525,
            67917.62161760827,
            -822.9794061376964,
            554.8070795189376,
            7536.1519722880375,
        ],
        [
            "2023-04-18T08:28:41.744064",
            -3317558.640767768,
            -6047766.722108539,
            75453.73153099201,
            -818.9460374467976,
            562.1609878883643,
            7536.064593778564,
        ],
        [
            "2023-04-18T08:28:42.744064",
            -3318375.5733739412,
            -6047200.888501875,
            82989.7494894997,
            -814.9116445709234,
            569.5142646084902,
            7535.968025898996,
        ],
        [
            "2023-04-18T08:28:43.744064",
            -3319188.471092942,
            -6046627.7019479135,
            90525.66630384693,
            -810.8762324469382,
            576.866900645898,
            7535.862268638544,
        ],
        [
            "2023-04-18T08:28:44.744064",
            -3319997.332907958,
            -6046047.163091751,
            98061.47278470133,
            -806.8398060131411,
            584.2188869676824,
            7535.747321997941,
        ],
        [
            "2023-04-18T08:28:45.744064",
            -3320802.157807084,
            -6045459.272587514,
            105597.15974279115,
            -802.8023702092072,
            591.5702145415504,
            7535.62318598943,
        ],
        [
            "2023-04-18T08:28:46.744064",
            -3321602.9447833803,
            -6044864.0310983565,
            113132.71798877101,
            -798.7639299762548,
            598.9208743356983,
            7535.48986063679,
        ],
        [
            "2023-04-18T08:28:47.744064",
            -3322399.692834845,
            -6044261.439296465,
            120668.13833342775,
            -794.7244902567405,
            606.2708573190106,
            7535.347345975296,
        ],
        [
            "2023-04-18T08:28:48.744064",
            -3323192.400964402,
            -6043651.497863061,
            128203.41158748553,
            -790.6840559945587,
            613.6201544608678,
            7535.195642051765,
        ],
        [
            "2023-04-18T08:28:49.744064",
            -3323981.068179951,
            -6043034.207488383,
            135738.52856182377,
            -786.6426321349277,
            620.9687567313651,
            7535.034748924513,
        ],
        [
            "2023-04-18T08:28:50.744064",
            -3324765.6934943097,
            -6042409.568871722,
            143273.48006733102,
            -782.6002236244674,
            628.3166551011666,
            7534.864666663388,
        ],
        [
            "2023-04-18T08:28:51.744064",
            -3325546.2759252544,
            -6041777.582721372,
            150808.25691495842,
            -778.5568354111685,
            635.6638405415614,
            7534.68539534976,
        ],
    ]
    return [
        State(
            instant=Instant.date_time(DateTime.parse(row[0]), Scale.UTC),
            position=Position.meters(
                [float(row[1]), float(row[2]), float(row[3])], Frame.GCRF()
            ),
            velocity=Velocity.meters_per_second(
                [float(row[4]), float(row[5]), float(row[6])], Frame.GCRF()
            ),
        )
        for row in data
    ]


@pytest.fixture
def test_states(reference_states: list[State]) -> list[State]:
    return reference_states[::4]


class TestTabulated:
    @pytest.mark.parametrize(
        "interpolation_type",
        (
            (Tabulated.InterpolationType.Linear),
            (Tabulated.InterpolationType.CubicSpline),
            (Tabulated.InterpolationType.BarycentricRational),
        ),
    )
    def test_constructor(
        self, test_states: list[State], interpolation_type: Tabulated.InterpolationType
    ):
        assert (
            Tabulated(
                states=test_states,
                initial_revolution_number=1,
                interpolation_type=interpolation_type,
            )
            is not None
        )

    def test_constructor_orbit_tabulated_sucess(
        self,
        test_states: list[State],
        earth,
    ):
        tabulated = Tabulated(
            states=test_states,
            initial_revolution_number=1,
            interpolation_type=Tabulated.InterpolationType.CubicSpline,
        )

        orbit: Orbit = Orbit(tabulated, earth)

        assert orbit is not None
        assert isinstance(orbit, Orbit)

    def test_get_interpolation_type(self, test_states: list[State]):
        tabulated = Tabulated(
            states=test_states,
            initial_revolution_number=1,
            interpolation_type=Tabulated.InterpolationType.CubicSpline,
        )

        assert (
            tabulated.get_interpolation_type() == Tabulated.InterpolationType.CubicSpline
        )

    @pytest.mark.parametrize(
        "interpolation_type,error_tolerance",
        (
            (Tabulated.InterpolationType.Linear, 420.0),
            (Tabulated.InterpolationType.CubicSpline, 5e-3),
            (Tabulated.InterpolationType.BarycentricRational, 5e-2),
        ),
    )
    def test_calculate_state_at_success(
        self,
        test_states: list[State],
        reference_states: list[State],
        interpolation_type: Tabulated.InterpolationType,
        error_tolerance: float,
    ):
        tabulated = Tabulated(
            states=test_states,
            initial_revolution_number=1,
            interpolation_type=interpolation_type,
        )

        for reference_state in reference_states:
            if not tabulated.get_interval().contains_instant(
                reference_state.get_instant()
            ):
                continue

            calculated_state: State = tabulated.calculate_state_at(
                reference_state.get_instant()
            )
            assert np.all(
                np.abs(
                    calculated_state.get_coordinates() - reference_state.get_coordinates()
                )
                < error_tolerance
            )

    @pytest.mark.parametrize(
        "interpolation_type,error_tolerance",
        (
            (Tabulated.InterpolationType.Linear, 420.0),
            (Tabulated.InterpolationType.CubicSpline, 5e-3),
            (Tabulated.InterpolationType.BarycentricRational, 5e-2),
        ),
    )
    def test_calculate_states_at_success(
        self,
        test_states: list[State],
        reference_states: list[State],
        interpolation_type: Tabulated.InterpolationType,
        error_tolerance: float,
    ):
        tabulated = Tabulated(
            states=test_states,
            initial_revolution_number=1,
            interpolation_type=interpolation_type,
        )

        states_within_interval: list[State] = [
            state
            for state in reference_states
            if tabulated.get_interval().contains_instant(state.get_instant())
        ]

        calculated_states: list[State] = tabulated.calculate_states_at(
            [state.get_instant() for state in states_within_interval]
        )

        for calculated_state, reference_state in zip(
            calculated_states, states_within_interval
        ):
            assert np.all(
                np.abs(
                    calculated_state.get_coordinates() - reference_state.get_coordinates()
                )
                < error_tolerance
            )

    @pytest.mark.parametrize(
        "interpolation_type",
        (
            (Tabulated.InterpolationType.Linear),
            (Tabulated.InterpolationType.CubicSpline),
            (Tabulated.InterpolationType.BarycentricRational),
        ),
    )
    def test_calculate_state_at_failure(
        self, test_states: list[State], interpolation_type: Tabulated.InterpolationType
    ):
        tabulated = Tabulated(
            states=test_states,
            initial_revolution_number=1,
            interpolation_type=interpolation_type,
        )

        with pytest.raises(Exception):
            tabulated.calculate_state_at(
                test_states[0].get_instant() - Duration.seconds(1)
            )

        with pytest.raises(Exception):
            tabulated.calculate_state_at(
                test_states[-1].get_instant() + Duration.seconds(1)
            )
