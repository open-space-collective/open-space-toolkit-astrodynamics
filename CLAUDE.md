# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Open Space Toolkit ▸ Astrodynamics (OSTk Astrodynamics) is a C++/Python library for orbit, attitude, access, and mission analysis. It is part of the Open Space Toolkit suite and depends on Core, I/O, Mathematics, and Physics libraries from the same ecosystem.

## Development Environment

### Docker-Based Development (Recommended)

All development is done within Docker containers to ensure consistent build environments.

To build:
- docker run --rm --workdir=/app --volume="/home/vishwa/ostk/open-space-toolkit-astrodynamics:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root ostk-build

To test C++:
- docker run --rm --workdir=/app --volume="/home/vishwa/ostk/open-space-toolkit-astrodynamics:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root bin/open-space-toolkit-astrodynamics.test
- docker run --rm --workdir=/app --volume="/home/vishwa/ostk/open-space-toolkit-astrodynamics:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root bin/open-space-toolkit-astrodynamics.test --gtest_filter=\*YOUR_TEST\*

To install python package:
- docker run --rm --workdir=/app --volume="/home/vishwa/ostk/open-space-toolkit-astrodynamics:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root ostk-install-python

To test python:
- docker run --rm --workdir=/app --volume="/home/vishwa/ostk/open-space-toolkit-astrodynamics:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root ostk-install-python && ostk-test-python
- docker run --rm --workdir=/app --volume="/home/vishwa/ostk/open-space-toolkit-astrodynamics:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root ostk-test-python -k "YOUR FILTER"

To format code:
- docker run --rm --workdir=/app --volume="/home/vishwa/ostk/open-space-toolkit-astrodynamics:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root ostk-test-python ostk-format-cpp
- docker run --rm --workdir=/app --volume="/home/vishwa/ostk/open-space-toolkit-astrodynamics:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root ostk-test-python ostk-format-python

To build docs:
- docker run --rm --workdir=/app --volume="/home/vishwa/ostk/open-space-toolkit-astrodynamics:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root ostk-test-python ostk-build-docs


### Helper Commands in Development Container

Once inside the development container, use these commands:

- `ostk-build`: Build the project (wrapper around cmake/make)
- `ostk-test`: Run tests
- `ostk-validate`: Run validation tests
- `ostk-format-cpp`: Format C++ code with clang-format
- `ostk-check-format-cpp`: Check C++ formatting
- `ostk-format-python`: Format Python code with black
- `ostk-check-format-python`: Check Python formatting
- `ostk-install-python`: Install Python bindings
- `ostk-build-docs`: Build documentation

## Build Commands

### Manual Build (Inside Container or Local)

From the `build/` directory:

```bash
cmake ..
make -j $(nproc)
```

**CMake Build Options:**

- `BUILD_UNIT_TESTS=ON/OFF` - Build C++ unit tests (default: ON)
- `BUILD_VALIDATION_TESTS=ON/OFF` - Build validation tests (default: OFF)
- `BUILD_PYTHON_BINDINGS=ON/OFF` - Build Python bindings (default: ON)
- `BUILD_BENCHMARK=ON/OFF` - Build benchmarks (default: OFF)
- `BUILD_CODE_COVERAGE=ON/OFF` - Build with coverage instrumentation (default: OFF)
- `BUILD_DOCUMENTATION=ON/OFF` - Build documentation (default: OFF)
- `BUILD_SCRIPT=ON/OFF` - Build executable from scripts/ directory (default: OFF)
- `BUILD_WITH_DEBUG_SYMBOLS=ON/OFF` - Include debug symbols (default: ON)

### Running Tests

- **C++ tests**: `./bin/open-space-toolkit-astrodynamics.test` (after building with `BUILD_UNIT_TESTS=ON`)
- **Specific test filter**: Use GTest filters, e.g., `./bin/open-space-toolkit-astrodynamics.test --gtest_filter="State.*"`
- **Python tests**: After building Python bindings, use `pytest` in the installed package location
- **Validation tests**: `./bin/open-space-toolkit-astrodynamics.validation` (after building with `BUILD_VALIDATION_TESTS=ON`)

## Code Architecture

### Core Concepts

**State**: Central abstraction representing spacecraft state at an instant. Contains:
- `Instant`: time
- `coordinates`: position, velocity, attitude, angular velocity, mass, etc.
- `CoordinateBroker`: manages coordinate subsets and their extraction/construction
- States are frame-dependent and can be transformed between frames

**Trajectory**: Time-parameterized path through space
- `Orbit`: Trajectory of a celestial body (uses orbit models like SGP4, Kepler, Propagated)
- `Profile`: Spacecraft trajectory with position AND attitude information

**Dynamics**: Defines equations of motion for propagation
- `CentralBodyGravity`: gravitational acceleration from central body (supports harmonics via EGM96)
- `ThirdBodyGravity`: gravitational perturbations from third bodies (Sun, Moon, etc.)
- `AtmosphericDrag`: drag force with atmospheric models
- `Thruster`: thrust forces
- `Tabulated`: pre-computed accelerations

**Propagator**: Integrates equations of motion to compute future states
- Uses numerical solvers (Runge-Kutta variants, Dormand-Prince, etc.)
- Configured with dynamics, initial state, and numerical solver settings

