#!/usr/bin/env bash

# Apache License 2.0

# Build and install Google Benchmark into ${INSTALL_PREFIX}.

set -euo pipefail

BENCHMARK_VERSION="1.8.2"
prefix="${INSTALL_PREFIX:-/usr/local}"

source_dir="$(mktemp -d)"
trap 'rm -rf "${source_dir}"' EXIT

git clone --branch "v${BENCHMARK_VERSION}" --depth 1 https://github.com/google/benchmark.git "${source_dir}"

cmake -S "${source_dir}" -B "${source_dir}/build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="${prefix}" \
    -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON
cmake --build "${source_dir}/build" --parallel "$(getconf _NPROCESSORS_ONLN)"
cmake --install "${source_dir}/build"
