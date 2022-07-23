#!/bin/bash

################################################################################################################################################################

# @project        Open Space Toolkit ▸ Physics
# @file           tools/development/helpers/test-python.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

project_directory="$(git rev-parse --show-toplevel)"
test_directory="${project_directory}/bindings/python/test"

pushd "${test_directory}" > /dev/null

python3.9 -m pytest -svx ${@}

popd > /dev/null

################################################################################################################################################################
