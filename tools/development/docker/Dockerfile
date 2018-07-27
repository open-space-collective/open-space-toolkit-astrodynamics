################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/development/docker/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

################################################################################################################################################################

FROM openspacecollective/library-physics:0.1.0

LABEL maintainer="lucas@loftorbital.com"

# Arguments

ARG version
ARG cpu_count=1

# Dependencies

## SGP4 [master]

RUN pushd /tmp > /dev/null \
    && git clone https://github.com/dnwrnr/sgp4.git \
    && cd sgp4 \
    && mkdir build \
    && cd build \
    && cmake -DCMAKE_CXX_FLAGS="-fPIC" .. \
    && make -j ${cpu_count} \
    && mkdir /usr/local/include/sgp4 \
    && cp ../libsgp4/*.h /usr/local/include/sgp4/ \
    && cp libsgp4/*.a /usr/local/lib \
    && rm -rf /tmp/sgp4 \
    && popd > /dev/null

## Library :: Physics [master]

RUN pushd /tmp > /dev/null \
    && git clone https://github.com/open-space-collective/library-physics.git \
    && cd library-physics \
    && mkdir build \
    && cd build \
    && cmake -DBUILD_UNIT_TESTS=OFF .. \
    && make -j ${cpu_count} \
    && make install \
    && rm -rf /tmp/library-physics \
    && popd > /dev/null

# Labels

ENV version ${version}

LABEL version="${version}"

# Execution

CMD ["/bin/bash"]

################################################################################################################################################################