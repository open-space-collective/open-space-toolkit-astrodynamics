#!/bin/bash

################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/ci/binaries.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

################################################################################################################################################################

script_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

project_directory="${script_directory}/../.."
development_directory="${project_directory}/tools/development"

source "${project_directory}/tools/.env"

# Generate binaries

docker run \
--rm \
--volume="${project_directory}:/app:rw" \
--volume="/app/build" \
--volume="${development_directory}/helpers/build.sh:/app/build/build.sh:ro" \
--volume="${development_directory}/helpers/test.sh:/app/build/test.sh:ro" \
--workdir="/app/build" \
${image_name} \
/bin/bash -c "/app/build/build.sh"

################################################################################################################################################################