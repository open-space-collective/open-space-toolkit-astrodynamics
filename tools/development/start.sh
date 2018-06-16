#!/bin/bash

################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/development/start.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

################################################################################################################################################################

script_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Setup environment

source "${script_directory}/../.env"

# Build Docker image if it does not exist already

if [[ "$(docker images -q ${image_name} 2> /dev/null)" == "" ]]; then

    pushd "${script_directory}/docker" > /dev/null

    ./build.sh

    popd

fi

# Run Docker container

docker run \
--name="${container_name}" \
-it \
--rm \
--privileged \
--volume="${project_directory}:/app:rw" \
--volume="/app/build" \
--volume="${script_directory}/helpers/build.sh:/app/build/build.sh:ro" \
--volume="${script_directory}/helpers/test.sh:/app/build/test.sh:ro" \
--volume="${script_directory}/helpers/debug.sh:/app/build/debug.sh:ro" \
--volume="${script_directory}/helpers/clean.sh:/app/build/clean.sh:ro" \
--workdir="/app/build" \
"${image_name}" \
"/bin/bash"

################################################################################################################################################################