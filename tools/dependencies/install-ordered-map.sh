#!/usr/bin/env bash

# Apache License 2.0

# Install ordered-map headers into ${INSTALL_PREFIX}.

set -euo pipefail

ORDERED_MAP_VERSION="0.6.0"
prefix="${INSTALL_PREFIX:-/usr/local}"

source_dir="$(mktemp -d)"
trap 'rm -rf "${source_dir}"' EXIT

git clone --branch "v${ORDERED_MAP_VERSION}" --depth 1 https://github.com/Tessil/ordered-map.git "${source_dir}"

mkdir -p "${prefix}/include"
cp -R "${source_dir}/include/tsl" "${prefix}/include/"
