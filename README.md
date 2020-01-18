# Open Space Toolkit ▸ Astrodynamics

[![Build Status](https://travis-ci.com/open-space-collective/open-space-toolkit-astrodynamics.svg?branch=master)](https://travis-ci.com/open-space-collective/open-space-toolkit-astrodynamics)
[![Code Coverage](https://codecov.io/gh/open-space-collective/open-space-toolkit-astrodynamics/branch/master/graph/badge.svg)](https://codecov.io/gh/open-space-collective/open-space-toolkit-astrodynamics)
[![Documentation](https://img.shields.io/readthedocs/pip/stable.svg)](https://open-space-collective.github.io/open-space-toolkit-astrodynamics)
[![GitHub version](https://badge.fury.io/gh/open-space-collective%2Fopen-space-toolkit-astrodynamics.svg)](https://badge.fury.io/gh/open-space-collective%2Fopen-space-toolkit-astrodynamics)
[![PyPI version](https://badge.fury.io/py/open-space-toolkit-astrodynamics.svg)](https://badge.fury.io/py/open-space-toolkit-astrodynamics)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

Orbit, attitude, access.

<img src="./docs/assets/example.svg" height="500px" width="auto">

## Warning

*⚠ This component is under development.*

## Getting Started

Want to get started? This is the simplest and quickest way:

[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/open-space-collective/open-space-toolkit/master?urlpath=lab/tree/notebooks)

*Nothing to download or install! This will automatically start a [JupyterLab](https://jupyterlab.readthedocs.io/en/stable/) environment in your browser with Open Space Toolkit libraries and example notebooks ready to use.*

### Alternatives

#### Docker Images

[Docker](https://www.docker.com/) must be installed on your system.

##### iPython

The following command will start an [iPython](https://ipython.org/) shell within a container where the OSTk components are already installed:

```bash
docker run -it openspacecollective/open-space-toolkit-astrodynamics-python
```

Once the shell is up and running, playing with it is easy:

```py
from ostk.physics import Environment
from ostk.physics.time import Instant
from ostk.astrodynamics.trajectory import Orbit
from ostk.astrodynamics.trajectory.orbit.models import SGP4
from ostk.astrodynamics.trajectory.orbit.models.sgp4 import TLE

tle = TLE(
    '1 25544U 98067A   18231.17878740  .00000187  00000-0  10196-4 0  9994',
    '2 25544  51.6447  64.7824 0005971  73.1467  36.4366 15.53848234128316'
) # Construct Two-Line Element set

earth = Environment.default().access_celestial_object_with_name('Earth') # Access Earth model

orbit = Orbit(SGP4(tle), earth) # Construct orbit using SGP4 model

orbit.get_state_at(Instant.now()) # Compute and display current satellite state (position, velocity)
```

By default, OSTk fetches the ephemeris from JPL, Earth Orientation Parameters (EOP) and leap second count from IERS.

As a result, when running OSTk for the first time, it may take a minute to fetch all the necessary data.

*Tip: Use tab for auto-completion!*

##### JupyterLab

The following command will start a [JupyterLab](https://jupyterlab.readthedocs.io/en/stable/) server within a container where the OSTk components are already installed:

```bash
docker run --publish=8888:8888 openspacecollective/open-space-toolkit-astrodynamics-jupyter
```

Once the container is running, access [http://localhost:8888/lab](http://localhost:8888/lab) and create a Python 3 Notebook.

## Installation

### C++

The binary packages are hosted using [GitHub Releases](https://github.com/open-space-collective/open-space-toolkit-astrodynamics/releases):

- Runtime libraries: `open-space-toolkit-astrodynamics-X.Y.Z-1.x86_64-runtime`
- C++ headers: `open-space-toolkit-astrodynamics-X.Y.Z-1.x86_64-devel`
- Python bindings: `open-space-toolkit-astrodynamics-X.Y.Z-1.x86_64-python`

#### Debian / Ubuntu

After downloading the relevant `.deb` binary packages, install:

```bash
apt install open-space-toolkit-astrodynamics-*.deb
```

#### Fedora / CentOS

After downloading the relevant `.rpm` binary packages, install:

```bash
dnf install open-space-toolkit-astrodynamics-*.rpm
```

### Python

Install from [PyPI](https://pypi.org/project/open-space-toolkit-astrodynamics/):

```bash
pip install open-space-toolkit-astrodynamics
```

## Structure

The **Astrodynamics** library exhibits the following structure:

```txt
├── Trajectory
│   ├── State
│   ├── Orbit
│   │   ├── Models
│   │   │   ├── Kepler
│   │   │   │   └── Classical Orbital Elements (COE)
│   │   │   ├── SGP4
│   │   │   │   └── Two-Line Element set (TLE)
│   │   │   └── Propagator
│   │   ├── Pass
│   │   └── Utilities
│   └── Composite
├── Flight
│   └── Profile
│       └── State
├── Access
└── State Profile
```

## Documentation

Documentation is available here:

- [C++](https://open-space-collective.github.io/open-space-toolkit-astrodynamics)
- [Python](./bindings/python/docs)

## Tutorials

Tutorials are available here:

- [C++](./tutorials/cpp)
- [Python](./tutorials/python)

## Setup

### Development Environment

Using [Docker](https://www.docker.com) for development is recommended, to simplify the installation of the necessary build tools and dependencies.
Instructions on how to install Docker are available [here](https://docs.docker.com/install/).

To start the development environment:

```bash
make start-development
```

This will:

1. Build the `openspacecollective/open-space-toolkit-astrodynamics-development` Docker image.
2. Create a development environment container with local source files and helper scripts mounted.
3. Start a `bash` shell from the `./build` working directory.

If installing Docker is not an option, you can manually install the development tools (GCC, CMake) and all required dependencies,
by following a procedure similar to the one described in the [Development Dockerfile](./docker/development/Dockerfile).

### Build

From the `./build` directory:

```bash
cmake ..
make
```

*Tip: `helpers/build.sh` simplifies building from within the development environment.*

### Test

To start a container to build and run the tests:

```bash
make test
```

Or to run them manually:

```bash
./bin/open-space-toolkit-astrodynamics.test
```

*Tip: `helpers/test.sh` simplifies running tests from within the development environment.*

## Dependencies

| Name        | Version | License                | Link                                                                                                                                       |
|-------------|---------|------------------------|--------------------------------------------------------------------------------------------------------------------------------------------|
| Boost       | 1.69.0  | Boost Software License | [boost.org](https://www.boost.org)                                                                                                         |
| Eigen       | 3.3.7   | MPL2                   | [eigen.tuxfamily.org](http://eigen.tuxfamily.org/index.php)                                                                                |
| SGP4        | master  | Apache License 2.0     | [github.com/dnwrnr/sgp4](https://github.com/dnwrnr/sgp4)                                                                                   |
| NLopt       | master  | LGPL                   | [github.com/stevengj/nlopt](https://github.com/stevengj/nlopt)                                                                             |
| Core        | master  | Apache License 2.0     | [github.com/open-space-collective/open-space-toolkit-core](https://github.com/open-space-collective/open-space-toolkit-core)               |
| I/O         | master  | Apache License 2.0     | [github.com/open-space-collective/open-space-toolkit-io](https://github.com/open-space-collective/open-space-toolkit-io)                   |
| Mathematics | master  | Apache License 2.0     | [github.com/open-space-collective/open-space-toolkit-mathematics](https://github.com/open-space-collective/open-space-toolkit-mathematics) |
| Physics     | master  | Apache License 2.0     | [github.com/open-space-collective/open-space-toolkit-astrodynamics](https://github.com/open-space-collective/open-space-toolkit-astrodynamics)         |

## Contribution

Contributions are more than welcome!

Please read our [contributing guide](CONTRIBUTING.md) to learn about our development process, how to propose fixes and improvements, and how to build and test the code.

## Special Thanks

*To be completed...*

## License

Apache License 2.0
