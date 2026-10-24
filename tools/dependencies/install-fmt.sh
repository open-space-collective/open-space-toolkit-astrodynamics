#!/usr/bin/env bash

# Apache License 2.0

# Build and install fmt into ${INSTALL_PREFIX}.

set -euo pipefail

FMT_VERSION="5.2.0"
prefix="${INSTALL_PREFIX:-/usr/local}"

source_dir="$(mktemp -d)"
trap 'rm -rf "${source_dir}"' EXIT

git clone --branch "${FMT_VERSION}" --depth 1 https://github.com/fmtlib/fmt.git "${source_dir}"

cmake -S "${source_dir}" -B "${source_dir}/build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="${prefix}" \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DFMT_DOC=OFF \
    -DFMT_TEST=OFF
cmake --build "${source_dir}/build" --parallel "$(getconf _NPROCESSORS_ONLN)"
cmake --install "${source_dir}/build"
