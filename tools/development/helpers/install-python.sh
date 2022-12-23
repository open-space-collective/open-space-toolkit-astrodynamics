#!/bin/bash

################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           tools/development/helpers/install-python.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

PYTHON_VERSION="3.11"

################################################################################################################################################################

project_directory="$(git rev-parse --show-toplevel)"
python_directory="${project_directory}/build/bindings/python/OpenSpaceToolkitAstrodynamicsPy-python-package-${PYTHON_VERSION}"

pushd "${python_directory}" > /dev/null

python${PYTHON_VERSION} -m pip install . --force-reinstall

popd > /dev/null

################################################################################################################################################################
