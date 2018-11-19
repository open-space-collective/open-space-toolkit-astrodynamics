################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/python/docker/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

FROM jupyter/scipy-notebook:latest

LABEL maintainer="lucas@loftorbital.com"

USER root

RUN ln -s /opt/conda/include/python3.6m /opt/conda/include/python3.6

RUN cd /tmp \
 && wget --quiet http://sourceforge.net/projects/boost/files/boost/1.68.0/boost_1_68_0.tar.gz \
 && tar -xf boost_1_68_0.tar.gz \
 && cd boost_1_68_0 \
 && ./bootstrap.sh --with-python=/opt/conda/bin/python3 --with-python-version=3.6 --with-python-root=/opt/conda/lib/python3.6 \
 && echo "using python : 3.6 : /usr : /opt/conda/include/python3.6m ;" >> project-config.jam \
 && ./b2 --with-python -j 8 install \
 && rm -rf /tmp/boost_1_68_0

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
 && chown -R $NB_UID:$NB_UID /opt/conda/bin/orca*

COPY ./shortcuts-extension /home/jovyan/.jupyter/lab/user-settings/@jupyterlab/shortcuts-extension

RUN chown -R $NB_UID:$NB_UID /home/jovyan/.jupyter

USER $NB_UID

################################################################################################################################################################