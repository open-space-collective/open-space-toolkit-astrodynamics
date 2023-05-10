# Apache License 2.0 

project_name := astrodynamics
project_version := $(shell git describe --tags --always)

docker_registry_path := openspacecollective
docker_image_repository := $(docker_registry_path)/open-space-toolkit-$(project_name)
docker_image_version := $(project_version)

docker_development_image_repository := $(docker_image_repository)-development
docker_release_image_cpp_repository := $(docker_image_repository)-cpp
docker_release_image_python_repository := $(docker_image_repository)-python
docker_release_image_jupyter_repository := $(docker_image_repository)-jupyter

jupyter_notebook_image_repository := jupyter/scipy-notebook:python-3.8.8
jupyter_notebook_port := 9005
jupyter_python_version := 3.8
jupyter_project_name_python_shared_object := OpenSpaceToolkitAstrodynamicsPy.cpython-38-x86_64-linux-gnu

clang_format_sources_path ?= $(shell find ~+ src/ include/ test/ bindings/python/src/ -name '*.cpp' -o -name '*.cxx' -o -name '*.hpp' -o -name '*.tpp')

pull: ## Pull all images

	@ echo "Pulling images..."

	@ make pull-development-image
	@ make pull-release-images

.PHONY: pull

pull-development-image: ## Pull development image

	@ echo "Pulling development image..."

	docker pull $(docker_development_image_repository):$(docker_image_version) || true
	docker pull $(docker_development_image_repository):latest || true

.PHONY: pull-development-image

pull-release-images: ## Pull release images

	@ echo "Pull release images..."

	@ make pull-release-image-cpp

	@ make pull-release-image-python

	@ make pull-release-image-jupyter

.PHONY: pull-release-images

pull-release-image-cpp:

	@ echo "Pull C++ release image..."

	docker pull $(docker_release_image_cpp_repository):$(docker_image_version) || true
	docker pull $(docker_release_image_cpp_repository):latest || true

.PHONY: pull-release-image-cpp

pull-release-image-python:

	@ echo "Pulling Python release image..."

	docker pull $(docker_release_image_python_repository):$(docker_image_version) || true
	docker pull $(docker_release_image_python_repository):latest || true

.PHONY: pull-release-image-python

pull-release-image-jupyter:

	@ echo "Pulling Jupyter Notebook release image..."

	docker pull $(docker_release_image_jupyter_repository):$(docker_image_version) || true
	docker pull $(docker_release_image_jupyter_repository):latest || true

.PHONY: pull-release-image-jupyter

build: build-images ## Build all images

build-images: ## Build development and release images

	@ echo "Building images..."

	@ make build-development-image
	@ make build-release-images

.PHONY: build-images

build-development-image: pull-development-image ## Build development image

	@ echo "Building development image..."

	docker build \
		--cache-from=$(docker_development_image_repository):latest \
		--file="$(CURDIR)/docker/development/Dockerfile" \
		--tag=$(docker_development_image_repository):$(docker_image_version) \
		--tag=$(docker_development_image_repository):latest \
		--build-arg="VERSION=$(docker_image_version)" \
		"$(CURDIR)"

.PHONY: build-development-image

build-release-images: ## Build release images

	@ echo "Building release images..."

	@ make build-release-image-cpp

	@ make build-release-image-python

	@ make build-release-image-jupyter

.PHONY: build-release-images

build-release-image-cpp: build-development-image pull-release-image-cpp

	@ echo "Building C++ release image..."

	docker build \
		--cache-from=$(docker_release_image_cpp_repository):latest \
		--file="$(CURDIR)/docker/release/Dockerfile" \
		--tag=$(docker_release_image_cpp_repository):$(docker_image_version) \
		--tag=$(docker_release_image_cpp_repository):latest \
		--build-arg="VERSION=$(docker_image_version)" \
		--target=cpp-release \
		"$(CURDIR)"
	
.PHONY: build-release-image-cpp

build-release-image-python: build-development-image pull-release-image-python

	@ echo "Building Python release image..."

	docker build \
		--cache-from=$(docker_release_image_python_repository):latest \
		--file="$(CURDIR)/docker/release/Dockerfile" \
		--tag=$(docker_release_image_python_repository):$(docker_image_version) \
		--tag=$(docker_release_image_python_repository):latest \
		--build-arg="VERSION=$(docker_image_version)" \
		--target=python-release \
		"$(CURDIR)"

.PHONY: build-release-image-python

build-release-image-jupyter: pull-release-image-jupyter

	@ echo "Building Jupyter Notebook release image..."

	docker build \
		--cache-from=$(docker_release_image_jupyter_repository):latest \
		--file="$(CURDIR)/docker/jupyter/Dockerfile" \
		--tag=$(docker_release_image_jupyter_repository):$(docker_image_version) \
		--tag=$(docker_release_image_jupyter_repository):latest \
		--build-arg="JUPYTER_NOTEBOOK_IMAGE_REPOSITORY=$(jupyter_notebook_image_repository)" \
		"$(CURDIR)/docker/jupyter"

