################################################################################################################################################################

# @project        Open Space Toolkit ▸ Mathematics
# @file           docker/development/fedora/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

ARG BASE_IMAGE_VERSION=latest

FROM openspacecollective/open-space-toolkit-base:${BASE_IMAGE_VERSION}-fedora

LABEL maintainer="lucas@loftorbital.com"

# Dependencies

## {fmt} [5.2.0]

RUN git clone --branch 5.2.0 --depth 1 https://github.com/fmtlib/fmt.git /tmp/fmt \
 && cd /tmp/fmt \
 && mkdir build \
 && cd build \
 && cmake -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE .. \
 && make --silent -j $(nproc) \
 && make install \
 && rm -rf /tmp/fmt

## ordered-map [0.6.0]

RUN git clone --branch v0.6.0 --depth 1 https://github.com/Tessil/ordered-map.git /tmp/ordered-map \
 && cd /tmp/ordered-map \
 && cp -r ./include/tsl /usr/local/include \
 && rm -rf /tmp/ordered-map

## Eigen [3.3.7]

RUN mkdir /tmp/eigen \
 && cd /tmp/eigen \
 && wget --quiet https://gitlab.com/libeigen/eigen/-/archive/3.3.7/eigen-3.3.7.tar.gz \
 && tar -xvf eigen-3.3.7.tar.gz \
 && cd eigen-3.3.7 \
 && mkdir build \
 && cd build \
 && cmake .. \
 && make install \
 && rm -rf /tmp/eigen

## SGP4 [master]

RUN git clone https://github.com/dnwrnr/sgp4.git /tmp/sgp4 \
 && cd /tmp/sgp4 \
 && mkdir build \
 && cd build \
 && cmake -DCMAKE_CXX_FLAGS="-fPIC" .. \
 && make -j $(nproc) \
 && mkdir /usr/local/include/sgp4 \
 && cp ../libsgp4/*.h /usr/local/include/sgp4/ \
 && cp libsgp4/*.a /usr/local/lib \
 && rm -rf /tmp/sgp4

## NLopt [2.5.0]

RUN git clone --branch v2.5.0 --depth 1 https://github.com/stevengj/nlopt.git /tmp/nlopt \
 && cd /tmp/nlopt \
 && mkdir build \
 && cd build \
 && cmake -DBUILD_SHARED_LIBS=OFF .. \
 && make \
 && make install \
 && rm -rf /tmp/nlopt

## Open Space Toolkit ▸ Core

ARG OSTK_CORE_VERSION=0.4.3

RUN mkdir -p /tmp/open-space-toolkit-core \
 && cd /tmp/open-space-toolkit-core \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-core/releases/download/${OSTK_CORE_VERSION}/open-space-toolkit-core-${OSTK_CORE_VERSION}-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-core/releases/download/${OSTK_CORE_VERSION}/open-space-toolkit-core-${OSTK_CORE_VERSION}-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/open-space-toolkit-core

## Open Space Toolkit ▸ IO

ARG OSTK_IO_VERSION=0.4.3

RUN mkdir -p /tmp/open-space-toolkit-io \
 && cd /tmp/open-space-toolkit-io \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-io/releases/download/${OSTK_IO_VERSION}/open-space-toolkit-io-${OSTK_IO_VERSION}-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-io/releases/download/${OSTK_IO_VERSION}/open-space-toolkit-io-${OSTK_IO_VERSION}-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/open-space-toolkit-io

## Open Space Toolkit ▸ Mathematics

ARG OSTK_MATHEMATICS_VERSION=0.4.4

RUN mkdir -p /tmp/open-space-toolkit-mathematics \
 && cd /tmp/open-space-toolkit-mathematics \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-mathematics/releases/download/${OSTK_MATHEMATICS_VERSION}/open-space-toolkit-mathematics-${OSTK_MATHEMATICS_VERSION}-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-mathematics/releases/download/${OSTK_MATHEMATICS_VERSION}/open-space-toolkit-mathematics-${OSTK_MATHEMATICS_VERSION}-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/open-space-toolkit-mathematics

## Open Space Toolkit ▸ Physics

ARG OSTK_PHYSICS_VERSION=0.5.13

RUN mkdir -p /tmp/open-space-toolkit-physics \
 && cd /tmp/open-space-toolkit-physics \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-physics/releases/download/${OSTK_PHYSICS_VERSION}/open-space-toolkit-physics-${OSTK_PHYSICS_VERSION}-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-physics/releases/download/${OSTK_PHYSICS_VERSION}/open-space-toolkit-physics-${OSTK_PHYSICS_VERSION}-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/open-space-toolkit-physics

# Labels

ARG VERSION

ENV VERSION ${VERSION}

LABEL VERSION="${VERSION}"

# Execution

CMD ["/bin/bash"]

################################################################################################################################################################
