#!/bin/bash

################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           tools/development/start.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

if [[ -z ${project_directory} ]]; then
    echo "Variable [project_directory] is undefined."
    exit 1
fi

options=""
command="/bin/bash"

# Setup linked mode

if [[ ! -z ${1} ]] && [[ ${1} == "--link" ]]; then

    options=""
    command=""

    # Library ▸ Core

    if [[ -z ${library_core_directory} ]]; then
        echo "Variable [library_core_directory] is undefined."
        exit 1
    fi

    if [[ ! -d ${library_core_directory} ]]; then
        echo "Library ▸ Core directory [${library_core_directory}] cannot be found."
        exit 1
    fi

    options="${options} \
    --volume=${library_core_directory}:/mnt/library-core:ro"

    command=" \
    rm -rf /usr/local/include/Library/Core; \
    rm -f /usr/local/lib/liblibrary-core.so*; \
    cp -as /mnt/library-core/include/Library/Core /usr/local/include/Library/Core; \
    cp -as /mnt/library-core/src/Library/Core/* /usr/local/include/Library/Core/; \
    ln -s /mnt/library-core/lib/liblibrary-core.so /usr/local/lib/; \
    ln -s /mnt/library-core/lib/liblibrary-core.so.0 /usr/local/lib/;"

    # Library ▸ I/O

    # TBI

    # Library ▸ Mathematics

    # TBI

    # Output

    command="${command} \
    /bin/bash"

fi

# Run Docker container

docker run \
-it \
--rm \
--privileged \
${options} \
--volume="${project_directory}:/app:delegated" \
--volume="${project_directory}/tools/development/helpers:/app/build/helpers:ro,delegated" \
--workdir="/app/build" \
${docker_development_image_repository}:${docker_image_version}-${target} \
/bin/bash -c "${command}"

################################################################################################################################################################
