################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           Makefile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

project_name := astrodynamics
project_version := $(shell git describe --tags --always)
python_version := 3.8
project_name_python_shared_object := OpenSpaceToolkitAstrodynamicsPy.cpython-38-x86_64-linux-gnu

docker_registry_path := openspacecollective
docker_image_repository := $(docker_registry_path)/open-space-toolkit-$(project_name)
docker_image_version := $(project_version)

docker_development_image_repository := $(docker_image_repository)-development
docker_release_image_cpp_repository := $(docker_image_repository)-cpp
docker_release_image_python_repository := $(docker_image_repository)-python
docker_release_image_jupyter_repository := $(docker_image_repository)-jupyter

jupyter_notebook_image_repository := jupyter/scipy-notebook:python-3.8.8
jupyter_notebook_port := 9005

################################################################################################################################################################

pull: ## Pull all images

	@ echo "Pulling images..."

	@ make pull-development-images
	@ make pull-release-images

pull-development-images: ## Pull development images

	@ echo "Pulling development images..."

	@ make pull-development-image-debian
	@ make pull-development-image-fedora

pull-development-image-debian: target := debian
pull-development-image-fedora: target := fedora

pull-development-image-debian pull-development-image-fedora: _pull-development-image

_pull-development-image:

	@ echo "Pulling [$(target)] development image..."

	docker pull $(docker_development_image_repository):$(docker_image_version)-$(target) || true
	docker pull $(docker_development_image_repository):latest-$(target) || true

pull-release-images: ## Pull release images

	@ echo "Pull release images..."

	@ make pull-release-image-cpp-debian
	@ make pull-release-image-cpp-fedora

	@ make pull-release-image-python-debian
	@ make pull-release-image-python-fedora

	@ make pull-release-image-jupyter

pull-release-image-cpp-debian: target := debian
pull-release-image-cpp-fedora: target := fedora

pull-release-image-cpp-debian pull-release-image-cpp-fedora: _pull-release-image-cpp

_pull-release-image-cpp:

	@ echo "Pull [$(target)] C++ release image..."

	docker pull $(docker_release_image_cpp_repository):$(docker_image_version)-$(target) || true
	docker pull $(docker_release_image_cpp_repository):latest-$(target) || true

pull-release-image-python-debian: target := debian
pull-release-image-python-fedora: target := fedora

pull-release-image-python-debian pull-release-image-python-fedora: _pull-release-image-python

_pull-release-image-python: _pull-development-image

	@ echo "Pulling [$(target)] Python release image..."

	docker pull $(docker_release_image_python_repository):$(docker_image_version)-$(target) || true
	docker pull $(docker_release_image_python_repository):latest-$(target) || true

pull-release-image-jupyter:

	@ echo "Pulling Jupyter Notebook release image..."

	docker pull $(docker_release_image_jupyter_repository):$(docker_image_version) || true
	docker pull $(docker_release_image_jupyter_repository):latest || true

################################################################################################################################################################

build: build-images ## Build all images

build-images: ## Build development and release images

	@ echo "Building images..."

	@ make build-development-images
	@ make build-release-images

build-development-images: ## Build development images

	@ echo "Building development images..."

	@ make build-development-image-debian
	@ make build-development-image-fedora

build-development-image-debian: target := debian
build-development-image-fedora: target := fedora

build-development-image-debian build-development-image-fedora: _build-development-image

_build-development-image: _pull-development-image

	@ echo "Building [$(target)] development image..."

	docker build \
		--cache-from=$(docker_development_image_repository):latest-$(target) \
		--file="$(CURDIR)/docker/development/$(target)/Dockerfile" \
		--tag=$(docker_development_image_repository):$(docker_image_version)-$(target) \
		--tag=$(docker_development_image_repository):latest-$(target) \
		--build-arg="VERSION=$(docker_image_version)" \
		"$(CURDIR)"

build-release-images: ## Build release images

	@ echo "Building release images..."

	@ make build-release-image-cpp-debian
	@ make build-release-image-cpp-fedora

	@ make build-release-image-python-debian
	@ make build-release-image-python-fedora

	@ make build-release-image-jupyter

build-release-image-cpp-debian: target := debian
build-release-image-cpp-fedora: target := fedora

build-release-image-cpp-debian build-release-image-cpp-fedora: _build-release-image-cpp

