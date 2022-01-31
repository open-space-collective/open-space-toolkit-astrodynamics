################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           Makefile
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

export project_name := open-space-toolkit-astrodynamics
export project_version := $(shell git describe --tags --always)
export project_directory := $(shell git rev-parse --show-toplevel)

export docker_registry_path := openspacecollective
export docker_image_repository := $(docker_registry_path)/$(project_name)
export docker_image_version := $(project_version)

export development_base_image_version := 0.2.1

export docker_development_image_repository := $(docker_image_repository)-development
export docker_release_image_cpp_repository := $(docker_image_repository)-cpp
export docker_release_image_python_repository := $(docker_image_repository)-python
export docker_release_image_jupyter_repository := $(docker_image_repository)-jupyter

export jupyter_notebook_image_repository := jupyter/scipy-notebook:latest
export jupyter_notebook_port := 9005

export open_space_toolkit_core_version := 0.4.3
export open_space_toolkit_io_version := 0.4.3
export open_space_toolkit_mathematics_version := 0.4.2
export open_space_toolkit_physics_version := 0.5.12

export open_space_toolkit_core_directory := $(project_directory)/../open-space-toolkit-core
export open_space_toolkit_io_directory := $(project_directory)/../open-space-toolkit-io
export open_space_toolkit_mathematics_directory := $(project_directory)/../open-space-toolkit-mathematics
export open_space_toolkit_physics_directory := $(project_directory)/../open-space-toolkit-physics

export OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY := /app/share/coordinate/frame/providers/iers
export OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY := /app/share/environment/ephemerides/spice
export OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY := /app/share/environment/gravitational/earth
export OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY := /app/share/environment/magnetic/earth

export ci_build_number := $(TRAVIS_BUILD_NUMBER)
export ci_commit := $(TRAVIS_COMMIT)
export ci_doc_repo_name := $(project_name)
export ci_doc_repo_token := $(GITHUB_API_KEY)
export ci_doc_repo_ref := github.com/open-space-collective/$(ci_doc_repo_name).git
export ci_doc_repo_user_name := "Travis CI"
export ci_doc_repo_user_email := "travis@travis-ci.org"
export ci_codecov_token := $(CODECOV_TOKEN)

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
	--file="$(project_directory)/docker/development/$(target)/Dockerfile" \
	--tag=$(docker_development_image_repository):$(docker_image_version)-$(target) \
	--tag=$(docker_development_image_repository):latest-$(target) \
	--build-arg="BASE_IMAGE_VERSION=$(development_base_image_version)" \
	--build-arg="VERSION=$(docker_image_version)" \
	--build-arg="OSTK_CORE_VERSION=$(open_space_toolkit_core_version)" \
	--build-arg="OSTK_IO_VERSION=$(open_space_toolkit_io_version)" \
	--build-arg="OSTK_MATHEMATICS_VERSION=$(open_space_toolkit_mathematics_version)" \
	--build-arg="OSTK_PHYSICS_VERSION=$(open_space_toolkit_physics_version)" \
	"$(project_directory)"

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
	--file="$(project_directory)/docker/release/$(target)/Dockerfile" \
	--tag=$(docker_release_image_cpp_repository):$(docker_image_version)-$(target) \
	--tag=$(docker_release_image_cpp_repository):latest-$(target) \
	--build-arg="VERSION=$(docker_image_version)" \
	--target=cpp-release \
	"$(project_directory)"

build-release-image-python-debian: target := debian
build-release-image-python-fedora: target := fedora

build-release-image-python-debian build-release-image-python-fedora: _build-release-image-python

_build-release-image-python: _build-development-image _pull-release-image-python

	@ echo "Building [$(target)] Python release image..."

	docker build \
	--cache-from=$(docker_release_image_python_repository):latest-$(target) \
	--file="$(project_directory)/docker/release/$(target)/Dockerfile" \
	--tag=$(docker_release_image_python_repository):$(docker_image_version)-$(target) \
	--tag=$(docker_release_image_python_repository):latest-$(target) \
	--build-arg="VERSION=$(docker_image_version)" \
	--target=python-release \
	"$(project_directory)"

