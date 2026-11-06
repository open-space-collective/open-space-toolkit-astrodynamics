#!/usr/bin/env bash

# Apache License 2.0

# Build and install NLopt as a static library into ${INSTALL_PREFIX}.

set -euo pipefail

NLOPT_VERSION="2.9.1"
prefix="${INSTALL_PREFIX:-/usr/local}"

source_dir="$(mktemp -d)"
trap 'rm -rf "${source_dir}"' EXIT

git clone --branch "v${NLOPT_VERSION}" --depth 1 https://github.com/stevengj/nlopt.git "${source_dir}"

cmake -S "${source_dir}" -B "${source_dir}/build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="${prefix}" \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DBUILD_SHARED_LIBS=OFF \
    -DNLOPT_PYTHON=OFF \
    -DNLOPT_OCTAVE=OFF \
    -DNLOPT_MATLAB=OFF \
    -DNLOPT_GUILE=OFF \
    -DNLOPT_SWIG=OFF
cmake --build "${source_dir}/build" --parallel "$(getconf _NPROCESSORS_ONLN)"
cmake --install "${source_dir}/build"