_build-release-image-cpp: _build-development-image _pull-release-image-cpp

	@ echo "Building [$(target)] C++ release image..."

	docker build \
		--cache-from=$(docker_release_image_cpp_repository):latest-$(target) \
		--file="$(CURDIR)/docker/release/$(target)/Dockerfile" \
		--tag=$(docker_release_image_cpp_repository):$(docker_image_version)-$(target) \
		--tag=$(docker_release_image_cpp_repository):latest-$(target) \
		--build-arg="VERSION=$(docker_image_version)" \
		--target=cpp-release \
		"$(CURDIR)"

build-release-image-python-debian: target := debian
build-release-image-python-fedora: target := fedora

build-release-image-python-debian build-release-image-python-fedora: _build-release-image-python

_build-release-image-python: _build-development-image _pull-release-image-python

	@ echo "Building [$(target)] Python release image..."

	docker build \
		--cache-from=$(docker_release_image_python_repository):latest-$(target) \
		--file="$(CURDIR)/docker/release/$(target)/Dockerfile" \
		--tag=$(docker_release_image_python_repository):$(docker_image_version)-$(target) \
		--tag=$(docker_release_image_python_repository):latest-$(target) \
		--build-arg="VERSION=$(docker_image_version)" \
		--target=python-release \
		"$(CURDIR)"

build-release-image-jupyter: pull-release-image-jupyter

	@ echo "Building Jupyter Notebook release image..."

	docker build \
		--cache-from=$(docker_release_image_jupyter_repository):latest \
		--file="$(CURDIR)/docker/jupyter/Dockerfile" \
		--tag=$(docker_release_image_jupyter_repository):$(docker_image_version) \
		--tag=$(docker_release_image_jupyter_repository):latest \
		--build-arg="JUPYTER_NOTEBOOK_IMAGE_REPOSITORY=$(jupyter_notebook_image_repository)" \
		"$(CURDIR)/docker/jupyter"

build-documentation: target := debian

build-documentation: _build-development-image ## Build documentation

	@ echo "Building [$(target)] documentation..."

	docker run \
		--rm \
		--volume="$(CURDIR):/app:delegated" \
		--volume="/app/build" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version)-$(target) \
		/bin/bash -c "cmake -DBUILD_UNIT_TESTS=OFF -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_DOCUMENTATION=ON .. \
		&& make docs"

build-packages: ## Build packages

	@ echo "Building packages..."

	@ make build-packages-cpp
	@ make build-packages-python

build-packages-cpp: ## Build C++ packages

	@ echo "Building C++ packages..."

	@ make build-packages-cpp-debian
	@ make build-packages-cpp-fedora

build-packages-cpp-debian: target := debian
build-packages-cpp-fedora: target := fedora

build-packages-cpp-debian: package_generator := DEB
build-packages-cpp-fedora: package_generator := RPM

build-packages-cpp-debian: package_extension := deb
build-packages-cpp-fedora: package_extension := rpm

build-packages-cpp-debian build-packages-cpp-fedora: _build-packages-cpp

_build-packages-cpp: _build-development-image

	@ echo "Building [$(target)] C++ packages..."

	docker run \
		--rm \
		--volume="$(CURDIR):/app:delegated" \
		--volume="/app/build" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version)-$(target) \
		/bin/bash -c "cmake -DBUILD_UNIT_TESTS=OFF -DBUILD_PYTHON_BINDINGS=OFF -DCPACK_GENERATOR=$(package_generator) .. \
		&& make package \
		&& mkdir -p /app/packages/cpp \
		&& mv /app/build/*.$(package_extension) /app/packages/cpp"

build-packages-python: ## Build Python packages

	@ echo "Building Python packages..."

	@ make build-packages-python-debian

build-packages-python-debian: target := debian
build-packages-python-fedora: target := fedora

build-packages-python-debian build-packages-python-fedora: _build-packages-python

_build-packages-python: _build-development-image

	@ echo "Building [$(target)] Python packages..."

	docker run \
		--rm \
		--volume="$(CURDIR):/app:delegated" \
		--volume="/app/build" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version)-$(target) \
		/bin/bash -c "cmake -DBUILD_UNIT_TESTS=OFF -DBUILD_PYTHON_BINDINGS=ON .. \
		&& make -j 4 \
		&& mkdir -p /app/packages/python \
		&& mv /app/build/bindings/python/dist/*.whl /app/packages/python"

################################################################################################################################################################

