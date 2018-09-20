################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/python/docker/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

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
 && python -m pip install --quiet plotly

COPY ./shortcuts-extension /home/jovyan/.jupyter/lab/user-settings/@jupyterlab/shortcuts-extension

RUN chown -R $NB_UID:$NB_UID /home/jovyan/.jupyter

USER $NB_UID

################################################################################################################################################################