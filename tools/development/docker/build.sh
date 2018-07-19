#!/bin/bash

################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/development/docker/build.sh
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

################################################################################################################################################################

script_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Setup environment

source "${script_directory}/../../.env"

echo "version = ${version}"
echo "cpu_count = ${cpu_count}"
echo "image_name = ${image_name}"
echo "script_directory = ${script_directory}"

# Build Docker image

docker build \
--build-arg="version=${version}" \
--build-arg="cpu_count=${cpu_count}" \
--tag="${image_name}" \
--file="${script_directory}/Dockerfile" \
"${script_directory}"

# Tag Docker image

docker tag "${image_name}" "${repository_name}/${project_name}:latest"

################################################################################################################################################################