start-development: ## Start development environment

	@ echo "Starting development environment..."

	make start-development-debian

start-development-debian: target := debian
start-development-fedora: target := fedora

start-development-debian start-development-fedora: _start-development

_start-development: _build-development-image

_start-development-no-link:

	@ echo "Starting [$(target)] development environment..."

	docker run \
		-it \
		--rm \
		--privileged \
		--volume="$(CURDIR):/app:delegated" \
		--volume="$(CURDIR)/tools/development/helpers:/app/build/helpers:ro,delegated" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version)-$(target) \
		/bin/bash

_start-development-link:

	@ echo "Starting [$(target)] development environment (linked)..."

	@ target=$(target) "$(CURDIR)/tools/development/start.sh" --link

ifndef link
_start-development: _start-development-no-link
else
_start-development: _start-development-link
endif

start-python: ## Start Python runtime environment

	@ echo "Starting Python runtime environment..."

	@ make start-python-debian

start-python-debian: target := debian
start-python-fedora: target := fedora

start-python-debian start-python-fedora: _start-python

_start-python: _build-release-image-python

	@ echo "Starting [$(target)] Python runtime environment..."

	docker run \
		-it \
		--rm \
		$(docker_release_image_python_repository):$(docker_image_version)-$(target)

start-jupyter-notebook: build-release-image-jupyter ## Starting Jupyter Notebook environment

	@ echo "Starting Jupyter Notebook environment..."

	docker run \
		-it \
		--rm \
		--publish="$(jupyter_notebook_port):8888" \
		--volume="$(CURDIR)/bindings/python/docs:/home/jovyan/docs" \
		--workdir="/home/jovyan" \
		$(docker_release_image_jupyter_repository):$(docker_image_version) \
		bash -c "start-notebook.sh --NotebookApp.token=''"

debug-jupyter-notebook: build-release-image-jupyter

	@ echo "Debugging Jupyter Notebook environment..."

	docker run \
		-it \
		--rm \
		--publish="$(jupyter_notebook_port):8888" \
		--volume="$(CURDIR)/bindings/python/docs:/home/jovyan/docs" \
		--volume="$(CURDIR)/tutorials/python/notebooks:/home/jovyan/tutorials" \
		--volume="$(CURDIR)/lib/libopen-space-toolkit-$(project_name).so.0:/opt/conda/lib/python$(python_version)/site-packages/ostk/$(project_name)/libopen-space-toolkit-$(project_name).so.0:ro" \
		--volume="$(CURDIR)/lib/$(project_name_python_shared_object).so:/opt/conda/lib/python$(python_version)/site-packages/ostk/$(project_name)/$(project_name_python_shared_object).so:ro" \
		--workdir="/home/jovyan" \
		$(docker_release_image_jupyter_repository):$(docker_image_version) \
		bash -c "start-notebook.sh --NotebookApp.token=''"

################################################################################################################################################################

debug-development:

	@ echo "Debugging development environment..."

	make debug-development-debian

debug-development-debian: target := debian
debug-development-fedora: target := fedora

debug-development-debian debug-development-fedora: _debug-development

_debug-development: _build-development-image

	@ echo "Debugging [$(target)] development environment..."

	docker run \
		-it \
		--rm \
		$(docker_development_image_repository):$(docker_image_version)-$(target) \
		/bin/bash

debug-cpp-release-debian: target := debian
debug-cpp-release-fedora: target := fedora

debug-cpp-release-debian debug-cpp-release-fedora: _debug-cpp-release

_debug-cpp-release: _build-release-image-cpp

	@ echo "Debugging [$(target)] C++ release environment..."

	docker run \
		-it \
		--rm \
		--entrypoint=/bin/bash \
		$(docker_release_image_cpp_repository):$(docker_image_version)-$(target)

debug-python-release-debian: target := debian
debug-python-release-fedora: target := fedora

debug-python-release-debian debug-python-release-fedora: _debug-python-release

_debug-python-release: _build-release-image-python

	@ echo "Debugging [$(target)] Python release environment..."

	docker run \
		-it \
		--rm \
		--entrypoint=/bin/bash \
		$(docker_release_image_python_repository):$(docker_image_version)-$(target)

################################################################################################################################################################

test: ## Run tests

	@ echo "Running tests..."

	@ make test-unit
	@ make test-coverage

test-unit: ## Run unit tests

	@ echo "Running unit tests..."

	@ make test-unit-debian

