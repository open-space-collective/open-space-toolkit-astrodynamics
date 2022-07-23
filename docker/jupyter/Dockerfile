################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           docker/jupyter/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

ARG JUPYTER_NOTEBOOK_IMAGE_REPOSITORY=jupyter/scipy-notebook:python-3.9.12
# ARG NB_UIB

FROM ${JUPYTER_NOTEBOOK_IMAGE_REPOSITORY}

LABEL maintainer="lucas@loftorbital.com"

ENV JUPYTER_ENABLE_LAB yes

# Set user to root
# RUN id -u

USER root
# RUN echo ${NB_UID}
# Install Python utilities
# RUN apt-get install curl
RUN sudo apt-get update
RUN sudo apt-get install -y curl

RUN pip install --quiet numpy pandas plotly
RUN conda install ipympl=0.4.0

# Install JupyterLab extensions

RUN jupyter labextension install --no-build \
        @jupyterlab/shortcuts-extension \
        @jupyter-widgets/jupyterlab-manager \
        jupyter-matplotlib \
        plotlywidget \
        jupyterlab-plotly \
 && jupyter lab build \
 && jupyter labextension update --all

COPY ./shortcuts-extension /home/jovyan/.jupyter/lab/user-settings/@jupyterlab/shortcuts-extension

RUN chown -R ${NB_UID}:${NB_UID} /home/jovyan/.jupyter

ENV LD_LIBRARY_PATH "/usr/local/lib:/usr/lib/x86_64-linux-gnu:/opt/conda/lib/python3.9/site-packages:/home/jovyan/lib:/opt/conda/lib"
ENV PYTHONPATH "/opt/conda/lib/python3.9/site-packages:/home/jovyan/lib"

# Add OSTk Data Settings

COPY --from=us.gcr.io/loft-orbital-public/software/open-space-toolkit/data:latest /open-space-toolkit /open-space-toolkit

# RUN chown -R jovyan:${NB_UID} /open-space-toolkit [TBR] will remove this soon if deemed unnecessary

ENV OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_MODE "Automatic"
ENV OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY "/open-space-toolkit/physics/coordinate/frame/providers/iers"
ENV OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_REMOTE_URL "https://storage.googleapis.com/loft-orbital-open-space-toolkit/physics/coordinate/frame/providers/iers/latest/"
ENV OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_ENGINE_MODE "Automatic"
ENV OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY "/open-space-toolkit/physics/environment/ephemerides/spice"
ENV OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_REMOTE_URL "https://storage.googleapis.com/loft-orbital-open-space-toolkit/physics/environment/ephemerides/spice/"
ENV OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_ENABLED true
ENV OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY "/open-space-toolkit/physics/environment/gravitational/earth"
ENV OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_REMOTE_URL "https://storage.googleapis.com/loft-orbital-open-space-toolkit/physics/environment/gravitational/earth/"
ENV OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_ENABLED true
ENV OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY "/open-space-toolkit/physics/environment/gravitational/earth"
ENV OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_REMOTE_URL "https://storage.googleapis.com/loft-orbital-open-space-toolkit/physics/environment/magnetic/earth/"

# Install Open Space Toolkit

ARG OSTK_CORE_VERSION
ARG OSTK_IO_VERSION
ARG OSTK_MATHEMATICS_VERSION
ARG OSTK_PHYSICS_VERSION

RUN pip install \
    open-space-toolkit-core==${OSTK_CORE_VERSION} \
    open-space-toolkit-io==${OSTK_IO_VERSION} \
    open-space-toolkit-mathematics==${OSTK_MATHEMATICS_VERSION} \
    open-space-toolkit-physics==${OSTK_PHYSICS_VERSION} \
    open-space-toolkit-astrodynamics

# Restore user

USER ${NB_UID}

# Disable token

CMD ["start-notebook.sh", "--NotebookApp.token=''"]

################################################################################################################################################################