.PHONY: build-release-image-jupyter

build-documentation: build-development-image ## Build documentation

	@ echo "Building documentation..."

	docker run \
		--rm \
		--volume="$(CURDIR):/app:delegated" \
		--volume="/app/build" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version) \
		/bin/bash -c "cmake -DBUILD_UNIT_TESTS=OFF -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_DOCUMENTATION=ON .. \
		&& make docs"

.PHONY: build-documentation

build-packages: ## Build packages

	@ echo "Building packages..."

	@ make build-packages-cpp
	@ make build-packages-python

.PHONY: build-packages

build-packages-cpp: build-development-image ## Build C++ packages

	@ echo "Building C++ packages..."

	docker run \
		--rm \
		--volume="$(CURDIR):/app:delegated" \
		--volume="/app/build" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version) \
		/bin/bash -c "cmake -DBUILD_UNIT_TESTS=OFF -DBUILD_PYTHON_BINDINGS=OFF -DCPACK_GENERATOR=DEB .. \
		&& make package \
		&& mkdir -p /app/packages/cpp \
		&& mv /app/build/*.deb /app/packages/cpp"

.PHONY: build-packages-cpp

build-packages-python: build-development-image ## Build Python packages

	@ echo "Building Python packages..."

	docker run \
		--rm \
		--volume="$(CURDIR):/app:delegated" \
		--volume="/app/build" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version) \
		/bin/bash -c "cmake -DBUILD_UNIT_TESTS=OFF -DBUILD_PYTHON_BINDINGS=ON .. \
		&& make -j 4 \
		&& mkdir -p /app/packages/python \
		&& mv /app/build/bindings/python/dist/*.whl /app/packages/python"

.PHONY: build-packages-python

start-development-no-link: build-development-image ## Start development environment

	@ echo "Starting development environment..."

	docker run \
		-it \
		--rm \
		--privileged \
		--volume="$(CURDIR):/app:delegated" \
		--volume="$(CURDIR)/tools/development/helpers:/app/build/helpers:ro,delegated" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version) \
		/bin/bash

.PHONY: start-development-no-link

