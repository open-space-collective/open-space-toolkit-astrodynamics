#!/bin/bash

################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/development/exec.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

script_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

pushd "${script_directory}" > /dev/null

# Setup environment

source "../.env"

# Exec Docker container

docker exec \
-it \
${container_name} \
/bin/bash

popd > /dev/null

################################################################################################################################################################