# Apache License 2.0

## Project Properties

SET (PROJECT_NAME "OpenSpaceToolkitAstrodynamicsPy")
SET (PROJECT_DESCRIPTION "Python bindings for Open Space Toolkit / Astrodynamics.")
SET (PROJECT_PACKAGE_NAME "OpenSpaceToolkitAstrodynamicsPy")
SET (PROJECT_GROUP "ostk")
SET (PROJECT_SUBGROUP "astrodynamics")
SET (PROJECT_PATH "${PROJECT_GROUP}/${PROJECT_SUBGROUP}")
SET (PROJECT_LICENSE "Apache License 2.0")
SET (PROJECT_VENDOR_ID "com.bremond.lucas")
SET (PROJECT_VENDOR_NAME "Lucas Brémond")
SET (PROJECT_VENDOR_CONTACT "lucas.bremond@gmail.com")
SET (PROJECT_VENDOR_URL "lucas.bremond.info")

## Setup

### Compatibility Check

CMAKE_MINIMUM_REQUIRED (VERSION "2.8.12" FATAL_ERROR)

### Paths

SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")

### Policies

CMAKE_POLICY (SET "CMP0048" NEW)

### Options

OPTION (BUILD_WHEELS_IN_CMAKE "Build python wheels with cmake." ON)

## Project Configuration

PROJECT (${PROJECT_NAME} VERSION ${PROJECT_VERSION_STRING} LANGUAGES "C" "CXX")

## Helper Functions

### Improved FIND_PYTHON macro

MACRO (FIND_PYTHON version)
    FIND_PROGRAM (PYTHON_CONFIG_${version} python${version}-config)
    IF (EXISTS ${PYTHON_CONFIG_${version}})
        EXECUTE_PROCESS (COMMAND ${PYTHON_CONFIG_${version}} --includes OUTPUT_VARIABLE _python_include_args)
        SEPARATE_ARGUMENTS (_python_includes UNIX_COMMAND "${_python_include_args}")
        STRING (REPLACE "-I" "" _python_includes "${_python_includes}")
        ADD_LIBRARY (python${version}::headers INTERFACE IMPORTED GLOBAL)
        SET_TARGET_PROPERTIES (python${version}::headers PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${_python_includes}"
        )
        EXECUTE_PROCESS (COMMAND ${PYTHON_CONFIG_${version}} --prefix OUTPUT_VARIABLE _python_prefix)
        STRING (STRIP "${_python_prefix}" _python_prefix)
        SET (PYTHON_${version}_EXECUTABLE "${_python_prefix}/bin/python${version}" CACHE PATH "")
    ENDIF ()
ENDMACRO ()

### Function to grab python extension and use for target

FUNCTION (PY_EXTENSION name version)
    SET (_python_module_extension)
    EXECUTE_PROCESS (COMMAND ${PYTHON_CONFIG_${version}} --extension-suffix OUTPUT_VARIABLE _python_module_extension)
    STRING (STRIP "${_python_module_extension}" _python_module_extension)
    SET_TARGET_PROPERTIES (${name} PROPERTIES PREFIX "" SUFFIX "${_python_module_extension}")
ENDFUNCTION ()

### Function to add python module using python::headers directly

