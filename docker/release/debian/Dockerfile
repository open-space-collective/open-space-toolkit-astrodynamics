################################################################################################################################################################

# @project        Library ▸ Astrodynamics
# @file           docker/release/debian/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

ARG VERSION

FROM openspacecollective/library-astrodynamics-development:${VERSION}-debian as cpp-builder

RUN mkdir -p /app/bin /app/build /app/lib

WORKDIR /app/build

COPY ./bindings /app/bindings
COPY ./docs /app/docs
COPY ./include /app/include
COPY ./share /app/share
COPY ./src /app/src
COPY ./test /app/test
COPY ./thirdparty /app/thirdparty
COPY ./tools /app/tools
COPY CMakeLists.txt /app/CMakeLists.txt
COPY LICENSE /app/LICENSE
COPY README.md /app/README.md
COPY .git /app/.git

RUN cmake .. \
 && make -j $(nproc) \
 && make install

################################################################################################################################################################

FROM debian:buster as cpp-release

ENV LD_LIBRARY_PATH /usr/local/lib

ENV LIBRARY_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY "/usr/local/share/Library/Physics/coordinate/frame/providers/iers"
ENV LIBRARY_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY "/usr/local/share/Library/Physics/environment/ephemerides/spice"
ENV LIBRARY_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY "/usr/local/share/Library/Physics/environment/gravitational/earth"
ENV LIBRARY_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY "/usr/local/share/Library/Physics/environment/magnetic/earth"

COPY --from=cpp-builder /usr/local/include/Library /usr/local/include/Library
COPY --from=cpp-builder /usr/local/lib/liblibrary-astrodynamics.* /usr/local/lib/
COPY --from=cpp-builder /usr/local/share/Library /usr/local/share/Library
COPY --from=cpp-builder /usr/local/test/Library /usr/local/test/Library

ENTRYPOINT ["/usr/local/test/Library/Astrodynamics/library-astrodynamics.test"]

################################################################################################################################################################

FROM python:3.7-slim as python-builder

COPY --from=cpp-builder /app/build/bindings/python/dist /dist

RUN pip install /dist/*.whl

################################################################################################################################################################

FROM python:3.7-slim as python-release

LABEL maintainer="lucas@loftorbital.com"

RUN apt-get update -y \
 && apt-get install -y libcurl4-openssl-dev libssl-dev \
 && rm -rf /var/lib/apt/lists/*

RUN pip install numpy

ENV LIBRARY_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY "/usr/local/share/Library/Physics/coordinate/frame/providers/iers"
ENV LIBRARY_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY "/usr/local/share/Library/Physics/environment/ephemerides/spice"
ENV LIBRARY_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY "/usr/local/share/Library/Physics/environment/gravitational/earth"
ENV LIBRARY_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY "/usr/local/share/Library/Physics/environment/magnetic/earth"

COPY --from=python-builder /usr/local/lib/python3.7/site-packages/Library /usr/local/lib/python3.7/site-packages/Library

ENTRYPOINT ["python"]

################################################################################################################################################################
