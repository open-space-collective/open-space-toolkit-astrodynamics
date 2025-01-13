# Apache License 2.0

ARG JUPYTER_NOTEBOOK_IMAGE_REPOSITORY=jupyter/scipy-notebook:x86_64-python-3.11.3

FROM ${JUPYTER_NOTEBOOK_IMAGE_REPOSITORY}

LABEL maintainer="lucas@loftorbital.com"

ENV JUPYTER_ENABLE_LAB="yes"

# Set user to root

USER root

# Install Python utilities

RUN apt-get update \
 && apt-get install -y curl git-lfs \
 && rm -rf /var/lib/apt/lists/*

RUN pip install --quiet "numpy~=2.0" "pandas" "plotly" "ipywidgets>=7.6" "ipympl>=0.9.0"

# Install JupyterLab extensions

RUN jupyter labextension install --no-build \
 @jupyterlab/shortcuts-extension

COPY ./shortcuts-extension /home/jovyan/.jupyter/lab/user-settings/@jupyterlab/shortcuts-extension

RUN chown -R ${NB_UID}:${NB_GID} /home/jovyan

# Install OSTk data

ARG OSTK_DATA_LOCAL_CACHE="/var/cache/open-space-toolkit-data"
ENV OSTK_DATA_LOCAL_CACHE="${OSTK_DATA_LOCAL_CACHE}"
ENV OSTK_PHYSICS_DATA_LOCAL_REPOSITORY="${OSTK_DATA_LOCAL_CACHE}/data"

RUN git clone --branch v1 --single-branch --depth=1 https://github.com/open-space-collective/open-space-toolkit-data.git ${OSTK_DATA_LOCAL_CACHE} && chmod -R g+w ${OSTK_DATA_LOCAL_CACHE}

RUN chown -R ${NB_UID}:${NB_GID} ${OSTK_DATA_LOCAL_CACHE}

## Install libstdc++ including GLIBCXX_3.4.32 (For OSTk), can be removed when Ubuntu version >= 24.0 or debian >= 13.0

RUN apt-get update \
 && apt-get install -y software-properties-common \
 && add-apt-repository ppa:ubuntu-toolchain-r/test \
 && apt-get update \
 && apt-get install -y libstdc++6 \
 && rm -rf /var/lib/apt/lists/*

# Install Open Space Toolkit

RUN pip install open-space-toolkit-astrodynamics

# Copy startup script

RUN mkdir -p /usr/local/bin/before-notebook.d
COPY startup.sh /usr/local/bin/before-notebook.d/

# Restore user

USER ${NB_UID}

# Disable token

CMD ["start-notebook.sh", "--NotebookApp.token=''"]
