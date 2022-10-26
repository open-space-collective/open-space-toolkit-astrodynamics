#!/bin/bash

################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           tools/development/start.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

project_directory=$(git rev-parse --show-toplevel)

open_space_toolkit_physics_directory="${project_directory}/../open-space-toolkit-physics"

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

    # Open Space Toolkit ▸ Core

    # if [[ -z ${open_space_toolkit_core_directory} ]]; then
    #     echo "Variable [open_space_toolkit_core_directory] is undefined."
    #     exit 1
    # fi

    # if [[ ! -d ${open_space_toolkit_core_directory} ]]; then
    #     echo "Open Space Toolkit ▸ Core directory [${open_space_toolkit_core_directory}] cannot be found."
    #     exit 1
    # fi

    # options="${options} \
    # --volume=${open_space_toolkit_core_directory}:/mnt/open-space-toolkit-core:ro"

    # command="${command} \
    # rm -rf /usr/local/include/OpenSpaceToolkit/Core; \
    # rm -f /usr/local/lib/libopen-space-toolkit-core.so*; \
    # cp -as /mnt/open-space-toolkit-core/include/OpenSpaceToolkit/Core /usr/local/include/OpenSpaceToolkit/Core; \
    # cp -as /mnt/open-space-toolkit-core/src/OpenSpaceToolkit/Core/* /usr/local/include/OpenSpaceToolkit/Core/; \
    # ln -s /mnt/open-space-toolkit-core/lib/libopen-space-toolkit-core.so /usr/local/lib/; \
    # ln -s /mnt/open-space-toolkit-core/lib/libopen-space-toolkit-core.so.0 /usr/local/lib/;"

    # Open Space Toolkit ▸ I/O

    # TBI

    # Open Space Toolkit ▸ Mathematics

    # TBI

    # Open Space Toolkit ▸ Physics

    if [[ -z ${open_space_toolkit_physics_directory} ]]; then
        echo "Variable [open_space_toolkit_physics_directory] is undefined."
        exit 1
    fi

    if [[ ! -d ${open_space_toolkit_physics_directory} ]]; then
        echo "Open Space Toolkit ▸ Physics directory [${open_space_toolkit_physics_directory}] cannot be found."
        exit 1
    fi

    # options="--volume=$(printf %q "${open_space_toolkit_physics_directory}"):/mnt/open-space-toolkit-physics:ro"

    command="${command} \
    rm -rf /usr/local/include/OpenSpaceToolkit/Physics; \
    rm -f /usr/local/lib/libopen-space-toolkit-physics.so*; \
    cp -as /mnt/open-space-toolkit-physics/include/OpenSpaceToolkit/Physics /usr/local/include/OpenSpaceToolkit/Physics; \
    cp -as /mnt/open-space-toolkit-physics/src/OpenSpaceToolkit/Physics/* /usr/local/include/OpenSpaceToolkit/Physics/; \
    ln -s /mnt/open-space-toolkit-physics/lib/libopen-space-toolkit-physics.so /usr/local/lib/; \
    ln -s /mnt/open-space-toolkit-physics/lib/libopen-space-toolkit-physics.so.0 /usr/local/lib/;"

    # Output

    command="${command} \
    /bin/bash"

fi

# Run Docker container

echo ">>> docker_development_image_repository: [${docker_development_image_repository}]"
echo ">>> docker_image_version: [${docker_image_version}]"
echo ">>> target: [${target}]"

docker run \
    -it \
    --rm \
    --privileged \
    --volume=/home/lucas/ostk/open-space-toolkit-astrodynamics/../open-space-toolkit-physics:/mnt/open-space-toolkit-physics:ro \
    --volume="${project_directory}:/app:delegated" \
    --volume="${project_directory}/tools/development/helpers:/app/build/helpers:ro,delegated" \
    --workdir="/app/build" \
    ${docker_development_image_repository}:${docker_image_version}-${target} \
    /bin/bash -c "${command}"

################################################################################################################################################################
