################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           tutorials/cpp/sensor-modeling/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

FROM openspacecollective/open-space-toolkit-base:0.1.4

LABEL maintainer="lucas@loftorbital.com"

# Dependencies

## {fmt} [master]

RUN pushd /tmp > /dev/null \
 && git clone https://github.com/fmtlib/fmt.git \
 && cd fmt \
 && mkdir build \
 && cd build \
 && cmake -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE .. \
 && make --silent -j $(nproc) \
 && make install \
 && rm -rf /tmp/fmt \
 && popd > /dev/null

## Eigen [3.3.4]

RUN mkdir /tmp/eigen \
 && cd /tmp/eigen \
 && wget --quiet http://bitbucket.org/eigen/eigen/get/3.3.4.tar.gz \
 && tar -xvf 3.3.4.tar.gz \
 && cd eigen-eigen-5a0156e40feb \
 && mkdir build \
 && cd build \
 && cmake .. \
 && make install \
 && rm -rf /tmp/eigen

## Open Space Toolkit ▸ Core [0.1.16]

RUN mkdir -p /tmp/open-space-toolkit-core \
 && pushd /tmp/open-space-toolkit-core > /dev/null \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-core/releases/download/0.1.16/open-space-toolkit-core-0.1.16-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-core/releases/download/0.1.16/open-space-toolkit-core-0.1.16-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/open-space-toolkit-core \
 && popd > /dev/null

## Open Space Toolkit ▸ Mathematics [0.1.12]

RUN mkdir -p /tmp/open-space-toolkit-mathematics \
 && pushd /tmp/open-space-toolkit-mathematics > /dev/null \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-mathematics/releases/download/0.1.12/open-space-toolkit-mathematics-0.1.12-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-mathematics/releases/download/0.1.12/open-space-toolkit-mathematics-0.1.12-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/open-space-toolkit-mathematics \
 && popd > /dev/null

## Open Space Toolkit ▸ Physics [0.1.8]

RUN mkdir -p /tmp/open-space-toolkit-physics \
 && pushd /tmp/open-space-toolkit-physics > /dev/null \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-physics/releases/download/0.1.8/open-space-toolkit-physics-0.1.8-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-physics/releases/download/0.1.8/open-space-toolkit-physics-0.1.8-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/open-space-toolkit-physics \
 && popd > /dev/null

## Open Space Toolkit ▸ Astrodynamics [0.1.3]

RUN mkdir -p /tmp/open-space-toolkit-astrodynamics \
 && pushd /tmp/open-space-toolkit-astrodynamics > /dev/null \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-astrodynamics/releases/download/0.1.3/open-space-toolkit-astrodynamics-0.1.3-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-astrodynamics/releases/download/0.1.3/open-space-toolkit-astrodynamics-0.1.3-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/open-space-toolkit-astrodynamics \
 && popd > /dev/null

# Labels

ARG VERSION

ENV VERSION ${VERSION}

LABEL VERSION="${VERSION}"

# Execution

CMD ["/bin/bash"]

################################################################################################################################################################
