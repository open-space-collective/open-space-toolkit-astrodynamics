################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tutorials/cpp/sensor-modeling/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

FROM openspacecollective/library-base:0.1.4

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

## Library ▸ Core [0.1.16]

RUN mkdir -p /tmp/library-core \
 && pushd /tmp/library-core > /dev/null \
 && wget --quiet https://github.com/open-space-collective/library-core/releases/download/0.1.16/library-core-0.1.16-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/library-core/releases/download/0.1.16/library-core-0.1.16-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/library-core \
 && popd > /dev/null

## Library ▸ Mathematics [0.1.12]

RUN mkdir -p /tmp/library-mathematics \
 && pushd /tmp/library-mathematics > /dev/null \
 && wget --quiet https://github.com/open-space-collective/library-mathematics/releases/download/0.1.12/library-mathematics-0.1.12-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/library-mathematics/releases/download/0.1.12/library-mathematics-0.1.12-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/library-mathematics \
 && popd > /dev/null

## Library ▸ Physics [0.1.8]

RUN mkdir -p /tmp/library-physics \
 && pushd /tmp/library-physics > /dev/null \
 && wget --quiet https://github.com/open-space-collective/library-physics/releases/download/0.1.8/library-physics-0.1.8-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/library-physics/releases/download/0.1.8/library-physics-0.1.8-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/library-physics \
 && popd > /dev/null

## Library ▸ Astrodynamics [0.1.3]

RUN mkdir -p /tmp/library-astrodynamics \
 && pushd /tmp/library-astrodynamics > /dev/null \
 && wget --quiet https://github.com/open-space-collective/library-astrodynamics/releases/download/0.1.3/library-astrodynamics-0.1.3-1.x86_64-runtime.rpm \
 && wget --quiet https://github.com/open-space-collective/library-astrodynamics/releases/download/0.1.3/library-astrodynamics-0.1.3-1.x86_64-devel.rpm \
 && dnf install -y ./*.rpm \
 && rm -rf /tmp/library-astrodynamics \
 && popd > /dev/null

# Labels

ARG VERSION

ENV VERSION ${VERSION}

LABEL VERSION="${VERSION}"

# Execution

CMD ["/bin/bash"]

################################################################################################################################################################