test-unit-debian:

	@ echo "Running [debian] unit tests..."

	@ make test-unit-cpp-debian
	@ make test-unit-python-debian

test-unit-fedora:

	@ echo "Running [fedora] unit tests..."

	@ make test-unit-cpp-fedora
	@ make test-unit-python-fedora

test-unit-cpp-debian: target := debian
test-unit-cpp-fedora: target := fedora

test-unit-cpp-debian test-unit-cpp-fedora: _test-unit-cpp

_test-unit-cpp: _build-development-image

	@ echo "Running [$(target)] C++ unit tests..."

	docker run \
		--rm \
		--volume="$(CURDIR):/app:delegated" \
		--volume="/app/build" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version)-$(target) \
		/bin/bash -c "cmake -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_UNIT_TESTS=ON .. \
		&& make -j 4 \
		&& make test"

test-unit-python-debian: target := debian
test-unit-python-fedora: target := fedora

test-unit-python-debian test-unit-python-fedora: _test-unit-python

_test-unit-python: _build-release-image-python

	@ echo "Running [$(target)] Python unit tests..."

	docker run \
		--rm \
		--workdir=/usr/local/lib/python$(python_version)/site-packages/ostk/$(project_name) \
		--entrypoint="" \
		$(docker_release_image_python_repository):$(docker_image_version)-$(target) \
		/bin/bash -c "pip install pytest && pytest -sv ."

test-coverage: ## Run test coverage cpp

	@ echo "Running coverage tests..."

	@ make test-coverage-cpp

test-coverage-cpp:

	@ echo "Running C++ coverage tests..."

	@ make test-coverage-cpp-debian

test-coverage-cpp-debian: target := debian
test-coverage-cpp-fedora: target := fedora

test-coverage-cpp-debian test-coverage-cpp-fedora: _test-coverage-cpp

_test-coverage-cpp: _build-development-image

	@ echo "Running [$(target)] C++ coverage tests..."

	docker run \
		--rm \
		--volume="$(CURDIR):/app:delegated" \
		--volume="/app/build" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version)-$(target) \
		/bin/bash -c "cmake -DBUILD_UNIT_TESTS=ON -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_CODE_COVERAGE=ON .. \
		&& make -j 4 \
		&& make coverage \
		&& (rm -rf /app/coverage || true) \
		&& mkdir /app/coverage \
		&& mv /app/build/coverage* /app/coverage"

################################################################################################################################################################

clean: ## Clean

	@ echo "Cleaning up..."

	rm -rf "$(CURDIR)/build"
	rm -rf "$(CURDIR)/bin/"*.test*
	rm -rf "$(CURDIR)/docs/html"
	rm -rf "$(CURDIR)/docs/latex"
	rm -rf "$(CURDIR)/lib/"*.so*
	rm -rf "$(CURDIR)/coverage"
	rm -rf "$(CURDIR)/packages"
	rm -rf "$(CURDIR)/.open-space-toolkit"

################################################################################################################################################################

.PHONY: pull pull-development-images pull-development-image-debian pull-development-image-fedora _pull-development-image \
		build build-images \
		build-development-images build-development-image-debian build-development-image-fedora \
		pull-release-images pull-release-image-cpp-debian pull-release-image-cpp-fedora _pull-release-image-cpp \
		pull-release-image-python-debian pull-release-image-python-fedora _pull-release-image-python \
		pull-release-image-jupyter \
		build-release-image-cpp-debian build-release-image-python-debian build-release-image-python-fedora \
		build-release-image-jupyter \
		build-documentation \
		build-packages-cpp build-packages-cpp-debian build-packages-cpp-fedora \
		start-development start-development-debian start-development-fedora \
		start-python start-python-debian start-python-fedora \
		start-jupyter-notebook debug-jupyter-notebook \
		debug-development-debian debug-cpp-release-debian debug-python-release-debian \
		debug-development-fedora debug-cpp-release-fedora debug-python-release-fedora \
		test \
		test-unit test-unit-debian test-unit-fedora \
		test-unit-cpp-debian test-unit-cpp-fedora \
		test-unit-python-debian test-unit-python-fedora \
		test-coverage \
		test-coverage-cpp test-coverage-cpp-debian test-coverage-cpp-fedora \
		clean

################################################################################################################################################################

help:

	@grep -E '^[0-9a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.DEFAULT_GOAL := help

################################################################################################################################################################
