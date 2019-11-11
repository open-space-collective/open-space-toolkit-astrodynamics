################################################################################################################################################################

# @project        Library ▸ Astrodynamics
# @file           docker/release/fedora/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

ARG VERSION

FROM openspacecollective/library-astrodynamics-development:${VERSION}-fedora as cpp-builder

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

FROM fedora:30 as cpp-release

ENV LD_LIBRARY_PATH /usr/local/lib

COPY --from=cpp-builder /usr/local/include/Library /usr/local/include/Library
COPY --from=cpp-builder /usr/local/lib/liblibrary-astrodynamics.* /usr/local/lib/
COPY --from=cpp-builder /usr/local/share/Library /usr/local/share/Library
COPY --from=cpp-builder /usr/local/test/Library /usr/local/test/Library

ENTRYPOINT ["/usr/local/test/Library/Astrodynamics/library-astrodynamics.test"]

################################################################################################################################################################

FROM fedora:30 as python-builder

COPY --from=cpp-builder /app/build/bindings/python/dist /dist

RUN pip3 install /dist/*.whl

################################################################################################################################################################

FROM fedora:30 as python-release

LABEL maintainer="lucas@loftorbital.com"

RUN ln -s /usr/bin/pip3 /usr/bin/pip

RUN dnf update -y \
 && dnf install -y libcurl-devel openssl-devel \
 && dnf clean all

RUN pip install numpy

COPY --from=python-builder /usr/local/lib/python3.7/site-packages/Library /usr/local/lib/python3.7/site-packages/Library

ENTRYPOINT ["python3"]

################################################################################################################################################################
