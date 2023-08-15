# Apache License 2.0

ARG JUPYTER_NOTEBOOK_IMAGE_REPOSITORY=jupyter/scipy-notebook:x86_64-python-3.11.3

FROM ${JUPYTER_NOTEBOOK_IMAGE_REPOSITORY}

LABEL maintainer="lucas@loftorbital.com"

ENV JUPYTER_ENABLE_LAB yes

# Set user to root

USER root

# Install Python utilities

RUN apt-get update \
 && apt-get install -y curl \
 && rm -rf /var/lib/apt/lists/*

RUN pip install --quiet "numpy" "pandas" "plotly" "ipywidgets>=7.6" "ipympl>=0.9.0"

RUN jupyter labextension install --no-build \
        @jupyterlab/shortcuts-extension

COPY ./shortcuts-extension /home/jovyan/.jupyter/lab/user-settings/@jupyterlab/shortcuts-extension

RUN chown -R ${NB_UID}:${NB_GID} /home/jovyan

# Add OSTk Data Settings

COPY --from=us.gcr.io/loft-orbital-public/software/open-space-toolkit/data:latest /open-space-toolkit /open-space-toolkit

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

RUN pip install open-space-toolkit-astrodynamics

# Restore user

USER ${NB_UID}

# Disable token

CMD ["start-notebook.sh", "--NotebookApp.token=''"]