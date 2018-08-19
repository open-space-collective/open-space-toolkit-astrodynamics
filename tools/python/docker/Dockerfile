################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           tools/python/docker/Dockerfile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

################################################################################################################################################################

FROM jupyter/scipy-notebook:latest

LABEL maintainer="lucas@loftorbital.com"

RUN pip install --index-url https://test.pypi.org/simple/ LibraryCorePy==0.1.7

RUN pip install --index-url https://test.pypi.org/simple/ LibraryMathematicsPy==0.1.1.17

RUN pip install --index-url https://test.pypi.org/simple/ LibraryAstrodynamicsPy==0.1.1

################################################################################################################################################################