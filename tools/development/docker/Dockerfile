################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/development/docker/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

################################################################################################################################################################

FROM openspacecollective/library-physics:latest

LABEL maintainer="lucas@loftorbital.com"

# Arguments

ARG version
ARG cpu_count=1

# Dependencies

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

## NumPy

RUN pip install --upgrade pip \
    && pip install numpy

# Labels

ENV version ${version}

LABEL version="${version}"

# Execution

CMD ["/bin/bash"]

################################################################################################################################################################