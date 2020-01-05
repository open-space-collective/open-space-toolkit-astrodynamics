#!/bin/bash

################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           tutorials/cpp/sensor-modeling/start.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

script_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

docker_image_name="open-space-toolkit-astrodynamics-tutorials-sensor-modeling"
docker_container_name="open-space-toolkit-astrodynamics-tutorials-sensor-modeling"

library_core_directory="/Users/lucas/Projects/Engineering/open-space-collective/open-space-toolkit-core"
library_mathematics_directory="/Users/lucas/Projects/Engineering/open-space-collective/open-space-toolkit-mathematics"
library_physics_directory="/Users/lucas/Projects/Engineering/open-space-collective/open-space-toolkit-physics"
library_astrodynamics_directory="/Users/lucas/Projects/Engineering/open-space-collective/open-space-toolkit-astrodynamics"

pushd ${script_directory} > /dev/null

# Build Docker image if it does not exist already

if [[ "$(docker images -q ${docker_image_name} 2> /dev/null)" == "" ]]; then

    # Build Docker image

    docker build \
    --tag ${docker_image_name} \
    .

fi

options=""
command="/bin/bash"

# Setup linked mode

if [[ ! -z $1 ]] && [[ $1 == "--link" ]]; then

    options=""
    command=""

    # Open Space Toolkit ▸ Core

    if [[ ! -d ${library_core_directory} ]]
    then

        echo "Open Space Toolkit ▸ Core directory [${library_core_directory}s] cannot be found."

        exit 1

    fi

    options="${options} \
    --volume=${library_core_directory}:/mnt/open-space-toolkit-core:ro"

    command=" \
    rm -rf /usr/local/include/OpenSpaceToolkit/Core; \
    rm -f /usr/local/lib/libopen-space-toolkit-core.so*; \
    cp -as /mnt/open-space-toolkit-core/include/OpenSpaceToolkit/Core /usr/local/include/OpenSpaceToolkit/Core; \
    cp -as /mnt/open-space-toolkit-core/src/OpenSpaceToolkit/Core/* /usr/local/include/OpenSpaceToolkit/Core/; \
    ln -s /mnt/open-space-toolkit-core/lib/libopen-space-toolkit-core.so /usr/local/lib/; \
    ln -s /mnt/open-space-toolkit-core/lib/libopen-space-toolkit-core.so.0 /usr/local/lib/;"

    ## Open Space Toolkit ▸ Mathematics

    if [[ ! -d ${library_mathematics_directory} ]]
    then

        echo "Open Space Toolkit ▸ Mathematics directory [${library_mathematics_directory}s] cannot be found."

        exit 1

    fi

    options="${options} \
    --volume=${library_mathematics_directory}:/mnt/open-space-toolkit-mathematics:ro"

    command="${command} \
    rm -rf /usr/local/include/OpenSpaceToolkit/Mathematics; \
    rm -f /usr/local/lib/libopen-space-toolkit-mathematics.so*; \
    cp -as /mnt/open-space-toolkit-mathematics/include/OpenSpaceToolkit/Mathematics /usr/local/include/OpenSpaceToolkit/Mathematics; \
    cp -as /mnt/open-space-toolkit-mathematics/src/OpenSpaceToolkit/Mathematics/* /usr/local/include/OpenSpaceToolkit/Mathematics/; \
    ln -s /mnt/open-space-toolkit-mathematics/lib/libopen-space-toolkit-mathematics.so /usr/local/lib/; \
    ln -s /mnt/open-space-toolkit-mathematics/lib/libopen-space-toolkit-mathematics.so.0 /usr/local/lib/;"

    ## Open Space Toolkit ▸ Physics

    if [[ ! -d ${library_physics_directory} ]]
    then

        echo "Open Space Toolkit ▸ Physics directory [${library_physics_directory}s] cannot be found."

        exit 1

    fi

    options="${options} \
    --volume=${library_physics_directory}:/mnt/open-space-toolkit-physics:ro"

    command="${command} \
    rm -rf /usr/local/include/OpenSpaceToolkit/Physics; \
    rm -f /usr/local/lib/libopen-space-toolkit-physics.so*; \
    cp -as /mnt/open-space-toolkit-physics/include/OpenSpaceToolkit/Physics /usr/local/include/OpenSpaceToolkit/Physics; \
    cp -as /mnt/open-space-toolkit-physics/src/OpenSpaceToolkit/Physics/* /usr/local/include/OpenSpaceToolkit/Physics/; \
    ln -s /mnt/open-space-toolkit-physics/lib/libopen-space-toolkit-physics.so /usr/local/lib/; \
    ln -s /mnt/open-space-toolkit-physics/lib/libopen-space-toolkit-physics.so.0 /usr/local/lib/;"

    ## Open Space Toolkit ▸ Astrodynamics

    if [[ ! -d ${library_astrodynamics_directory} ]]
    then

        echo "Open Space Toolkit ▸ Astrodynamics directory [${library_astrodynamics_directory}s] cannot be found."

        exit 1

    fi

    options="${options} \
    --volume=${library_astrodynamics_directory}:/mnt/open-space-toolkit-astrodynamics:ro"

    command="${command} \
    rm -rf /usr/local/include/OpenSpaceToolkit/Astrodynamics; \
    rm -f /usr/local/lib/libopen-space-toolkit-astrodynamics.so*; \
    cp -as /mnt/open-space-toolkit-astrodynamics/include/OpenSpaceToolkit/Astrodynamics /usr/local/include/OpenSpaceToolkit/Astrodynamics; \
    cp -as /mnt/open-space-toolkit-astrodynamics/src/OpenSpaceToolkit/Astrodynamics/* /usr/local/include/OpenSpaceToolkit/Astrodynamics/; \
    ln -s /mnt/open-space-toolkit-astrodynamics/lib/libopen-space-toolkit-astrodynamics.so /usr/local/lib/; \
    ln -s /mnt/open-space-toolkit-astrodynamics/lib/libopen-space-toolkit-astrodynamics.so.0 /usr/local/lib/;"

    # Output

    command="${command} \
    /bin/bash"

fi

# Run Docker container

docker run \
--name="${docker_container_name}" \
-it \
--rm \
${options} \
--volume="${script_directory}:/app:rw" \
--volume="${library_astrodynamics_directory}/share/data:/app/share/data:ro" \
--workdir="/app/build" \
"${docker_image_name}" \
/bin/bash -c "${command}"

popd > /dev/null

################################################################################################################################################################