start-development-link: ## Start linked development environment

	$(if $(links), , $(error "You need to provide the links to the C++ dependency repositories you want to link with, separated by white spaces. For example: make start-development-link links="/home/OSTk/open-space-toolkit-io /home/OSTk/open-space-toolkit-core"))

	@ echo "Starting development environment (linked)..."

	@ project_directory="$(CURDIR)" docker_development_image_repository=$(docker_development_image_repository) docker_image_version=$(docker_image_version) "$(CURDIR)/tools/development/start.sh" --link $(links)

.PHONY: start-development-link

ifndef link
start-development: start-development-no-link
else
start-development: start-development-link
endif

.PHONY: start-development

start-python: build-release-image-python ## Start Python runtime environment

	@ echo "Starting Python runtime environment..."

	docker run \
		-it \
		--rm \
		$(docker_release_image_python_repository):$(docker_image_version)

.PHONY: start-python

start-jupyter-notebook: build-release-image-jupyter ## Starting Jupyter Notebook environment

	@ echo "Starting Jupyter Notebook environment..."

	docker run \
		-it \
		--rm \
		--publish="$(jupyter_notebook_port):8888" \
		--volume="$(CURDIR)/bindings/python/docs:/home/jovyan/docs" \
		--workdir="/home/jovyan" \
		$(docker_release_image_jupyter_repository):$(docker_image_version) \
		bash -c "start-notebook.sh --ServerApp.token=''"

.PHONY: start-jupyter-notebook

debug-jupyter-notebook: build-release-image-jupyter

	@ echo "Debugging Jupyter Notebook environment..."

	docker run \
		-it \
		--rm \
		--publish="$(jupyter_notebook_port):8888" \
		--volume="$(CURDIR)/bindings/python/docs:/home/jovyan/docs" \
		--volume="$(CURDIR)/tutorials/python/notebooks:/home/jovyan/tutorials" \
		--volume="$(CURDIR)/lib/libopen-space-toolkit-$(project_name).so.0:/opt/conda/lib/python$(jupyter_python_version)/site-packages/ostk/$(project_name)/libopen-space-toolkit-$(project_name).so.0:ro" \
		--volume="$(CURDIR)/lib/$(jupyter_project_name_python_shared_object).so:/opt/conda/lib/python$(jupyter_python_version)/site-packages/ostk/$(project_name)/$(jupyter_project_name_python_shared_object).so:ro" \
		--workdir="/home/jovyan" \
		$(docker_release_image_jupyter_repository):$(docker_image_version) \
		bash -c "start-notebook.sh --ServerApp.token=''"

.PHONY: debug-jupyter-notebook

debug-development: build-development-image ## Debug development environment

	@ echo "Debugging development environment..."

	docker run \
		-it \
		--rm \
		$(docker_development_image_repository):$(docker_image_version) \
		/bin/bash

.PHONY: debug-development

debug-cpp-release: build-release-image-cpp ## Debug C++ release environment

	@ echo "Debugging C++ release environment..."

	docker run \
		-it \
		--rm \
		--entrypoint=/bin/bash \
		$(docker_release_image_cpp_repository):$(docker_image_version)

.PHONY: debug-cpp-release

debug-python-release: build-release-image-python ## Debug Python release environment

	@ echo "Debugging Python release environment..."

	docker run \
		-it \
		--rm \
		--entrypoint=/bin/bash \
		$(docker_release_image_python_repository):$(docker_image_version)

.PHONY: debug-python-release

format: build-development-image ## Format all of the source code with the rules in .clang-format

	docker run \
		--rm \
		--volume="$(CURDIR):/app" \
		--workdir=/app \
		--user="$(shell id -u):$(shell id -g)" \
		$(docker_development_image_repository):$(docker_image_version) \
		clang-format -i -style=file:thirdparty/clang/.clang-format ${clang_format_sources_path}

.PHONY: format

format-check: build-development-image ## Runs the clang-format tool to check the code against rules and formatting

	docker run \
		--rm \
		--volume="$(CURDIR):/app" \
		--workdir=/app \
		--user="$(shell id -u):$(shell id -g)" \
		$(docker_development_image_repository):$(docker_image_version) \
		clang-format -Werror --dry-run -style=file:thirdparty/clang/.clang-format ${clang_format_sources_path}

.PHONY: format-check

format-python: build-development-image  ## Runs the black format tool against python code

	docker run \
		--rm \
		--volume="$(CURDIR):/app" \
		--workdir=/app \
		$(docker_development_image_repository):$(docker_image_version) \
		/bin/bash -c "python3.11 -m black --line-length=90 bindings/python/"

.PHONY: format-python

test: ## Run tests

	@ echo "Running tests..."

	@ make test-unit
	@ make test-coverage

.PHONY: test

test-unit: ## Run unit tests

	@ echo "Running unit tests..."

	@ make test-unit-cpp
	@ make test-unit-python

.PHONY: test-unit

test-unit-cpp: build-development-image ## Run C++ unit tests

	@ echo "Running C++ unit tests..."

	docker run \
		--rm \
		--volume="$(CURDIR):/app:delegated" \
		--volume="$(CURDIR)/share/OpenSpaceToolkit:/usr/local/share/OpenSpaceToolkit:delegated" \
		--volume="/app/build" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version) \
		/bin/bash -c "cmake -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_UNIT_TESTS=ON .. \
		&& make -j 4 \
		&& make test"

.PHONY: test-unit-cpp

test-unit-python: build-release-image-python ## Run Python unit tests

	@ echo "Running Python unit tests..."

	docker run \
		--rm \
		--volume="$(CURDIR)/share/OpenSpaceToolkit:/usr/local/share/OpenSpaceToolkit:delegated" \
		--workdir=/usr/local/lib/python3.11/site-packages/ostk/$(project_name) \
		--entrypoint="" \
		$(docker_release_image_python_repository):$(docker_image_version) \
		/bin/bash -c "pip install pytest && pytest -sv ."

.PHONY: test-unit-python

test-coverage: ## Run test coverage cpp

	@ echo "Running coverage tests..."

	@ make test-coverage-cpp

.PHONY: test-coverage

test-coverage-cpp: build-development-image

	@ echo "Running C++ coverage tests..."

	docker run \
		--rm \
		--volume="$(CURDIR):/app:delegated" \
		--volume="$(CURDIR)/share/OpenSpaceToolkit:/usr/local/share/OpenSpaceToolkit:delegated" \
		--volume="/app/build" \
		--workdir=/app/build \
		$(docker_development_image_repository):$(docker_image_version) \
		/bin/bash -c "cmake -DBUILD_UNIT_TESTS=ON -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_CODE_COVERAGE=ON .. \
		&& make -j 4 \
		&& make coverage \
		&& (rm -rf /app/coverage || true) \
		&& mkdir /app/coverage \
		&& mv /app/build/coverage* /app/coverage"

.PHONY: test-coverage-cpp

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

.PHONY: clean

help:

	@grep -E '^[0-9a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

export DOCKER_BUILDKIT = 1

.DEFAULT_GOAL := help
