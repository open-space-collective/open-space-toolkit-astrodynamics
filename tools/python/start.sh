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

docker run \
--name="${container_name}-notebook" \
-it \
--rm \
--publish="8889:8888" \
--user="" \
--env="JUPYTER_LAB_ENABLE=yes" \
--env="LD_LIBRARY_PATH=/opt/conda/lib/python3.6/site-packages:/home/jovyan/lib" \
--env="PYTHONPATH=/opt/conda/lib/python3.6/site-packages:/home/jovyan/lib" \
--volume=$(pwd)/../../lib:/opt/lib:ro \
--volume=$(pwd)/../../share/python/notebooks:/home/jovyan/notebooks \
jupyter/scipy-notebook \
bash -c "mkdir -p /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics \
&& ln -s /opt/lib/liblibrary-astrodynamics.so.0 /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/liblibrary-astrodynamics.so.0 \
&& ln -s /opt/lib/LibraryAstrodynamicsPy.so /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/LibraryAstrodynamicsPy.so \
&& echo 'from .LibraryAstrodynamicsPy import *' > /opt/conda/lib/python3.6/site-packages/Library/Astrodynamics/__init__.py \
&& start-notebook.sh --NotebookApp.token=''"

popd

################################################################################################################################################################