build-release-image-jupyter: pull-release-image-jupyter

	@ echo "Building Jupyter Notebook release image..."

	docker build \
	--cache-from=$(docker_release_image_jupyter_repository):latest \
	--file="$(project_directory)/docker/jupyter/Dockerfile" \
	--tag=$(docker_release_image_jupyter_repository):$(docker_image_version) \
	--tag=$(docker_release_image_jupyter_repository):latest \
	--build-arg="JUPYTER_NOTEBOOK_IMAGE_REPOSITORY=$(jupyter_notebook_image_repository)" \
	"$(project_directory)/docker/jupyter"

build-documentation: target := debian

build-documentation: _build-development-image ## Build documentation

	@ echo "Building [$(target)] documentation..."

	docker run \
	--rm \
	--volume="$(project_directory):/app:delegated" \
	--volume="/app/build" \
	--workdir=/app/build \
	$(docker_development_image_repository):$(docker_image_version)-$(target) \
	/bin/bash -c "cmake -DBUILD_DOCUMENTATION=ON .. && make docs"

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
	--volume="$(project_directory):/app:delegated" \
	--volume="/app/build" \
	--workdir=/app/build \
	$(docker_development_image_repository):$(docker_image_version)-$(target) \
	/bin/bash -c "cmake -DBUILD_UNIT_TESTS=OFF -DBUILD_PYTHON_BINDINGS=OFF -DCPACK_GENERATOR=$(package_generator) .. \
	&& make package && mkdir -p /app/packages/cpp && mv /app/build/*.$(package_extension) /app/packages/cpp"

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
	--volume="$(project_directory):/app:delegated" \
	--volume="/app/build" \
	--workdir=/app/build \
	$(docker_development_image_repository):$(docker_image_version)-$(target) \
	/bin/bash -c "cmake -DBUILD_UNIT_TESTS=OFF -DBUILD_PYTHON_BINDINGS=ON .. && make -j 4 \
	&& mkdir -p /app/packages/python && mv /app/build/bindings/python/dist/*.whl /app/packages/python"

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
	--volume="$(project_directory):/app:delegated" \
	--volume="${project_directory}/tools/development/helpers:/app/build/helpers:ro,delegated" \
	--workdir=/app/build \
	$(docker_development_image_repository):$(docker_image_version)-$(target) \
	/bin/bash

_start-development-link:

	@ echo "Starting [$(target)] development environment (linked)..."

	@ target=$(target) "$(project_directory)/tools/development/start.sh" --link

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
	--publish="${jupyter_notebook_port}:8888" \
	--volume="${project_directory}/bindings/python/docs:/home/jovyan/docs" \
	--volume="${project_directory}/tutorials/python/notebooks:/home/jovyan/tutorials" \
	--workdir="/home/jovyan" \
	$(docker_release_image_jupyter_repository):$(docker_image_version) \
	bash -c "start-notebook.sh --NotebookApp.token=''"

debug-jupyter-notebook: build-release-image-jupyter

	@ echo "Debugging Jupyter Notebook environment..."

	docker run \
	-it \
	--rm \
	--publish="${jupyter_notebook_port}:8888" \
	--volume="${project_directory}/bindings/python/docs:/home/jovyan/docs" \
	--volume="${project_directory}/tutorials/python/notebooks:/home/jovyan/tutorials" \
	--volume="${project_directory}/lib/libopen-space-toolkit-astrodynamics.so.0:/opt/conda/lib/python3.7/site-packages/ostk/astrodynamics/libopen-space-toolkit-astrodynamics.so.0:ro" \
	--volume="${project_directory}/lib/OpenSpaceToolkitAstrodynamicsPy.so:/opt/conda/lib/python3.7/site-packages/ostk/astrodynamics/OpenSpaceToolkitAstrodynamicsPy.so:ro" \
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
	--volume="$(project_directory):/app:delegated" \
	--volume="/app/build" \
	--workdir=/app/build \
	--env=OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY \
	--env=OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY \
	--env=OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY \
	--env=OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY \
	$(docker_development_image_repository):$(docker_image_version)-$(target) \
	/bin/bash -c "cmake -DBUILD_UNIT_TESTS=ON .. && make -j 4 && make test"