FUNCTION (PY_ADD_MODULE NAME)
    SET (options)
    SET (oneValueArgs PYTHON_VERSION)
    SET (multiValueArgs)

    CMAKE_PARSE_ARGUMENTS (PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    SET (PYTHON_VERSION ${PARSE_PYTHON_VERSION})

    ADD_LIBRARY (${NAME} MODULE ${PARSE_UNPARSED_ARGUMENTS})
    PYBIND11_STRIP (${NAME})
    PY_EXTENSION (${NAME} ${PYTHON_VERSION})

    # Source Code
    TARGET_INCLUDE_DIRECTORIES (${NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")
    TARGET_INCLUDE_DIRECTORIES (${NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src")
    TARGET_INCLUDE_DIRECTORIES (${NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
    TARGET_INCLUDE_DIRECTORIES (${NAME} PUBLIC "${PROJECT_SOURCE_DIR}/src")

    # Previous Link with Boost Numpy and Python
    # TARGET_LINK_LIBRARIES (${LIBRARY_TARGET} "Python3::Python")
    # TARGET_LINK_LIBRARIES (${LIBRARY_TARGET} "Boost::${PYTHON_VERSION}" "Boost::numpy37")

    # Pybind11/Python Headers
    TARGET_LINK_LIBRARIES (${NAME} PRIVATE pybind11::module pybind11::lto python${PYTHON_VERSION}::headers)

    TARGET_LINK_LIBRARIES (${NAME} PRIVATE ${SHARED_LIBRARY_TARGET})

    # Target Properties
    SET_TARGET_PROPERTIES (${NAME} PROPERTIES
                          VERSION ${PROJECT_VERSION_STRING}
                          SOVERSION ${PROJECT_VERSION_MAJOR}
                          OUTPUT_NAME ${PROJECT_PACKAGE_NAME}
                          CLEAN_DIRECT_OUTPUT 1
                          PREFIX ""
                          BUILD_WITH_INSTALL_RPATH TRUE
                          INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/"
                          C_VISIBILITY_PRESET hidden
                          CXX_VISIBILITY_PRESET hidden)
ENDFUNCTION ()

### Function preparing python package directory/build wheel for each python version

FUNCTION (PY_ADD_PACKAGE_DIRECTORY NAME)
    SET (options)
    SET (oneValueArgs PYTHON_VERSION)

    CMAKE_PARSE_ARGUMENTS (PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    SET (PYTHON_VERSION ${PARSE_PYTHON_VERSION})

    SET (DEPS "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/${PROJECT_PATH}/__init__.py")
    SET (OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/build/timestamp_${PYTHON_VERSION}")

    # Package directory name
    SET (PACKAGE_DIRECTORY_NAME ${NAME})

    # Get python extension for relevant shared library spotting
    STRING (REPLACE "." "" EXTENSION "${PYTHON_VERSION}")

    # Configure setup.cfg file
    SET (SETUP_CFG_IN "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/setup.cfg.in")
    SET (SETUP_CFG "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/setup.cfg")
    CONFIGURE_FILE (${SETUP_CFG_IN} ${SETUP_CFG})

    # Configure pyproject.toml file
    SET (PYPROJECT_TOML_IN "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/pyproject.toml.in")
    SET (PYPROJECT_TOML "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/pyproject.toml")
    CONFIGURE_FILE (${PYPROJECT_TOML_IN} ${PYPROJECT_TOML})

    IF (BUILD_WHEELS_IN_CMAKE)

        # Build wheels with cmake
        ADD_CUSTOM_COMMAND (OUTPUT ${OUTPUT}
                            COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}"
                            COMMAND rm -rf "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}/test"
                            COMMAND cp -r "${CMAKE_CURRENT_SOURCE_DIR}/test" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}/test"
                            COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/${PROJECT_GROUP}/__init__.py" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_GROUP}/__init__.py"
                            COMMAND cp -r "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/${PROJECT_PATH}/*" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}/"
                            COMMAND cp "${CMAKE_SOURCE_DIR}/lib/${SHARED_LIBRARY_TARGET}" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}/libopen-space-toolkit-astrodynamics.so.0"
                            COMMAND cp "${CMAKE_SOURCE_DIR}/lib/${LIBRARY_TARGET}.*${EXTENSION}*.so" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}/"
                            COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/requirements.txt"
                            COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/README.md" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/README.md"
                            COMMAND cd "${CMAKE_CURRENT_BINARY_DIR}/${NAME}" && python${PYTHON_VERSION} -m build --no-isolation -w
                            COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/dist"
                            COMMAND cp "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/dist/*" "${CMAKE_CURRENT_BINARY_DIR}/dist"
                            DEPENDS ${DEPS})
    ELSE ()
        # Build package directories only to integrate with a ci build wheel process
        ADD_CUSTOM_COMMAND (OUTPUT ${OUTPUT}
                            COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}"
                            COMMAND rm -rf "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}/test"
                            COMMAND cp -r "${CMAKE_CURRENT_SOURCE_DIR}/test" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}/test"
                            COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/${PROJECT_GROUP}/__init__.py" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_GROUP}/__init__.py"
                            COMMAND cp -r "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/${PROJECT_PATH}/*" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}/"
                            COMMAND cp "${CMAKE_SOURCE_DIR}/lib/${SHARED_LIBRARY_TARGET}" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}/libopen-space-toolkit-astrodynamics.so.0"
                            COMMAND cp "${CMAKE_SOURCE_DIR}/lib/${LIBRARY_TARGET}.*${EXTENSION}*.so" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}/${PROJECT_PATH}/"
                            DEPENDS ${DEPS})
    ENDIF ()

    ADD_CUSTOM_TARGET (${NAME} ALL DEPENDS ${OUTPUT})
    ADD_DEPENDENCIES (${NAME} "${LIBRARY_TARGET}_${PYTHON_VERSION}")
