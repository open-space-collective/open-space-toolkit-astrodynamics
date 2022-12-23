################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           docker/release/fedora/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

ARG VERSION

FROM openspacecollective/open-space-toolkit-astrodynamics-development:${VERSION}-fedora as cpp-builder

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

FROM fedora:32 as cpp-release

ENV LD_LIBRARY_PATH="/usr/local/lib"

ENV OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY="/usr/local/share/OpenSpaceToolkit/Physics/coordinate/frame/providers/iers"
ENV OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY="/usr/local/share/OpenSpaceToolkit/Physics/environment/ephemerides/spice"
ENV OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY="/usr/local/share/OpenSpaceToolkit/Physics/environment/gravitational/earth"
ENV OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY="/usr/local/share/OpenSpaceToolkit/Physics/environment/magnetic/earth"

COPY --from=cpp-builder /usr/local/include/OpenSpaceToolkit /usr/local/include/OpenSpaceToolkit
COPY --from=cpp-builder /usr/local/lib/libopen-space-toolkit-astrodynamics.* /usr/local/lib/
COPY --from=cpp-builder /usr/local/share/OpenSpaceToolkit /usr/local/share/OpenSpaceToolkit
COPY --from=cpp-builder /usr/local/test/OpenSpaceToolkit /usr/local/test/OpenSpaceToolkit

ENTRYPOINT ["/usr/local/test/OpenSpaceToolkit/Astrodynamics/open-space-toolkit-astrodynamics.test"]

################################################################################################################################################################

FROM fedora:32 as python-builder

COPY --from=cpp-builder /app/build/bindings/python/dist /dist

RUN python3.8 -m ensurepip --default-pip \
 && python3.8 -m pip install /dist/*38*.whl

################################################################################################################################################################

FROM fedora:32 as python-release

LABEL maintainer="lucas@loftorbital.com"

RUN python3.8 -m ensurepip --default-pip \
 && python3.8 -m pip install ipython numpy

COPY --from=us.gcr.io/loft-orbital-public/software/open-space-toolkit/data:0.3.0 /open-space-toolkit/physics /usr/local/share/OpenSpaceToolkit/Physics

ENV OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY="/usr/local/share/OpenSpaceToolkit/Physics/coordinate/frame/providers/iers"
ENV OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY="/usr/local/share/OpenSpaceToolkit/Physics/environment/ephemerides/spice"
ENV OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY="/usr/local/share/OpenSpaceToolkit/Physics/environment/gravitational/earth"
ENV OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY="/usr/local/share/OpenSpaceToolkit/Physics/environment/magnetic/earth"

COPY --from=python-builder /usr/local/lib/python3.8/site-packages/ostk /usr/local/lib/python3.8/site-packages/ostk

ENTRYPOINT ["ipython"]

################################################################################################################################################################
