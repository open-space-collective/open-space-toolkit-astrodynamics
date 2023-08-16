# Apache License 2.0

ARG BASE_IMAGE_VERSION="latest"

FROM openspacecollective/open-space-toolkit-base:${BASE_IMAGE_VERSION}

LABEL maintainer="lucas@loftorbital.com"

# Dependencies

## Zip, jq, & git-lfs

RUN apt-get update \
 && apt-get install -y unzip jq git-lfs \
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

## Open Space Toolkit ▸ Core

ARG OSTK_CORE_MAJOR="0"
ARG OSTK_CORE_MINOR="7"

## Force an image rebuild when new Core patch versions are detected
ADD https://api.github.com/repos/open-space-collective/open-space-toolkit-core/git/matching-refs/tags/${OSTK_CORE_MAJOR}.${OSTK_CORE_MINOR} /tmp/open-space-toolkit-core/versions.json

RUN mkdir -p /tmp/open-space-toolkit-core \
 && cd /tmp/open-space-toolkit-core \
 && export LATEST_PATCH_OF_MINOR=$(jq -r .[-1].ref versions.json | cut -d "/" -f3) \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-core/releases/download/${LATEST_PATCH_OF_MINOR}/open-space-toolkit-core-${LATEST_PATCH_OF_MINOR}-1.x86_64-runtime.deb \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-core/releases/download/${LATEST_PATCH_OF_MINOR}/open-space-toolkit-core-${LATEST_PATCH_OF_MINOR}-1.x86_64-devel.deb \
 && apt-get install -y ./*.deb \
 && rm -rf /tmp/open-space-toolkit-core

## Open Space Toolkit ▸ I/O

ARG OSTK_IO_MAJOR="0"
ARG OSTK_IO_MINOR="7"

## Force an image rebuild when new IO patch versions are detected
ADD https://api.github.com/repos/open-space-collective/open-space-toolkit-io/git/matching-refs/tags/${OSTK_IO_MAJOR}.${OSTK_IO_MINOR} /tmp/open-space-toolkit-io/versions.json

RUN mkdir -p /tmp/open-space-toolkit-io \
 && cd /tmp/open-space-toolkit-io \
 && export LATEST_PATCH_OF_MINOR=$(jq -r .[-1].ref versions.json | cut -d "/" -f3) \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-io/releases/download/${LATEST_PATCH_OF_MINOR}/open-space-toolkit-io-${LATEST_PATCH_OF_MINOR}-1.x86_64-runtime.deb \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-io/releases/download/${LATEST_PATCH_OF_MINOR}/open-space-toolkit-io-${LATEST_PATCH_OF_MINOR}-1.x86_64-devel.deb \
 && apt-get install -y ./*.deb \
 && rm -rf /tmp/open-space-toolkit-io

## Open Space Toolkit ▸ Mathematics

ARG OSTK_MATHEMATICS_MAJOR="0"
ARG OSTK_MATHEMATICS_MINOR="7"

## Force an image rebuild when new Mathematics patch versions are detected
ADD https://api.github.com/repos/open-space-collective/open-space-toolkit-mathematics/git/matching-refs/tags/${OSTK_MATHEMATICS_MAJOR}.${OSTK_MATHEMATICS_MINOR} /tmp/open-space-toolkit-mathematics/versions.json

RUN mkdir -p /tmp/open-space-toolkit-mathematics \
 && cd /tmp/open-space-toolkit-mathematics \
 && export LATEST_PATCH_OF_MINOR=$(jq -r .[-1].ref versions.json | cut -d "/" -f3) \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-mathematics/releases/download/${LATEST_PATCH_OF_MINOR}/open-space-toolkit-mathematics-${LATEST_PATCH_OF_MINOR}-1.x86_64-runtime.deb \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-mathematics/releases/download/${LATEST_PATCH_OF_MINOR}/open-space-toolkit-mathematics-${LATEST_PATCH_OF_MINOR}-1.x86_64-devel.deb \
 && apt-get install -y ./*.deb \
 && rm -rf /tmp/open-space-toolkit-mathematics

## Open Space Toolkit ▸ Physics

ARG OSTK_PHYSICS_MAJOR="2"
ARG OSTK_PHYSICS_MINOR="0"

## Force an image rebuild when new Physics patch versions are detected
ADD https://api.github.com/repos/open-space-collective/open-space-toolkit-physics/git/matching-refs/tags/${OSTK_PHYSICS_MAJOR}.${OSTK_PHYSICS_MINOR} /tmp/open-space-toolkit-physics/versions.json

RUN mkdir -p /tmp/open-space-toolkit-physics \
 && cd /tmp/open-space-toolkit-physics \
 && export LATEST_PATCH_OF_MINOR=$(jq -r .[-1].ref versions.json | cut -d "/" -f3) \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-physics/releases/download/${LATEST_PATCH_OF_MINOR}/open-space-toolkit-physics-${LATEST_PATCH_OF_MINOR}-1.x86_64-runtime.deb \
 && wget --quiet https://github.com/open-space-collective/open-space-toolkit-physics/releases/download/${LATEST_PATCH_OF_MINOR}/open-space-toolkit-physics-${LATEST_PATCH_OF_MINOR}-1.x86_64-devel.deb \
 && apt-get install -y ./*.deb \
 && rm -rf /tmp/open-space-toolkit-physics

# Install data

RUN git clone --branch main --single-branch --depth=1 https://github.com/open-space-collective/open-space-toolkit-data.git /usr/local/share/open-space-toolkit-data

ENV OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data/coordinate/frame/providers/iers"
ENV OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data/environment/ephemerides/spice"
ENV OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data/environment/gravitational/earth"
ENV OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data/environment/magnetic/earth"
ENV OSTK_PHYSICS_ENVIRONMENT_ATMOSPHERIC_EARTH_MANAGER_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data/environment/atmospheric/earth"
ENV OSTK_PHYSICS_DATA_MANIFEST_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data"

# Labels

ARG VERSION
ENV VERSION="${VERSION}"
LABEL VERSION="${VERSION}"

# Execution

CMD [ "/bin/bash" ]
