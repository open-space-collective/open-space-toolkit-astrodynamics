# Apache License 2.0

ARG BASE_IMAGE_VERSION="latest"

FROM openspacecollective/open-space-toolkit-base:${BASE_IMAGE_VERSION}

LABEL maintainer="lucas@loftorbital.com"

# Dependencies

## Zip & jq

RUN apt-get update \
 && apt-get install -y unzip jq \
 && rm -rf /var/lib/apt/lists/*

## {fmt}

ARG FMT_VERSION="5.2.0"

RUN git clone --branch ${FMT_VERSION} --depth 1 https://github.com/fmtlib/fmt.git /tmp/fmt \
 && cd /tmp/fmt \
 && mkdir build \
 && cd build \
 && cmake -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE .. \
 && make --silent -j $(nproc) \
 && make install \
 && rm -rf /tmp/fmt

## ordered-map

ARG ORDERED_MAP_VERSION="0.6.0"

RUN git clone --branch v${ORDERED_MAP_VERSION} --depth 1 https://github.com/Tessil/ordered-map.git /tmp/ordered-map \
 && cd /tmp/ordered-map \
 && cp -r ./include/tsl /usr/local/include \
 && rm -rf /tmp/ordered-map

## Eigen

ARG EIGEN_VERSION="3.4.0"

RUN mkdir /tmp/eigen \
 && cd /tmp/eigen \
 && wget --quiet https://gitlab.com/libeigen/eigen/-/archive/${EIGEN_VERSION}/eigen-${EIGEN_VERSION}.tar.gz \
 && tar -xvf eigen-${EIGEN_VERSION}.tar.gz \
 && cd eigen-${EIGEN_VERSION} \
 && mkdir build \
 && cd build \
 && cmake .. \
 && make install \
 && rm -rf /tmp/eigen

## SGP4

ARG SGP4_VERSION="6a448b4"

RUN git clone https://github.com/dnwrnr/sgp4.git /tmp/sgp4 \
 && cd /tmp/sgp4 \
 && git checkout ${SGP4_VERSION} \
 && mkdir build \
 && cd build \
 && cmake -DCMAKE_CXX_FLAGS="-fPIC" .. \
 && make -j $(nproc) \
 && mkdir /usr/local/include/sgp4 \
 && cp ../libsgp4/*.h /usr/local/include/sgp4/ \
 && cp libsgp4/*.a /usr/local/lib \
 && rm -rf /tmp/sgp4

## NLopt

ARG NLOPT_VERSION="2.5.0"

RUN git clone --branch v${NLOPT_VERSION} --depth 1 https://github.com/stevengj/nlopt.git /tmp/nlopt \
 && cd /tmp/nlopt \
 && mkdir build \
 && cd build \
 && cmake -DBUILD_SHARED_LIBS=OFF .. \
 && make \
 && make install \
 && rm -rf /tmp/nlopt

## Script to get the latest patch version of an Open Space Toolkit repository
## args: $1:project name, $2:major version, $3:minor version
RUN echo "VERSION_REGEX='^'\$2'\\.'\$3'\\.[0-9]*$'; curl https://api.github.com/repos/open-space-collective/\$1/releases -s | jq -r .[].tag_name | grep -m1 \$VERSION_REGEX" \
    > "/tmp/latest_ostk_patch_version.sh" \
    && chmod +x "/tmp/latest_ostk_patch_version.sh"

## Open Space Toolkit ▸ Core

ARG OSTK_CORE_MAJOR="0"
ARG OSTK_CORE_MINOR="7"

RUN mkdir -p /tmp/open-space-toolkit-core \
 && cd /tmp/open-space-toolkit-core \
 && export FETCH_VERSION=$(/tmp/latest_ostk_patch_version.sh open-space-toolkit-core ${OSTK_CORE_MAJOR} ${OSTK_CORE_MINOR}) \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-core/releases/download/${FETCH_VERSION}/open-space-toolkit-core-${FETCH_VERSION}-1.x86_64-runtime.deb \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-core/releases/download/${FETCH_VERSION}/open-space-toolkit-core-${FETCH_VERSION}-1.x86_64-devel.deb \
 && apt-get install -y ./*.deb \
 && rm -rf /tmp/open-space-toolkit-core

## Open Space Toolkit ▸ I/O

ARG OSTK_IO_MAJOR="0"
ARG OSTK_IO_MINOR="6"

RUN mkdir -p /tmp/open-space-toolkit-io \
 && cd /tmp/open-space-toolkit-io \
 && export FETCH_VERSION=$(/tmp/latest_ostk_patch_version.sh open-space-toolkit-io ${OSTK_IO_MAJOR} ${OSTK_IO_MINOR}) \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-io/releases/download/${FETCH_VERSION}/open-space-toolkit-io-${FETCH_VERSION}-1.x86_64-runtime.deb \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-io/releases/download/${FETCH_VERSION}/open-space-toolkit-io-${FETCH_VERSION}-1.x86_64-devel.deb \
 && apt-get install -y ./*.deb \
 && rm -rf /tmp/open-space-toolkit-io

## Open Space Toolkit ▸ Mathematics

ARG OSTK_MATHEMATICS_MAJOR="0"
ARG OSTK_MATHEMATICS_MINOR="7"

RUN mkdir -p /tmp/open-space-toolkit-mathematics \
 && cd /tmp/open-space-toolkit-mathematics \
 && export FETCH_VERSION=$(/tmp/latest_ostk_patch_version.sh open-space-toolkit-mathematics ${OSTK_MATHEMATICS_MAJOR} ${OSTK_MATHEMATICS_MINOR}) \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-mathematics/releases/download/${FETCH_VERSION}/open-space-toolkit-mathematics-${FETCH_VERSION}-1.x86_64-runtime.deb \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-mathematics/releases/download/${FETCH_VERSION}/open-space-toolkit-mathematics-${FETCH_VERSION}-1.x86_64-devel.deb \
 && apt-get install -y ./*.deb \
 && rm -rf /tmp/open-space-toolkit-mathematics

## Open Space Toolkit ▸ Physics

ARG OSTK_PHYSICS_MAJOR="1"
ARG OSTK_PHYSICS_MINOR="1"

RUN mkdir -p /tmp/open-space-toolkit-physics \
 && cd /tmp/open-space-toolkit-physics \
 && export FETCH_VERSION=$(/tmp/latest_ostk_patch_version.sh open-space-toolkit-physics ${OSTK_PHYSICS_MAJOR} ${OSTK_PHYSICS_MINOR}) \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-physics/releases/download/${FETCH_VERSION}/open-space-toolkit-physics-${FETCH_VERSION}-1.x86_64-runtime.deb \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-physics/releases/download/${FETCH_VERSION}/open-space-toolkit-physics-${FETCH_VERSION}-1.x86_64-devel.deb \
 && apt-get install -y ./*.deb \
 && rm -rf /tmp/open-space-toolkit-physics

# Install data

COPY --from=us.gcr.io/loft-orbital-public/software/open-space-toolkit/data:0.3.0 /open-space-toolkit/physics /usr/local/share/OpenSpaceToolkit/Physics

ENV OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY "/usr/local/share/OpenSpaceToolkit/Physics/coordinate/frame/providers/iers"
ENV OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY "/usr/local/share/OpenSpaceToolkit/Physics/environment/ephemerides/spice"
ENV OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY "/usr/local/share/OpenSpaceToolkit/Physics/environment/gravitational/earth"
ENV OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY "/usr/local/share/OpenSpaceToolkit/Physics/environment/magnetic/earth"

# Labels

ARG VERSION

ENV VERSION ${VERSION}

LABEL VERSION="${VERSION}"

# Execution

CMD ["/bin/bash"]