test-unit-python-debian: target := debian
test-unit-python-fedora: target := fedora

test-unit-python-debian test-unit-python-fedora: _test-unit-python

_test-unit-python: _build-release-image-python

	@ echo "Running [$(target)] Python unit tests..."

	docker run \
	--rm \
	--workdir=/usr/local/lib/python3.8/site-packages/ostk/astrodynamics \
	--entrypoint="" \
	--volume="$(project_directory)/share:/app/share" \
	--env=OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY \
	--env=OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY \
	--env=OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY \
	--env=OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY \
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
	--volume="$(project_directory):/app:delegated" \
	--volume="/app/build" \
	--workdir=/app/build \
	--env=OSTK_PHYSICS_COORDINATE_FRAME_PROVIDERS_IERS_MANAGER_LOCAL_REPOSITORY \
	--env=OSTK_PHYSICS_ENVIRONMENT_EPHEMERIDES_SPICE_MANAGER_LOCAL_REPOSITORY \
	--env=OSTK_PHYSICS_ENVIRONMENT_GRAVITATIONAL_EARTH_MANAGER_LOCAL_REPOSITORY \
	--env=OSTK_PHYSICS_ENVIRONMENT_MAGNETIC_EARTH_MANAGER_LOCAL_REPOSITORY \
	$(docker_development_image_repository):$(docker_image_version)-$(target) \
	/bin/bash -c "cmake -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_CODE_COVERAGE=ON .. && make -j 4 && make coverage \
	&& (rm -rf /app/coverage || true) && mkdir /app/coverage && mv /app/build/coverage* /app/coverage"

################################################################################################################################################################

deploy: ## Deploy everything

	@ echo "Deploying..."

	@ make deploy-coverage-cpp-results
	@ make deploy-images
	@ make deploy-packages
	@ make deploy-documentation

deploy-images: ## Deploy packages

	@ echo "Deploying images..."

	@ make deploy-development-images
	@ make deploy-release-images

deploy-development-images: ## Deploy development images

	@ echo "Deploying development images..."

	@ make _deploy-development-image target=debian
	@ make _deploy-development-image target=fedora

_deploy-development-image: _build-development-image

	@ echo "Deploying [$(target)] development image..."

	docker push $(docker_development_image_repository):$(docker_image_version)-$(target)
	docker push $(docker_development_image_repository):latest-$(target)

deploy-release-images: ## Deploy release images

	@ echo "Deploying release images..."

	@ make _deploy-release-images target=debian
	@ make _deploy-release-images target=fedora

	docker tag $(docker_release_image_cpp_repository):$(docker_image_version)-debian $(docker_release_image_cpp_repository):$(docker_image_version)
	docker tag $(docker_release_image_cpp_repository):latest-debian $(docker_release_image_cpp_repository):latest

	docker tag $(docker_release_image_python_repository):$(docker_image_version)-debian $(docker_release_image_python_repository):$(docker_image_version)
	docker tag $(docker_release_image_python_repository):latest-debian $(docker_release_image_python_repository):latest

	docker push $(docker_release_image_cpp_repository):$(docker_image_version)
	docker push $(docker_release_image_cpp_repository):latest

	docker push $(docker_release_image_python_repository):$(docker_image_version)
	docker push $(docker_release_image_python_repository):latest

	@ make deploy-release-image-jupyter

_deploy-release-images: _build-release-image-cpp _build-release-image-python

	@ echo "Deploying [$(target)] release images..."

	docker push $(docker_release_image_cpp_repository):$(docker_image_version)-$(target)
	docker push $(docker_release_image_cpp_repository):latest-$(target)

	docker push $(docker_release_image_python_repository):$(docker_image_version)-$(target)
	docker push $(docker_release_image_python_repository):latest-$(target)

