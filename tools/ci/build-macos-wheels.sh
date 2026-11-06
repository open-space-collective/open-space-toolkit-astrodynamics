#!/usr/bin/env bash

# Apache License 2.0

# Build Apple Silicon wheels for open-space-toolkit-astrodynamics.
# fmt, ordered-map, Eigen, NLopt, and Google Benchmark are installed with
# tools/dependencies, as in docker/development/Dockerfile.
# Core, IO, Mathematics, and Physics come from the branches that can produce macOS libraries.

set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
prefix="${root}/.macos-deps"
build_dir="${root}/build-macos"
packages_dir="${root}/packages/python"

export HOMEBREW_NO_AUTO_UPDATE=1
export NONINTERACTIVE=1
export UV_PYTHON_PREFERENCE=only-managed
unset VIRTUAL_ENV

if [[ "$(uname -s)" != "Darwin" || "$(uname -m)" != "arm64" ]]; then
    echo "This script builds Apple Silicon (arm64) wheels."
    exit 1
fi

export PATH="$(brew --prefix)/bin:${HOME}/.local/bin:${PATH}"
brew install cmake boost

deployment_target="${MACOSX_DEPLOYMENT_TARGET:-14.0}"
boost_library="$(find "$(brew --prefix boost)/lib" -name 'libboost_filesystem.dylib' -print -quit)"
if [[ -f "${boost_library}" ]]; then
    library_minos="$(otool -l "${boost_library}" | awk '/minos/ { print $2; exit }')"
    if [[ -n "${library_minos}" ]]; then
        deployment_target="$(
            deployment_target="${deployment_target}" library_minos="${library_minos}" python3 - <<'PY'
import os

def version(value):
    return tuple(int(part) for part in value.split("."))

chosen = os.environ["deployment_target"]
required = os.environ["library_minos"]
print(required if version(required) > version(chosen) else chosen)
PY
        )"
    fi
fi
export MACOSX_DEPLOYMENT_TARGET="${deployment_target}"
export CMAKE_POLICY_VERSION_MINIMUM=3.5
echo "Wheel deployment target is ${deployment_target}."

if [[ ! -f "${prefix}/.installed" ]]; then
    INSTALL_PREFIX="${prefix}" "${root}/tools/dependencies/install.sh"
    touch "${prefix}/.installed"
fi

