# Apache License 2.0

import pytest

import numpy as np

from ostk.mathematics.curve_fitting import Interpolator

from ostk.physics.time import Instant
from ostk.physics.time import DateTime
from ostk.physics.time import Duration
from ostk.physics.time import Scale
from ostk.physics.coordinate import Frame

from ostk.astrodynamics.trajectory.state import CoordinateSubset
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianVelocity
from ostk.astrodynamics import Dynamics
from ostk.astrodynamics.dynamics import Tabulated


@pytest.fixture
def instants() -> list[Instant]:
    return [
        Instant.date_time(DateTime(2021, 3, 20, 12, 0, 0), Scale.UTC),
        Instant.date_time(DateTime(2021, 3, 20, 12, 0, 1), Scale.UTC),
        Instant.date_time(DateTime(2021, 3, 20, 12, 0, 2), Scale.UTC),
        Instant.date_time(DateTime(2021, 3, 20, 12, 0, 3), Scale.UTC),
        Instant.date_time(DateTime(2021, 3, 20, 12, 0, 4), Scale.UTC),
        Instant.date_time(DateTime(2021, 3, 20, 12, 0, 5), Scale.UTC),
        Instant.date_time(DateTime(2021, 3, 20, 12, 0, 6), Scale.UTC),
        Instant.date_time(DateTime(2021, 3, 20, 12, 0, 7), Scale.UTC),
        Instant.date_time(DateTime(2021, 3, 20, 12, 1, 8), Scale.UTC),
        Instant.date_time(DateTime(2021, 3, 20, 12, 1, 9), Scale.UTC),
    ]


@pytest.fixture
def contribution_profile() -> np.ndarray:
    return np.array(
        [
            [1.0, 2.0, 3.0, -1.0],
            [4.0, 5.0, 6.0, -1.0],
            [7.0, 8.0, 9.0, -1.0],
            [10.0, 11.0, 12.0, -1.0],
            [13.0, 14.0, 15.0, -1.0],
            [16.0, 17.0, 18.0, -1.0],
            [19.0, 20.0, 21.0, -1.0],
            [22.0, 23.0, 24.0, -1.0],
            [25.0, 26.0, 27.0, -1.0],
            [28.0, 29.0, 30.0, -1.0],
        ]
    )


@pytest.fixture
def coordinate_subsets() -> list[CoordinateSubset]:
    return [
        CartesianVelocity.default(),
        CoordinateSubset.mass(),
    ]


@pytest.fixture
def frame() -> Frame:
    return Frame.GCRF()


@pytest.fixture
def dynamics(
    instants: list[Instant],
    contribution_profile: np.ndarray,
    coordinate_subsets: list[CoordinateSubset],
    frame: Frame,
) -> Tabulated:
    return Tabulated(
        instants,
        contribution_profile,
        coordinate_subsets,
        frame,
        Interpolator.Type.Linear,
    )


class TestTabulated:
    def test_constructor(
        self,
        dynamics: Tabulated,
    ):
        assert dynamics is not None
        assert isinstance(dynamics, Tabulated)
        assert isinstance(dynamics, Dynamics)
        assert dynamics.is_defined()

    def test_accessor(
        self,
        dynamics: Tabulated,
        instants: list[Instant],
        contribution_profile: np.ndarray,
        frame: Frame,
    ):
        assert np.array_equal(
            dynamics.access_contribution_profile(), contribution_profile
        )

        assert dynamics.access_instants() == instants

        assert dynamics.access_frame() == frame

    def test_getters(
        self,
        dynamics: Tabulated,
        contribution_profile: np.ndarray,
    ):
        assert np.array_equal(
            dynamics.get_contribution_profile_from_coordinate_subsets(
                [CartesianVelocity.default()]
            ),
            contribution_profile[:, 0:3],
        )

        assert dynamics.get_interpolation_type() == Interpolator.Type.Linear

    def test_compute_contribution(
        self,
        dynamics: Tabulated,
        instants: list[Instant],
        contribution_profile: np.ndarray,
    ):
        for i in range(len(instants)):
            contribution = dynamics.compute_contribution(instants[i], [], Frame.GCRF())
            assert contribution == pytest.approx(contribution_profile[i])

            new_contribution = dynamics.compute_contribution(
                instants[i] + Duration.seconds(0.5), [], Frame.GCRF()
            )

            assert new_contribution is not None
            assert len(new_contribution) == 4