deploy-release-image-jupyter: build-release-image-jupyter

	@ echo "Deploying Jupyter Notebook release image..."

	docker push $(docker_release_image_jupyter_repository):$(docker_image_version)
	docker push $(docker_release_image_jupyter_repository):latest

deploy-packages: ## Deploy packages

	@ echo "Deploying packages..."

	@ make deploy-packages-debian
	@ make deploy-packages-fedora

deploy-packages-debian: target := debian
deploy-packages-fedora: target := fedora

deploy-packages-debian deploy-packages-fedora: _deploy_packages

_deploy_packages:

	@ echo "Deploying [$(target)] packages..."

	@ make _deploy-packages-cpp target=$(target)
	@ make _deploy-packages-python target=$(target)

deploy-packages-cpp:

	@ echo "Deploying C++ packages..."

	@ make _deploy-packages-cpp target=debian
	@ make _deploy-packages-cpp target=fedora

_deploy-packages-cpp: _build-packages-cpp

	@ echo "Deploying [$(target)] C++ packages..."

	@ echo "TBI"

deploy-packages-python: ## Deploy python packages

	@ echo "Deploying Python packages..."

	@ make deploy-packages-python-debian

deploy-packages-python-debian: target := debian
deploy-packages-python-fedora: target := fedora

deploy-packages-python-debian deploy-packages-python-fedora: _deploy-packages-python

_deploy-packages-python: _build-packages-python

	@ echo "Deploying [$(target)] Python packages..."

	docker run \
	--rm \
	--volume="$(project_directory)/packages/python:/packages:ro" \
	--env="TWINE_USERNAME=${PYPI_USERNAME}" \
	--env="TWINE_PASSWORD=${PYPI_PASSWORD}" \
	python:3.8-slim \
	/bin/bash -c "pip install twine && python3 -m twine upload /packages/*"

deploy-coverage-cpp-results: target := debian

deploy-coverage-cpp-results: _test-coverage-cpp

	@ echo "Deploying C++ coverage results..."

	docker run \
	--rm \
	--volume="$(project_directory):/app:delegated" \
	--workdir=/app \
	$(docker_development_image_repository):$(docker_image_version)-$(target) \
	/bin/bash -c "bash <(curl -s https://codecov.io/bash) -X gcov -y .codecov.yml -t ${ci_codecov_token}"

deploy-documentation: build-documentation ## Deploy documentation

	@ echo "Deploying documentation..."

	@ "$(project_directory)"/tools/ci/deploy-documentation.sh

################################################################################################################################################################

clean: ## Clean

	@ echo "Cleaning up..."

	rm -r "$(project_directory)/build" || true
	rm -r "$(project_directory)/bin/"*.test* || true
	rm -r "$(project_directory)/docs/html" || true
	rm -r "$(project_directory)/docs/latex" || true
	rm -r "$(project_directory)/lib/"*.so* || true
	rm -r "$(project_directory)/coverage" || true
	rm -r "$(project_directory)/packages" || true

################################################################################################################################################################

.PHONY: pull pull-development-images pull-development-image-debian pull-development-image-fedora _pull-development-image \
		build build-images \
		build-development-images build-development-image-debian build-development-image-fedora \
		pull-release-images pull-release-image-cpp-debian pull-release-image-cpp-fedora _pull-release-image-cpp \
		pull-release-image-python-debian pull-release-image-python-fedora _pull-release-image-python \
		build-release-image-cpp-debian build-release-image-python-debian build-release-image-python-fedora \
		pull-release-image-jupyter \
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
		deploy \
		deploy-images deploy-development-images deploy-release-images deploy-release-image-jupyter \
		deploy-packages deploy-coverage-cpp-results deploy-documentation \
		deploy-packages-cpp deploy-packages-python \
		clean

################################################################################################################################################################

help:

	@grep -E '^[0-9a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.DEFAULT_GOAL := help

################################################################################################################################################################
