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

options=""
command="/bin/bash"

# Setup linked mode

if [[ ! -z $1 ]] && [[ $1 == "--link" ]]; then

    options=""
    command=""

    # Library :: Core

    library_core_directory="${project_directory}/../library-core"

    if [[ ! -d ${library_core_directory} ]]
    then

        echo "Library :: Core directory [${library_core_directory}s] cannot be found."

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
    ln -s /mnt/library-core/lib/liblibrary-core.so.2018 /usr/local/lib/;"

    ## Library :: Mathematics

    library_mathematics_directory="${project_directory}/../library-mathematics"

    if [[ ! -d ${library_mathematics_directory} ]]
    then

        echo "Library :: Mathematics directory [${library_mathematics_directory}s] cannot be found."

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
    ln -s /mnt/library-mathematics/lib/liblibrary-mathematics.so.2018 /usr/local/lib/;"

    ## Library :: Physics

    library_physics_directory="${project_directory}/../library-physics"

    if [[ ! -d ${library_physics_directory} ]]
    then

        echo "Library :: Physics directory [${library_physics_directory}s] cannot be found."

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
    ln -s /mnt/library-physics/lib/liblibrary-physics.so.2018 /usr/local/lib/;"

    # Output

    command="${command} \
    /bin/bash"

fi

# Run Docker container

docker run \
--name="${container_name}" \
-it \
--rm \
--privileged \
${options} \
--volume="${project_directory}:/app:rw" \
--volume="/app/build" \
--volume="${script_directory}/helpers/build.sh:/app/build/build.sh:ro" \
--volume="${script_directory}/helpers/test.sh:/app/build/test.sh:ro" \
--volume="${script_directory}/helpers/debug.sh:/app/build/debug.sh:ro" \
--volume="${script_directory}/helpers/clean.sh:/app/build/clean.sh:ro" \
--workdir="/app/build" \
"${image_name}" \
/bin/bash -c "${command}"

################################################################################################################################################################