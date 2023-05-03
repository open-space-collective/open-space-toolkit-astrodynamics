#!/bin/bash

# Copyright © Loft Orbital Solutions Inc.

project_directory="$(git rev-parse --show-toplevel)"

pushd "${project_directory}" > /dev/null

if [[ -z ${1} ]]; then
    gdb --args ./bin/*.test
else
    gdb --args ./bin/*.test --gtest_filter=${1}
fi

popd > /dev/null
