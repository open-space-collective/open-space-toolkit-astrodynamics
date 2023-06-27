#!/bin/bash

# Apache License 2.0

# Check input arguments

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
deps=""

# Setup linked mode

if [[ ! -z ${1} ]] && [[ ${1} == "--link" ]]; then

    for link in "${@:2}"

    do

        # Extract last part of the path

        dep=$(basename ${link})

        deps+=" ${dep}"

        # Log the linking step

        echo "Linking with ${dep} at ${link}..."

        # Open Space Toolkit ▸ Dep

        project_name=$(echo ${dep} | cut -d "-" -f 4)

        if [ ${project_name} = "io" ]; then
            project_name_capitalized="IO"
        else
            project_name_capitalized=${project_name^}
        fi

        options+=( "-v" )
        options+=( "${link}:/mnt/${dep}:ro" )

        command="${command} \
        rm -rf /usr/local/include/OpenSpaceToolkit/${project_name_capitalized}; \
        rm -f /usr/local/lib/lib${dep}.so*; \
        cp -as /mnt/${dep}/include/OpenSpaceToolkit/${project_name_capitalized} /usr/local/include/OpenSpaceToolkit/${project_name_capitalized}; \
        cp -as /mnt/${dep}/src/OpenSpaceToolkit/${project_name_capitalized}/* /usr/local/include/OpenSpaceToolkit/${project_name_capitalized}/; \
        ln -s /mnt/${dep}/lib/lib${dep}.so /usr/local/lib/; \
        ln -s /mnt/${dep}/lib/lib${dep}.so.* /usr/local/lib/; \
        cp -as /mnt/${dep}/build/bindings/python/dist/* /usr/local/share;"

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
    --env="deps=${deps}" \
    --workdir="/app/build" \
    ${docker_development_image_repository}:${docker_image_version} \
    /bin/bash -c "${command}"
