# Apache License 2.0

import pytest

import pathlib

from ostk.core.filesystem import Path
from ostk.core.filesystem import File


@pytest.fixture
def data_directory_path() -> str:
    return f"{pathlib.Path(__file__).parent.absolute()}/data"


@pytest.fixture
def opm_file(data_directory_path: str) -> File:
    return File.path(Path.parse(f"{data_directory_path}/opm_1.yaml"))
