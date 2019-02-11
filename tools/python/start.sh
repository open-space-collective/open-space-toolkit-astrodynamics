#!/bin/bash

################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/python/start.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

script_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

pushd "${script_directory}" > /dev/null

source "../.env"

# Build Docker image if it does not exist already

if [[ "$(docker images -q ${image_name}-python:${image_version} 2> /dev/null)" == "" ]]; then

    pushd "${script_directory}/docker" > /dev/null

    ./build.sh

    popd

fi

options=""
command="start-notebook.sh --NotebookApp.token=''"

if [[ ! -z $1 ]] && [[ $1 == "--link" ]]; then

    command=""

    # # Library ▸ Core

    # options="${options} \
    # --volume=${library_core_directory}:/opt/library-core:ro"

    # command="${command} \
    # mkdir -p /opt/conda/lib/python3.6/site-packages/Library/Core; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/Core/liblibrary-core.so.0; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/Core/LibraryCorePy.so; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/Core/__init__.py; \
    # ln -s /opt/library-core/lib/liblibrary-core.so.0 /opt/conda/lib/python3.6/site-packages/Library/Core/liblibrary-core.so.0; \
    # ln -s /opt/library-core/lib/LibraryCorePy.so /opt/conda/lib/python3.6/site-packages/Library/Core/LibraryCorePy.so; \
    # ln -s /opt/library-core/bindings/python/tools/python/Library/Core/__init__.py /opt/conda/lib/python3.6/site-packages/Library/Core/__init__.py;"

    # # Library ▸ I/O

    # options="${options} \
    # --volume=${library_io_directory}:/opt/library-io:ro"

    # command="${command} \
    # mkdir -p /opt/conda/lib/python3.6/site-packages/Library/IO; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/IO/liblibrary-io.so.0; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/IO/LibraryIOPy.so; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/IO/__init__.py; \
    # ln -s /opt/library-io/lib/liblibrary-io.so.0 /opt/conda/lib/python3.6/site-packages/Library/IO/liblibrary-io.so.0; \
    # ln -s /opt/library-io/lib/LibraryIOPy.so /opt/conda/lib/python3.6/site-packages/Library/IO/LibraryIOPy.so; \
    # ln -s /opt/library-io/bindings/python/tools/python/Library/IO/__init__.py /opt/conda/lib/python3.6/site-packages/Library/IO/__init__.py;"

    # # Library ▸ Mathematics

    # options="${options} \
    # --volume=${library_mathematics_directory}:/opt/library-mathematics:ro"

    # command="${command} \
    # mkdir -p /opt/conda/lib/python3.6/site-packages/Library/Mathematics; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/Mathematics/liblibrary-mathematics.so.0; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/Mathematics/LibraryMathematicsPy.so; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/Mathematics/__init__.py; \
    # ln -s /opt/library-mathematics/lib/liblibrary-mathematics.so.0 /opt/conda/lib/python3.6/site-packages/Library/Mathematics/liblibrary-mathematics.so.0; \
    # ln -s /opt/library-mathematics/lib/LibraryMathematicsPy.so /opt/conda/lib/python3.6/site-packages/Library/Mathematics/LibraryMathematicsPy.so; \
    # ln -s /opt/library-mathematics/bindings/python/tools/python/Library/Mathematics/__init__.py /opt/conda/lib/python3.6/site-packages/Library/Mathematics/__init__.py;"

    # # Library ▸ Physics

    # options="${options} \
    # --volume=${project_directory}:/opt/library-physics:ro"

    # command="${command} \
    # mkdir -p /opt/conda/lib/python3.6/site-packages/Library/Physics; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/Physics/liblibrary-physics.so.0; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/Physics/LibraryPhysicsPy.so; \
    # rm -rf /opt/conda/lib/python3.6/site-packages/Library/Physics/__init__.py; \
    # ln -s /opt/library-physics/lib/liblibrary-physics.so.0 /opt/conda/lib/python3.6/site-packages/Library/Physics/liblibrary-physics.so.0; \
    # ln -s /opt/library-physics/lib/LibraryPhysicsPy.so /opt/conda/lib/python3.6/site-packages/Library/Physics/LibraryPhysicsPy.so; \
    # ln -s /opt/library-physics/bindings/python/tools/python/Library/Physics/__init__.py /opt/conda/lib/python3.6/site-packages/Library/Physics/__init__.py;"

    # Library ▸ Astrodynamics

    options="${options} \
    --volume=${project_directory}:/opt/library-astrodynamics:ro"

    command="${command} \
    mkdir -p /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics; \
    rm -rf /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/liblibrary-astrodynamics.so.0; \
    rm -rf /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/LibraryAstrodynamicsPy.so; \
    rm -rf /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/__init__.py; \
    ln -s /opt/library-astrodynamics/lib/liblibrary-astrodynamics.so.0 /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/liblibrary-astrodynamics.so.0; \
    ln -s /opt/library-astrodynamics/lib/LibraryAstrodynamicsPy.so /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/LibraryAstrodynamicsPy.so; \
    ln -s /opt/library-astrodynamics/bindings/python/tools/python/Library/Astrodynamics/__init__.py /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/__init__.py;"

    command="${command} \
    start-notebook.sh --NotebookApp.token=''"

fi

# Run Docker container

docker run \
--name="${container_name}-notebook" \
-it \
--rm \
--publish="${python_port}:8888" \
--env-file="${script_directory}/.env" \
--volume="${project_directory}/bindings/python/docs:/home/jovyan/docs" \
--volume="${project_directory}/tutorials/python/notebooks:/home/jovyan/tutorials" \
--volume="${project_directory}/share:/home/jovyan/.library/astrodynamics" \
--workdir="/home/jovyan" \
${options} \
"${image_name}-python:${image_version}" \
/bin/bash -c "${command}"

popd > /dev/null

################################################################################################################################################################