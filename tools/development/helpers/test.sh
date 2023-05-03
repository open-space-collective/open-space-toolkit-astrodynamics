#!/bin/bash

# Copyright © Loft Orbital Solutions Inc.

project_directory="$(git rev-parse --show-toplevel)"

pushd "${project_directory}" > /dev/null

# make test

if [[ -z ${1} ]]; then
    ./bin/*.test
else
    ./bin/*.test --gtest_filter=${1}
fi

popd > /dev/null
