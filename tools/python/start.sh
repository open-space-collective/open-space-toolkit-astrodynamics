#!/bin/bash

################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/python/start.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

################################################################################################################################################################

script_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

pushd "${script_directory}" > /dev/null

source ../.env

# Build Docker image if it does not exist already

if [[ "$(docker images -q ${repository_name}/${project_name}-python 2> /dev/null)" == "" ]]; then

    pushd "${script_directory}/docker" > /dev/null

    ./build.sh

    popd

fi

if [[ ! -z $1 ]] && [[ $1 == "--link" ]]; then

    docker run \
    --name="${container_name}-notebook" \
    -it \
    --rm \
    --publish="${python_port}:8888" \
    --user="" \
    --env="JUPYTER_ENABLE_LAB=yes" \
    --env="LD_LIBRARY_PATH=/usr/local/lib:/opt/conda/lib/python3.6/site-packages:/home/jovyan/lib" \
    --env="PYTHONPATH=/opt/conda/lib/python3.6/site-packages:/home/jovyan/lib" \
    --volume="${library_core_directory}/lib:/opt/library-core:ro" \
    --volume="${library_mathematics_directory}/lib:/opt/library-mathematics:ro" \
    --volume="${library_physics_directory}/lib:/opt/library-physics:ro" \
    --volume="${project_directory}/lib:/opt/library-astrodynamics:ro" \
    --volume="${project_directory}/bindings/python/docs:/home/jovyan/docs" \
    --volume="${project_directory}/tutorials/python/notebooks:/home/jovyan/tutorials" \
    --volume="${project_directory}/share/data:/app/share/data" \
    --workdir="/home/jovyan" \
    "${repository_name}/${project_name}-python" \
    bash -c "mkdir -p /opt/conda/lib/python3.6/site-packages/Library/Core \
    && ln -s /opt/library-core/liblibrary-core.so.0 /opt/conda/lib/python3.6/site-packages/Library/Core/liblibrary-core.so.0 \
    && ln -s /opt/library-core/LibraryCorePy.so /opt/conda/lib/python3.6/site-packages/Library/Core/LibraryCorePy.so \
    && echo 'from .LibraryCorePy import *' > /opt/conda/lib/python3.6/site-packages/Library/Core/__init__.py \
    && mkdir -p /opt/conda/lib/python3.6/site-packages/Library/Mathematics \
    && ln -s /opt/library-mathematics/liblibrary-mathematics.so.0 /opt/conda/lib/python3.6/site-packages/Library/Mathematics/liblibrary-mathematics.so.0 \
    && ln -s /opt/library-mathematics/LibraryMathematicsPy.so /opt/conda/lib/python3.6/site-packages/Library/Mathematics/LibraryMathematicsPy.so \
    && echo 'from .LibraryMathematicsPy import *' > /opt/conda/lib/python3.6/site-packages/Library/Mathematics/__init__.py \
    && mkdir -p /opt/conda/lib/python3.6/site-packages/Library/Physics \
    && ln -s /opt/library-physics/liblibrary-physics.so.0 /opt/conda/lib/python3.6/site-packages/Library/Physics/liblibrary-physics.so.0 \
    && ln -s /opt/library-physics/LibraryPhysicsPy.so /opt/conda/lib/python3.6/site-packages/Library/Physics/LibraryPhysicsPy.so \
    && echo 'from .LibraryPhysicsPy import *' > /opt/conda/lib/python3.6/site-packages/Library/Physics/__init__.py \
    && mkdir -p /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics \
    && ln -s /opt/library-astrodynamics/liblibrary-astrodynamics.so.0 /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/liblibrary-astrodynamics.so.0 \
    && ln -s /opt/library-astrodynamics/LibraryAstrodynamicsPy.so /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/LibraryAstrodynamicsPy.so \
    && echo 'from .LibraryAstrodynamicsPy import *' > /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/__init__.py \
    && start-notebook.sh --NotebookApp.token=''"

else

    docker run \
    --name="${container_name}-notebook" \
    -it \
    --rm \
    --publish="${python_port}:8888" \
    --user="" \
    --env="JUPYTER_ENABLE_LAB=yes" \
    --env="LD_LIBRARY_PATH=/usr/local/lib:/opt/conda/lib/python3.6/site-packages:/home/jovyan/lib" \
    --env="PYTHONPATH=/opt/conda/lib/python3.6/site-packages:/home/jovyan/lib" \
    --volume="${project_directory}/lib:/opt/library-astrodynamics:ro" \
    --volume="${project_directory}/bindings/python/docs:/home/jovyan/docs" \
    --volume="${project_directory}/tutorials/python/notebooks:/home/jovyan/tutorials" \
    --volume="${project_directory}/share:/var/library-physics" \
    --volume="${project_directory}/share:/var/library-astrodynamics" \
    --workdir="/home/jovyan" \
    "${repository_name}/${project_name}-python" \
    bash -c "mkdir -p /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics \
    && ln -s /opt/library-astrodynamics/liblibrary-astrodynamics.so.0 /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/liblibrary-astrodynamics.so.0 \
    && ln -s /opt/library-astrodynamics/LibraryAstrodynamicsPy.so /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/LibraryAstrodynamicsPy.so \
    && echo 'from .LibraryAstrodynamicsPy import *' > /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/__init__.py \
    && start-notebook.sh --NotebookApp.token=''"

fi

popd > /dev/null

################################################################################################################################################################