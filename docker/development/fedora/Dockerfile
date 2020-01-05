################################################################################################################################################################

# @project        Open Space Toolkit ▸ Mathematics
# @file           docker/development/fedora/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

ARG BASE_IMAGE_VERSION=latest
ARG BASE_IMAGE_SYSTEM=debian

FROM openspacecollective/open-space-toolkit-base:${BASE_IMAGE_VERSION}-${BASE_IMAGE_SYSTEM} as development-base

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
 && wget --quiet http://bitbucket.org/eigen/eigen/get/3.3.7.tar.gz \
 && tar -xvf 3.3.7.tar.gz \
 && cd eigen-eigen-323c052e1731 \
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

## Open Space Toolkit ▸ Core [0.2.5]

RUN mkdir -p /tmp/library-core \
 && cd /tmp/library-core \
 && wget --quiet https://github.com/open-space-collective/library-core/releases/download/0.2.5/library-core-0.2.5-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/library-core/releases/download/0.2.5/library-core-0.2.5-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/library-core

## Open Space Toolkit ▸ IO [0.2.4]

RUN mkdir -p /tmp/library-io \
 && cd /tmp/library-io \
 && wget --quiet https://github.com/open-space-collective/library-io/releases/download/0.2.4/library-io-0.2.4-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/library-io/releases/download/0.2.4/library-io-0.2.4-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/library-io

## Open Space Toolkit ▸ Mathematics [0.2.6]

RUN mkdir -p /tmp/library-mathematics \
 && cd /tmp/library-mathematics \
 && wget --quiet https://github.com/open-space-collective/library-mathematics/releases/download/0.2.6/library-mathematics-0.2.6-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/library-mathematics/releases/download/0.2.6/library-mathematics-0.2.6-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/library-mathematics

## Open Space Toolkit ▸ Physics [0.3.1]

RUN mkdir -p /tmp/library-physics \
 && cd /tmp/library-physics \
 && wget --quiet https://github.com/open-space-collective/library-physics/releases/download/0.3.1/library-physics-0.3.1-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/library-physics/releases/download/0.3.1/library-physics-0.3.1-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/library-physics

# Labels

ARG VERSION

ENV VERSION ${VERSION}

LABEL VERSION="${VERSION}"

# Execution

CMD ["/bin/bash"]

################################################################################################################################################################