ENDFUNCTION ()

## Dependencies

### Pybind11

SET (PYBIND11_NOPYTHON ON)
FIND_PACKAGE (pybind11 2.6.0 REQUIRED)

### Python

# Decide on the versions to build for and support
SET (PYTHON_SEARCH_VERSIONS 3.8 3.9 3.10 3.11)

MESSAGE (STATUS "Looking for available python versions...")
FOREACH (PYTHON_VERSION ${PYTHON_SEARCH_VERSIONS})
    FIND_PYTHON (${PYTHON_VERSION})
    IF (TARGET python${PYTHON_VERSION}::headers)
        MESSAGE (STATUS "Python ${PYTHON_VERSION} found.")
        LIST (APPEND PYTHON_VERSIONS ${PYTHON_VERSION})
    ELSE ()
        MESSAGE (STATUS "Python ${PYTHON_VERSION} not found.")
    ENDIF ()
ENDFOREACH ()

## Target

SET (LIBRARY_NAME ${PROJECT_PACKAGE_NAME})
SET (LIBRARY_TARGET "${LIBRARY_NAME}")

FILE (GLOB_RECURSE PROJECT_SRCS "${PROJECT_SOURCE_DIR}/src/*.cxx")

# Global library target
ADD_CUSTOM_TARGET (${LIBRARY_TARGET})

# Build python-specific shared library targets (.so)
FOREACH (PYTHON_VERSION ${PYTHON_VERSIONS})
    MESSAGE (STATUS "Building shared library for python ${PYTHON_VERSION}...")
    PY_ADD_MODULE ("${LIBRARY_TARGET}_${PYTHON_VERSION}" ${PROJECT_SRCS} PYTHON_VERSION ${PYTHON_VERSION})
    ADD_DEPENDENCIES (${LIBRARY_TARGET} "${LIBRARY_TARGET}_${PYTHON_VERSION}")

    # Install make commands for each python version
    INSTALL (TARGETS ${LIBRARY_TARGET}_${PYTHON_VERSION} DESTINATION "${INSTALL_LIB}/${PROJECT_PATH}" COMPONENT "python")
    INSTALL (FILES "${PROJECT_SOURCE_DIR}/tools/python/${PROJECT_GROUP}/__init__.py" DESTINATION "${INSTALL_LIB}/${PROJECT_GROUP}" COMPONENT "python")
    INSTALL (FILES "${PROJECT_SOURCE_DIR}/tools/python/${PROJECT_PATH}/__init__.py" DESTINATION "${INSTALL_LIB}/${PROJECT_PATH}" COMPONENT "python")
ENDFOREACH ()

## Generate python package directory/wheels

# Generate a python-package target for each of the python versions that are considered here
SET (PACKAGE_NAME ${PROJECT_PACKAGE_NAME})
SET (PACKAGE_TARGET "${PACKAGE_NAME}-python-package")

# Build python-specific directory to build wheels
FOREACH (PYTHON_VERSION ${PYTHON_VERSIONS})
    MESSAGE (STATUS "Building package directory for python ${PYTHON_VERSION}...")
    PY_ADD_PACKAGE_DIRECTORY ("${PACKAGE_TARGET}-${PYTHON_VERSION}" PYTHON_VERSION ${PYTHON_VERSION})
ENDFOREACH ()
