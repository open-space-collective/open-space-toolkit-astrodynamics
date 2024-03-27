# Apache License 2.0

ARG VERSION

ARG PYTHON_TEST_VERSION="3.11"
ARG PYTHON_TEST_VERSION_WITHOUT_DOT="311"

FROM openspacecollective/open-space-toolkit-astrodynamics-development:${VERSION} as cpp-builder

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

FROM debian:buster as cpp-release

ENV LD_LIBRARY_PATH="/usr/local/lib"

# Install OSTk data

ARG OSTK_DATA_LOCAL_CACHE="/var/cache/open-space-toolkit-data"
ENV OSTK_DATA_LOCAL_CACHE="${OSTK_DATA_LOCAL_CACHE}"
ENV OSTK_PHYSICS_DATA_LOCAL_REPOSITORY="${OSTK_DATA_LOCAL_CACHE}/data"

RUN git clone --branch v1 --single-branch --depth=1 https://github.com/open-space-collective/open-space-toolkit-data.git ${OSTK_PHYSICS_DATA_LOCAL_REPOSITORY} && chmod -R g+w ${OSTK_PHYSICS_DATA_LOCAL_REPOSITORY}

COPY --from=cpp-builder /usr/local/include/OpenSpaceToolkit /usr/local/include/OpenSpaceToolkit
COPY --from=cpp-builder /usr/local/lib/libopen-space-toolkit-astrodynamics.* /usr/local/lib/
COPY --from=cpp-builder /usr/local/share/OpenSpaceToolkit /usr/local/share/OpenSpaceToolkit
COPY --from=cpp-builder /usr/local/test/OpenSpaceToolkit /usr/local/test/OpenSpaceToolkit

ENTRYPOINT ["/usr/local/test/OpenSpaceToolkit/Astrodynamics/open-space-toolkit-astrodynamics.test"]

FROM python:${PYTHON_TEST_VERSION}-slim as python-builder

COPY --from=cpp-builder /app/build/bindings/python/dist /dist

RUN pip install /dist/*${PYTHON_TEST_VERSION_WITHOUT_DOT}*.whl

FROM python:${PYTHON_TEST_VERSION}-slim as python-release

LABEL maintainer="lucas@loftorbital.com"

RUN apt-get update -y \
 && apt-get install -y libcurl4-openssl-dev libssl-dev wget unzip \
 && rm -rf /var/lib/apt/lists/*

RUN pip install ipython numpy

# Install OSTk data

ARG OSTK_DATA_LOCAL_CACHE="/var/cache/open-space-toolkit-data"
ENV OSTK_DATA_LOCAL_CACHE="${OSTK_DATA_LOCAL_CACHE}"
ENV OSTK_PHYSICS_DATA_LOCAL_REPOSITORY="${OSTK_DATA_LOCAL_CACHE}/data"

RUN git clone --branch v1 --single-branch --depth=1 https://github.com/open-space-collective/open-space-toolkit-data.git ${OSTK_PHYSICS_DATA_LOCAL_REPOSITORY} && chmod -R g+w ${OSTK_PHYSICS_DATA_LOCAL_REPOSITORY}

COPY --from=python-builder /usr/local/lib/python${PYTHON_TEST_VERSION}/site-packages/ostk /usr/local/lib/python${PYTHON_TEST_VERSION}/site-packages/ostk

ENTRYPOINT ["ipython"]
