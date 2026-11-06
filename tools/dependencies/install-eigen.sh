#!/usr/bin/env bash

# Apache License 2.0

# Install Eigen headers into ${INSTALL_PREFIX}.

set -euo pipefail

EIGEN_VERSION="3.4.0"
prefix="${INSTALL_PREFIX:-/usr/local}"

source_dir="$(mktemp -d)"
trap 'rm -rf "${source_dir}"' EXIT

archive="${source_dir}/eigen.tar.gz"
url="https://gitlab.com/libeigen/eigen/-/archive/${EIGEN_VERSION}/eigen-${EIGEN_VERSION}.tar.gz"
if command -v curl >/dev/null 2>&1; then
    curl -fsSL "${url}" -o "${archive}"
elif command -v wget >/dev/null 2>&1; then
    wget --quiet "${url}" -O "${archive}"
else
    echo "curl or wget is required to download Eigen."
    exit 1
fi
tar -xzf "${archive}" -C "${source_dir}" --strip-components 1

cmake -S "${source_dir}" -B "${source_dir}/build" \
    -DCMAKE_INSTALL_PREFIX="${prefix}"
cmake --build "${source_dir}/build" --target install --parallel "$(getconf _NPROCESSORS_ONLN)"
