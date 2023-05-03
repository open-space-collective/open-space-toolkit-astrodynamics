#!/bin/bash

# Copyright © Loft Orbital Solutions Inc.

PYTHON_VERSION="3.11"

project_directory="$(git rev-parse --show-toplevel)"
test_directory="${project_directory}/bindings/python/test"

pushd "${test_directory}" > /dev/null

python${PYTHON_VERSION} -m pytest -svx ${@}

popd > /dev/null