prepare_dependency() {
    local repo="$1"
    local match="$2"
    local sibling="${root}/../${repo}"
    if [[ ! -x "${sibling}/tools/ci/build-macos-wheels.sh" ]] || ! git -C "${sibling}" describe --tags --match "${match}*" --always | grep -q "^${match}"; then
        echo "Need a ${match} checkout of ${repo} next to this repository."
        exit 1
    fi
    shopt -s nullglob
    local wheels=("${sibling}/packages/python/"*cp314-cp314-*.whl)
    if [[ ${#wheels[@]} -eq 0 ]]; then
        bash "${sibling}/tools/ci/build-macos-wheels.sh"
        wheels=("${sibling}/packages/python/"*cp314-cp314-*.whl)
    fi
    if [[ ${#wheels[@]} -eq 0 ]]; then
        echo "${repo} did not produce a CPython 3.14 wheel."
        exit 1
    fi
    cmake --install "${sibling}/build-macos" --prefix "${prefix}"
    local wheel
    for wheel in "${sibling}/packages/python/"*.whl; do
        ln -sfn "${wheel}" "${prefix}/wheelhouse/$(basename "${wheel}")"
    done
}

mkdir -p "${prefix}/wheelhouse"
prepare_dependency open-space-toolkit-core "5."
prepare_dependency open-space-toolkit-io "4."
prepare_dependency open-space-toolkit-mathematics "4."
prepare_dependency open-space-toolkit-physics "14."

tool_venv="${prefix}/tool-venv"
if [[ ! -x "${tool_venv}/bin/python" ]]; then
    uv venv "${tool_venv}" --python 3.12
fi
uv pip install --python "${tool_venv}/bin/python" "pybind11>=2.12" delocate
pybind11_dir="$("${tool_venv}/bin/python" -c 'import pybind11; print(pybind11.get_cmake_dir())')"

installed_versions=()
for version in 3.9 3.10 3.11 3.12 3.13 3.14; do
    if uv python install "${version}"; then
        installed_versions+=("${version}")
    elif [[ "${version}" == "3.9" ]]; then
        echo "Python 3.9 has no build for this macOS. Skipping it."
    else
        exit 1
    fi
done
python_search_versions="$(IFS=';'; echo "${installed_versions[*]}")"

export UV_FIND_LINKS="${prefix}/wheelhouse"
export CPATH="${prefix}/include${CPATH:+:${CPATH}}"

rm -rf "${build_dir}"
cmake -S "${root}" -B "${build_dir}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="${prefix};$(brew --prefix)" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="${MACOSX_DEPLOYMENT_TARGET}" \
    -DOpenSpaceToolkitCore_DIR="${prefix}/lib/cmake/OpenSpaceToolkitCore" \
    -DOpenSpaceToolkitCore_INCLUDE_DIR="${prefix}/include" \
    -DOpenSpaceToolkitCore_LIBRARY="${prefix}/lib/libopen-space-toolkit-core.dylib" \
    -DOpenSpaceToolkitIO_DIR="${prefix}/lib/cmake/OpenSpaceToolkitIO" \
    -DOpenSpaceToolkitIO_INCLUDE_DIR="${prefix}/include" \
    -DOpenSpaceToolkitIO_LIBRARY="${prefix}/lib/libopen-space-toolkit-io.dylib" \
    -DOpenSpaceToolkitMathematics_DIR="${prefix}/lib/cmake/OpenSpaceToolkitMathematics" \
    -DOpenSpaceToolkitMathematics_INCLUDE_DIR="${prefix}/include" \
    -DOpenSpaceToolkitMathematics_LIBRARY="${prefix}/lib/libopen-space-toolkit-mathematics.dylib" \
    -DOpenSpaceToolkitPhysics_DIR="${prefix}/lib/cmake/OpenSpaceToolkitPhysics" \
    -DOpenSpaceToolkitPhysics_INCLUDE_DIR="${prefix}/include" \
    -DOpenSpaceToolkitPhysics_LIBRARY="${prefix}/lib/libopen-space-toolkit-physics.dylib" \
    -Dpybind11_DIR="${pybind11_dir}" \
    -DPYTHON_SEARCH_VERSIONS="${python_search_versions}" \
    -DBUILD_WITH_DEBUG_SYMBOLS=OFF \
    -DBUILD_UNIT_TESTS=OFF \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DBUILD_DOCUMENTATION=OFF \
    -DBUILD_BENCHMARK=OFF \
    -DBUILD_WITH_BOOST_STACKTRACE=OFF \
    -DBUILD_WITH_BOOST_STATIC=OFF
cmake --build "${build_dir}" --parallel "$(sysctl -n hw.ncpu)"

mkdir -p "${packages_dir}"
rm -f "${packages_dir}"/*.whl
shopt -s nullglob
wheels=("${build_dir}/bindings/python/dist/"*.whl)
if [[ ${#wheels[@]} -eq 0 ]]; then
    echo "CMake did not produce any wheels."
    exit 1
fi

for wheel in "${wheels[@]}"; do
    "${tool_venv}/bin/delocate-wheel" --require-archs arm64 --ignore-missing-dependencies -w "${packages_dir}" "${wheel}"
done

for wheel in "${packages_dir}"/*.whl; do
    unpack="$(mktemp -d)"
    unzip -q "${wheel}" -d "${unpack}"
    for binary in "${unpack}"/ostk/astrodynamics/libopen-space-toolkit-astrodynamics*.dylib "${unpack}"/ostk/astrodynamics/OpenSpaceToolkitAstrodynamicsPy*.so; do
        [[ -f "${binary}" ]] || continue
        install_name_tool -add_rpath @loader_path/../core "${binary}"
        install_name_tool -add_rpath @loader_path/../io "${binary}"
        install_name_tool -add_rpath @loader_path/../mathematics "${binary}"
        install_name_tool -add_rpath @loader_path/../physics "${binary}"
        codesign -s - -f "${binary}" >/dev/null
    done
    rm -f "${wheel}"
    uv run --python 3.12 --with wheel wheel pack "${unpack}" -d "${packages_dir}" >/dev/null
    rm -rf "${unpack}"
done

smoke_wheel="$(echo "${packages_dir}"/*cp314-cp314-*.whl)"
smoke_venv="${prefix}/smoke-venv"
rm -rf "${smoke_venv}"
uv venv "${smoke_venv}" --python 3.14
uv pip install --python "${smoke_venv}/bin/python" --find-links "${prefix}/wheelhouse" "${smoke_wheel}"
"${smoke_venv}/bin/python" -c 'import ostk.astrodynamics'

echo "macOS wheels are in ${packages_dir}"