### Major Components

```
OpenSpaceToolkit/Astrodynamics/
├── Trajectory/
│   ├── State/                    # State representation and coordinate management
│   │   ├── CoordinateBroker      # Manages coordinate subsets
│   │   ├── CoordinateSubset/     # Position, velocity, attitude, mass, etc.
│   │   └── NumericalSolver       # ODE solver configuration
│   ├── Orbit/                    # Orbit representations
│   │   ├── Model/                # Kepler, SGP4, BrouwerLyddaneMean, Propagated, Tabulated
│   │   ├── Pass                  # Satellite pass over ground location
│   │   └── Message/              # OPM and other orbit messages
│   ├── Propagator                # Numerical propagation engine
│   ├── Segment                   # Trajectory segment (coast or maneuver)
│   └── Sequence                  # Ordered segments forming mission sequence
├── Flight/
│   ├── Profile/                  # Trajectory with attitude
│   │   └── Model/                # Transform, Tabulated
│   ├── System/                   # Spacecraft system models
│   └── Maneuver                  # Maneuver definitions
├── Dynamics/                     # Force and torque models
│   ├── CentralBodyGravity
│   ├── ThirdBodyGravity
│   ├── AtmosphericDrag
│   ├── Thruster
│   └── Tabulated
├── Access/                       # Visibility and access analysis
│   └── Generator                 # Generate access windows
├── EventCondition/               # Conditions for event detection (eclipse, AOS/LOS, etc.)
├── GuidanceLaw/                  # Attitude guidance laws
├── Eclipse/                      # Eclipse modeling
├── Estimator/                    # State estimation
├── Solver/                       # Optimization solvers
├── Data/                         # Data providers (gravity models, atmospheric data)
└── Conjunction/                  # Conjunction analysis and CDMs
```

### Key Patterns

1. **Frame Management**: All States, Positions, Velocities are frame-aware. Use `Frame::GCRF()` for inertial, `Frame::ITRF()` for Earth-fixed. Transform states between frames with `state.inFrame(targetFrame)`.

2. **Coordinate Brokers**: States use CoordinateBrokers to define which coordinates are present (position, velocity, attitude, etc.). When propagating, the broker determines what gets integrated.

3. **Shared Pointers**: Heavy use of `Shared<T>` (alias for `std::shared_ptr<T>`) for polymorphic objects like Dynamics, Models, Frames.

4. **Propagation Workflow**:
   ```cpp
   // Define dynamics
   Array<Shared<Dynamics>> dynamics = {/* CentralBodyGravity, AtmosphericDrag, etc. */};

   // Create propagator
   Propagator propagator = Propagator::FromEnvironment(numericalSolver, dynamics);

   // Propagate from initial state
   State futureState = propagator.calculateStateAt(initialState, targetInstant);
   ```

5. **Python Bindings**: Use pybind11. Python package is `ostk.astrodynamics`, imported as `from ostk.astrodynamics import ...`

## Validation Framework

OSTk includes cross-validation against GMAT and Orekit for end-to-end propagation accuracy.

- **Scenario definitions**: YAML files in `validation/data/scenarios/` define spacecraft, dynamics, and mission sequences
- **Reference data**: Pre-computed outputs from GMAT/Orekit in `validation/data/{gmat,orekit}_astrodynamics/`
- **Tests**: Defined in `validation/OpenSpaceToolkit/Astrodynamics/Framework.validation.cpp`
- **Run validation**: `make validation` or `ostk-validate` inside dev container

## Coding Style

- **Naming**: camelCase for variables, methods, and members (see `.cursor/rules/ostk-format-rule.mdc`)
- **Formatting**: Use clang-format (`.clang-format` rules in `thirdparty/clang/`)
- **Headers**: `.hpp` for headers, `.cpp` for implementations, `.tpp` for template implementations
- **Namespaces**: `ostk::astrodynamics::trajectory`, `ostk::astrodynamics::flight`, etc.
- **License**: All files start with `/// Apache License 2.0` or `# Apache License 2.0`

## Testing

- **Unit tests**: Located in `test/OpenSpaceToolkit/Astrodynamics/` mirroring source structure
- **Test naming**: `*.test.cpp` files with GTest framework
- **Validation tests**: `*.validation.cpp` files in `validation/`
- **Python tests**: In `bindings/python/test/` using pytest

## Dependencies

This library depends on other Open Space Toolkit libraries in this order:
- Core (v5.x.y) - Fundamental types, containers, error handling
- I/O (v4.x.y) - File I/O, URL handling
- Mathematics (v4.x.y) - Linear algebra, geometry, optimization
- Physics (v13.x.y) - Frames, time, environment, celestial bodies

External dependencies: Boost, Eigen3, SGP4, NLopt, pybind11

## Important Notes

- On first run, OSTk fetches ephemeris from JPL, Earth Orientation Parameters (EOP), and leap seconds from IERS, which may take a minute
- Data is cached in `.open-space-toolkit/` directory
- Default environment includes Earth, Sun, Moon with full ephemeris
- Use `Environment::default()` to get pre-configured environment
- Build system uses C++20 by default (C++17 for ARM64)
