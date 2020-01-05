################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           docker/jupyter/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

ARG JUPYTER_NOTEBOOK_IMAGE_REPOSITORY=jupyter/scipy-notebook:latest

FROM ${JUPYTER_NOTEBOOK_IMAGE_REPOSITORY}

LABEL maintainer="lucas@loftorbital.com"

USER root

# RUN apt-get update \
#  && apt-get install -y libssl1.0.0 libssl-dev \
#  && rm -rf /var/lib/apt/lists/*

# Install extensions

RUN jupyter labextension install @jupyterlab/plotly-extension

COPY ./shortcuts-extension /home/jovyan/.jupyter/lab/user-settings/@jupyterlab/shortcuts-extension

# Install NumPy and Plotly

RUN python -m pip install --quiet numpy --upgrade \
 && python -m pip install --quiet plotly ipympl

RUN chown -R ${NB_UID}:${NB_UID} /home/jovyan/.jupyter

USER ${NB_UID}

ENV JUPYTER_ENABLE_LAB yes

ENV LD_LIBRARY_PATH "/usr/local/lib:/usr/lib/x86_64-linux-gnu:/opt/conda/lib/python3.7/site-packages:/home/jovyan/lib:/opt/conda/lib"
ENV PYTHONPATH "/opt/conda/lib/python3.7/site-packages:/home/jovyan/lib"

RUN pip install LibraryAstrodynamicsPy

################################################################################################################################################################
