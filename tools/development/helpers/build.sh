#!/bin/bash

# Apache License 2.0

project_directory="$(git rev-parse --show-toplevel)"
build_directory="${project_directory}/build"

mkdir -p "${build_directory}"

pushd "${build_directory}" > /dev/null

if [[ ! -z $1 ]] && [[ $1 == "--debug" ]]; then

    cmake -DCMAKE_BUILD_TYPE=Debug ..

elif [[ ! -z $1 ]] && [[ $1 == "--notest" ]]; then

    cmake -DBUILD_UNIT_TESTS=OFF ..

elif [[ ! -z $1 ]] && [[ $1 == "--nobind" ]]; then

    cmake -DBUILD_PYTHON_BINDINGS=OFF ..

elif [[ ! -z $1 ]] && [[ $1 == "--notestbind" ]]; then

    cmake -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_UNIT_TESTS=OFF ..

else

    cmake ..

fi

make -j $(nproc)

popd > /dev/null
