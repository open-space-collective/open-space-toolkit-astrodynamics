################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/python/docker/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

FROM jupyter/scipy-notebook:latest

LABEL maintainer="lucas@loftorbital.com"

USER root

RUN apt-get update \
 && apt-get install -y libssl1.0.0 libssl-dev \
 && rm -rf /var/lib/apt/lists/*

COPY ./shortcuts-extension /home/jovyan/.jupyter/lab/user-settings/@jupyterlab/shortcuts-extension

RUN jupyter labextension install @jupyterlab/plotly-extension

RUN python -m pip install --quiet numpy --upgrade \
 && python -m pip install --quiet plotly psutil orca

RUN apt-get update \
 && apt-get install -y libxtst6 libgconf-2-4 xvfb \
 && rm -rf /var/lib/apt/lists/*

RUN conda install -y -c plotly plotly-orca

RUN mv /opt/conda/bin/orca /opt/conda/bin/orca-manual \
 && echo '#!/bin/sh\nxvfb-run /opt/conda/bin/orca-manual "$@"' >> /opt/conda/bin/orca \
 && chmod u+x /opt/conda/bin/orca \
 && chown -R ${NB_UID}:${NB_UID} /opt/conda/bin/orca*

RUN chown -R ${NB_UID}:${NB_UID} /home/jovyan/.jupyter

USER ${NB_UID}

# [TBR] LibraryPhysicsPy
RUN pip install LibraryPhysicsPy LibraryAstrodynamicsPy

RUN mkdir -p /home/jovyan/.library

################################################################################################################################################################