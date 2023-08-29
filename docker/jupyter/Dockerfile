# Apache License 2.0

ARG JUPYTER_NOTEBOOK_IMAGE_REPOSITORY=jupyter/scipy-notebook:x86_64-python-3.11.3

FROM ${JUPYTER_NOTEBOOK_IMAGE_REPOSITORY}

LABEL maintainer="lucas@loftorbital.com"

ENV JUPYTER_ENABLE_LAB yes

# Set user to root

USER root

# Install Python utilities

RUN apt-get update \
 && apt-get install -y curl git-lfs \
 && rm -rf /var/lib/apt/lists/*

RUN pip install --quiet "numpy" "pandas" "plotly" "ipywidgets>=7.6" "ipympl>=0.9.0"

RUN jupyter labextension install --no-build \
        @jupyterlab/shortcuts-extension

COPY ./shortcuts-extension /home/jovyan/.jupyter/lab/user-settings/@jupyterlab/shortcuts-extension

RUN chown -R ${NB_UID}:${NB_GID} /home/jovyan

# Add OSTk Data Settings

RUN git clone --branch main --single-branch --depth=1 https://github.com/open-space-collective/open-space-toolkit-data.git /usr/local/share/open-space-toolkit-data

ENV OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data/coordinate/frame/providers/iers"
ENV OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data/environment/ephemerides/spice"
ENV OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data/environment/gravitational/earth"
ENV OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data/environment/magnetic/earth"
ENV OSTK_PHYSICS_ENVIRONMENT_ATMOSPHERIC_EARTH_MANAGER_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data/environment/atmospheric/earth"
ENV OSTK_PHYSICS_DATA_MANIFEST_LOCAL_REPOSITORY="/usr/local/share/open-space-toolkit-data/data"

RUN chown -R ${NB_UID}:${NB_GID} /usr/local/share/open-space-toolkit-data

# Install Open Space Toolkit

RUN pip install open-space-toolkit-astrodynamics

# Restore user

USER ${NB_UID}

# Disable token

CMD ["start-notebook.sh", "--NotebookApp.token=''"]
