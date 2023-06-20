#!/bin/bash

# Apache License 2.0 

# Check input arguments

if [[ -z ${project_directory} ]]; then
    echo "Variable [project_directory] is undefined."
    exit 1
fi

if [[ -z ${docker_development_image_repository} ]]; then
    echo "Variable [docker_development_image_repository] is undefined."
    exit 1
fi

if [[ -z ${docker_image_version} ]]; then
    echo "Variable [docker_image_version] is undefined."
    exit 1
fi

project_directory=$(git rev-parse --show-toplevel)

# Initialize variables

options=()
command=""

# Setup linked mode

if [[ ! -z ${1} ]] && [[ ${1} == "--link" ]]; then

    for link in "${@:2}"

    do

        # Extract last part of the path
        link_without_trailing_slash=${link%/}
        dep=${link_without_trailing_slash##*/}

        # Log the linking step

        echo "Linking with ${dep} at ${link}..."

        # Open Space Toolkit ▸ Core

        if [[ ${dep} == "open-space-toolkit-core" ]]; then

            options+=( "-v" )
            options+=( "${link}:/mnt/open-space-toolkit-core:ro" )

            command="${command} \
            rm -rf /usr/local/include/OpenSpaceToolkit/Core; \
            rm -f /usr/local/lib/libopen-space-toolkit-core.so*; \
            cp -as /mnt/open-space-toolkit-core/include/OpenSpaceToolkit/Core /usr/local/include/OpenSpaceToolkit/Core; \
            cp -as /mnt/open-space-toolkit-core/src/OpenSpaceToolkit/Core/* /usr/local/include/OpenSpaceToolkit/Core/; \
            ln -s /mnt/open-space-toolkit-core/lib/libopen-space-toolkit-core.so /usr/local/lib/; \
            ln -s /mnt/open-space-toolkit-core/lib/libopen-space-toolkit-core.so.${project_major_version} /usr/local/lib/;"

        fi

        # Open Space Toolkit ▸ IO

        if [[ ${dep} == "open-space-toolkit-io" ]]; then

            options+=( "-v" )
            options+=( "${link}:/mnt/open-space-toolkit-io:ro" )

            command="${command} \
            rm -rf /usr/local/include/OpenSpaceToolkit/IO; \
            rm -f /usr/local/lib/libopen-space-toolkit-io.so*; \
            cp -as /mnt/open-space-toolkit-io/include/OpenSpaceToolkit/IO /usr/local/include/OpenSpaceToolkit/IO; \
            cp -as /mnt/open-space-toolkit-io/src/OpenSpaceToolkit/IO/* /usr/local/include/OpenSpaceToolkit/IO/; \
            ln -s /mnt/open-space-toolkit-io/lib/libopen-space-toolkit-io.so /usr/local/lib/; \
            ln -s /mnt/open-space-toolkit-io/lib/libopen-space-toolkit-io.so.${project_major_version} /usr/local/lib/;"

        fi

        # Open Space Toolkit ▸ Mathematics

        if [[ ${dep} == "open-space-toolkit-mathematics" ]]; then

            options+=( "-v" )
            options+=( "${link}:/mnt/open-space-toolkit-mathematics:ro" )

            command="${command} \
            rm -rf /usr/local/include/OpenSpaceToolkit/Mathematics; \
            rm -f /usr/local/lib/libopen-space-toolkit-mathematics.so*; \
            cp -as /mnt/open-space-toolkit-mathematics/include/OpenSpaceToolkit/Mathematics /usr/local/include/OpenSpaceToolkit/Mathematics; \
            cp -as /mnt/open-space-toolkit-mathematics/src/OpenSpaceToolkit/Mathematics/* /usr/local/include/OpenSpaceToolkit/Mathematics/; \
            ln -s /mnt/open-space-toolkit-mathematics/lib/libopen-space-toolkit-mathematics.so /usr/local/lib/; \
            ln -s /mnt/open-space-toolkit-mathematics/lib/libopen-space-toolkit-mathematics.so.${project_major_version} /usr/local/lib/;"

        fi

        # Open Space Toolkit ▸ Physics

        if [[ ${dep} == "open-space-toolkit-physics" ]]; then

            options+=( "-v" )
            options+=( "${link}:/mnt/open-space-toolkit-physics:ro" )
        
            command="${command} \
            rm -rf /usr/local/include/OpenSpaceToolkit/Physics; \
            rm -f /usr/local/lib/libopen-space-toolkit-physics.so*; \
            cp -as /mnt/open-space-toolkit-physics/include/OpenSpaceToolkit/Physics /usr/local/include/OpenSpaceToolkit/Physics; \
            cp -as /mnt/open-space-toolkit-physics/src/OpenSpaceToolkit/Physics/* /usr/local/include/OpenSpaceToolkit/Physics/; \
            ln -s /mnt/open-space-toolkit-physics/lib/libopen-space-toolkit-physics.so /usr/local/lib/; \
            ln -s /mnt/open-space-toolkit-physics/lib/libopen-space-toolkit-physics.so.${project_major_version} /usr/local/lib/;"

        fi

    done

    command="${command} /bin/bash"

fi

# Run Docker container

docker run \
    -it \
    --rm \
    --privileged \
    "${options[@]}" \
    --volume="${project_directory}:/app:delegated" \
    --volume="${project_directory}/tools/development/helpers:/app/build/helpers:ro,delegated" \
    --workdir="/app/build" \
    ${docker_development_image_repository}:${docker_image_version} \
    /bin/bash -c "${command}"
