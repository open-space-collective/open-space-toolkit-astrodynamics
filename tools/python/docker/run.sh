#!/bin/bash

################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/python/docker/run.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

################################################################################################################################################################

script_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

pushd ${script_directory} > /dev/null

source "../../.env"

docker run \
--name="${container_name}-python" \
-it \
--rm \
"${repository_name}/${project_name}-python" \
/bin/bash

popd > /dev/null

################################################################################################################################################################