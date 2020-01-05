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

library_core_directory="/Users/lucas/Projects/Engineering/open-space-collective/library-core"
library_mathematics_directory="/Users/lucas/Projects/Engineering/open-space-collective/library-mathematics"
library_physics_directory="/Users/lucas/Projects/Engineering/open-space-collective/library-physics"
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
    --volume=${library_core_directory}:/mnt/library-core:ro"

    command=" \
    rm -rf /usr/local/include/Library/Core; \
    rm -f /usr/local/lib/liblibrary-core.so*; \
    cp -as /mnt/library-core/include/Library/Core /usr/local/include/Library/Core; \
    cp -as /mnt/library-core/src/Library/Core/* /usr/local/include/Library/Core/; \
    ln -s /mnt/library-core/lib/liblibrary-core.so /usr/local/lib/; \
    ln -s /mnt/library-core/lib/liblibrary-core.so.0 /usr/local/lib/;"

    ## Open Space Toolkit ▸ Mathematics

    if [[ ! -d ${library_mathematics_directory} ]]
    then

        echo "Open Space Toolkit ▸ Mathematics directory [${library_mathematics_directory}s] cannot be found."

        exit 1

    fi

    options="${options} \
    --volume=${library_mathematics_directory}:/mnt/library-mathematics:ro"

    command="${command} \
    rm -rf /usr/local/include/Library/Mathematics; \
    rm -f /usr/local/lib/liblibrary-mathematics.so*; \
    cp -as /mnt/library-mathematics/include/Library/Mathematics /usr/local/include/Library/Mathematics; \
    cp -as /mnt/library-mathematics/src/Library/Mathematics/* /usr/local/include/Library/Mathematics/; \
    ln -s /mnt/library-mathematics/lib/liblibrary-mathematics.so /usr/local/lib/; \
    ln -s /mnt/library-mathematics/lib/liblibrary-mathematics.so.0 /usr/local/lib/;"

    ## Open Space Toolkit ▸ Physics

    if [[ ! -d ${library_physics_directory} ]]
    then

        echo "Open Space Toolkit ▸ Physics directory [${library_physics_directory}s] cannot be found."

        exit 1

    fi

    options="${options} \
    --volume=${library_physics_directory}:/mnt/library-physics:ro"

    command="${command} \
    rm -rf /usr/local/include/Library/Physics; \
    rm -f /usr/local/lib/liblibrary-physics.so*; \
    cp -as /mnt/library-physics/include/Library/Physics /usr/local/include/Library/Physics; \
    cp -as /mnt/library-physics/src/Library/Physics/* /usr/local/include/Library/Physics/; \
    ln -s /mnt/library-physics/lib/liblibrary-physics.so /usr/local/lib/; \
    ln -s /mnt/library-physics/lib/liblibrary-physics.so.0 /usr/local/lib/;"

    ## Open Space Toolkit ▸ Astrodynamics

    if [[ ! -d ${library_astrodynamics_directory} ]]
    then

        echo "Open Space Toolkit ▸ Astrodynamics directory [${library_astrodynamics_directory}s] cannot be found."

        exit 1

    fi

    options="${options} \
    --volume=${library_astrodynamics_directory}:/mnt/open-space-toolkit-astrodynamics:ro"

    command="${command} \
    rm -rf /usr/local/include/Library/Astrodynamics; \
    rm -f /usr/local/lib/libopen-space-toolkit-astrodynamics.so*; \
    cp -as /mnt/open-space-toolkit-astrodynamics/include/Library/Astrodynamics /usr/local/include/Library/Astrodynamics; \
    cp -as /mnt/open-space-toolkit-astrodynamics/src/Library/Astrodynamics/* /usr/local/include/Library/Astrodynamics/; \
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
