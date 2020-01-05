# Open Space Toolkit ▸ Astrodynamics

Orbit, attitude, access.

[![Build Status](https://travis-ci.com/open-space-collective/open-space-toolkit-astrodynamics.svg?branch=master)](https://travis-ci.com/open-space-collective/open-space-toolkit-astrodynamics)
[![Code Coverage](https://codecov.io/gh/open-space-collective/open-space-toolkit-astrodynamics/branch/master/graph/badge.svg)](https://codecov.io/gh/open-space-collective/open-space-toolkit-astrodynamics)
[![Documentation](https://img.shields.io/readthedocs/pip/stable.svg)](https://open-space-collective.github.io/open-space-toolkit-astrodynamics)
[![GitHub version](https://badge.fury.io/gh/open-space-collective%2Fopen-space-toolkit-astrodynamics.svg)](https://badge.fury.io/gh/open-space-collective%2Fopen-space-toolkit-astrodynamics)
[![PyPI version](https://badge.fury.io/py/open-space-toolkit-astrodynamics.svg)](https://badge.fury.io/py/open-space-toolkit-astrodynamics)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

<img src="./docs/assets/example.svg" height="500px" width="auto">

## Warning

*⚠ This component is under development.*

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
| Physics     | master  | Apache License 2.0     | [github.com/open-space-collective/open-space-toolkit-physics](https://github.com/open-space-collective/open-space-toolkit-physics)         |

## Contribution

Contributions are more than welcome!

Please read our [contributing guide](CONTRIBUTING.md) to learn about our development process, how to propose fixes and improvements, and how to build and test the code.

## Special Thanks

*To be completed...*

## License

Apache License 2.0
