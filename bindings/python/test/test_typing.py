# Apache License 2.0

"""Calls that the bindings accept, and that the generated type stubs must accept too.

The bindings convert some Python values implicitly, for example a `str` where a `String` is
declared, or a list of floats where a vector is declared. Each test makes such a call.
`test_stubs_accept_these_calls` type-checks this file with mypy against the generated stubs,
so a stub that rejects one of these calls fails.
"""

from pathlib import Path

import numpy as np
import pytest

import ostk.astrodynamics
from ostk.mathematics.geometry import Angle as MathematicsAngle
from ostk.physics.coordinate import Frame
from ostk.physics.time import DateTime
from ostk.physics.time import Instant
from ostk.physics.time import Scale
from ostk.physics.unit import Length
from ostk.astrodynamics.trajectory import State
from ostk.astrodynamics.trajectory import StateBuilder
from ostk.astrodynamics.trajectory.orbit.model.kepler import COE
from ostk.astrodynamics.trajectory.orbit.model.sgp4 import TLE
from ostk.astrodynamics.trajectory.state.coordinate_subset import CartesianPosition


def test_string_parameters_accept_str() -> None:
    tle = TLE(
        "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537",
    )

    assert isinstance(tle, TLE)


def test_vector_parameter_accepts_list() -> None:
    instant = Instant.date_time(DateTime(2024, 1, 2, 3, 4, 5), Scale.UTC)

    state: State = StateBuilder(Frame.GCRF(), [CartesianPosition.default()]).build(
        instant, [1.0, 2.0, 3.0]
    )
    coordinates: np.ndarray = state.get_coordinates()

    assert coordinates.tolist() == [1.0, 2.0, 3.0]


def test_sequence_parameter_accepts_list_of_derived_class() -> None:
    # `list` is invariant, `Sequence` is covariant: a list of CartesianPosition is accepted
    # where a sequence of CoordinateSubset is declared.
    coordinate_subsets: list[CartesianPosition] = [CartesianPosition.default()]

    assert isinstance(StateBuilder(Frame.GCRF(), coordinate_subsets), StateBuilder)


def test_angle_parameters_accept_mathematics_angle() -> None:
    coe = COE(
        Length.kilometers(7000.0),
        0.001,
        MathematicsAngle.degrees(98.0),
        MathematicsAngle.degrees(10.0),
        MathematicsAngle.degrees(20.0),
        MathematicsAngle.degrees(30.0),
    )

    assert isinstance(coe, COE)


def test_stubs_accept_these_calls(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    mypy_api = pytest.importorskip("mypy.api")

    # In the CMake build tree, the package is a directory next to `test`, not an installed
    # distribution: mypy only finds it through MYPYPATH.
    package_root: Path = Path(ostk.astrodynamics.__file__).resolve().parents[2]

    if package_root.name != "site-packages":
        monkeypatch.setenv("MYPYPATH", str(package_root))

    # Report the errors in the calls of this file only: the stubs themselves are not checked,
    # as for an installed package.
    config: Path = tmp_path / "mypy.ini"
    config.write_text("[mypy]\n[mypy-ostk.*]\nignore_errors = True\n")

    stdout, stderr, exit_status = mypy_api.run(
        [
            __file__,
            "--config-file",
            str(config),
            "--cache-dir",
            str(tmp_path / "mypy_cache"),
        ]
    )

    assert exit_status == 0, stdout + stderr
