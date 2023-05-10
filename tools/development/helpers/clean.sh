#!/bin/bash

# Apache License 2.0 

project_directory="$(git rev-parse --show-toplevel)"

pushd "${project_directory}" > /dev/null

find ./build -mindepth 1 ! -regex '^./build/helpers\(/.*\)?' -delete

rm -rf ./bin/*.exe
rm -rf ./bin/*.test
rm -rf ./bin/*.test-*
rm -rf ./docs/html
rm -rf ./docs/latex
rm -rf ./lib/*.so
rm -rf ./lib/*.so.*

popd > /dev/null
