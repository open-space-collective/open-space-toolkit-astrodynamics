################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           LibraryAstrodynamicsPy/CMakeLists.txt
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

################################################################################################################################################################

## Project Properties

SET (PROJECT_NAME "LibraryAstrodynamicsPy")
SET (PROJECT_DESCRIPTION "Python bindings for Library / Astrodynamics.")
SET (PROJECT_PACKAGE_NAME "LibraryAstrodynamicsPy")
SET (PROJECT_PATH "Library/Astrodynamics")
SET (PROJECT_LICENSE "MIT License")
SET (PROJECT_VENDOR_ID "com.bremond.lucas")
SET (PROJECT_VENDOR_NAME "Lucas Brémond")
SET (PROJECT_VENDOR_CONTACT "lucas.bremond@gmail.com")
SET (PROJECT_VENDOR_URL "lucas.bremond.info")

################################################################################################################################################################

## Setup

### Compatibility Check

CMAKE_MINIMUM_REQUIRED (VERSION "2.8.12" FATAL_ERROR)

### Paths

SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")

### Policies

CMAKE_POLICY (SET "CMP0048" NEW)

################################################################################################################################################################

## Project Configuration

PROJECT (${PROJECT_NAME} VERSION ${PROJECT_VERSION_STRING} LANGUAGES "C" "CXX")

################################################################################################################################################################

## Dependencies

### Boost [1.67.0]

SET (Boost_USE_STATIC_LIBS OFF)
SET (Boost_USE_MULTITHREADED ON)

UNSET (Boost_INCLUDE_DIR CACHE)
UNSET (Boost_LIBRARY_DIRS CACHE)

FIND_PACKAGE ("Boost" "1.67" COMPONENTS "python36" REQUIRED)

INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES (${Boost_LIBRARY_DIRS})

### Python

# SET (PYTHON_INCLUDE_DIR "/usr/include/python2.7")
# SET (PYTHON_LIBRARIES "/usr/lib64/libpython2.7.so")

SET (PYTHON_INCLUDE_DIR "/usr/include/python3.6m")
SET (PYTHON_LIBRARIES "/usr/lib64/libpython3.6m.so")

### NumPy

SET (NUMPY_INCLUDE_DIR "/usr/local/lib64/python3.6/site-packages/numpy/core/include")

################################################################################################################################################################

## Target

SET (LIBRARY_NAME ${PROJECT_PACKAGE_NAME})
SET (LIBRARY_TARGET "${LIBRARY_NAME}.so")

FILE (GLOB_RECURSE PROJECT_SRCS "${PROJECT_SOURCE_DIR}/src/*.cxx")

ADD_LIBRARY (${LIBRARY_TARGET} SHARED ${PROJECT_SRCS})

ADD_DEPENDENCIES (${LIBRARY_TARGET} ${SHARED_LIBRARY_TARGET})

TARGET_INCLUDE_DIRECTORIES (${LIBRARY_TARGET} PUBLIC ${PYTHON_INCLUDE_DIR})
TARGET_INCLUDE_DIRECTORIES (${LIBRARY_TARGET} PUBLIC ${NUMPY_INCLUDE_DIR})
TARGET_INCLUDE_DIRECTORIES (${LIBRARY_TARGET} PUBLIC "${CMAKE_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES (${LIBRARY_TARGET} PUBLIC "${CMAKE_SOURCE_DIR}/src")
TARGET_INCLUDE_DIRECTORIES (${LIBRARY_TARGET} PUBLIC "${PROJECT_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES (${LIBRARY_TARGET} PUBLIC "${PROJECT_SOURCE_DIR}/src")

TARGET_LINK_LIBRARIES (${LIBRARY_TARGET} ${PYTHON_LIBRARIES})
TARGET_LINK_LIBRARIES (${LIBRARY_TARGET} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES (${LIBRARY_TARGET} ${SHARED_LIBRARY_TARGET})

SET_TARGET_PROPERTIES ( ${LIBRARY_TARGET} PROPERTIES
                        VERSION ${PROJECT_VERSION_STRING}
                        SOVERSION ${PROJECT_VERSION_MAJOR}
                        OUTPUT_NAME ${PROJECT_PACKAGE_NAME}
                        CLEAN_DIRECT_OUTPUT 1
                        PREFIX ""
                        BUILD_WITH_INSTALL_RPATH TRUE
                        INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/")

INSTALL (TARGETS ${LIBRARY_TARGET} DESTINATION "${INSTALL_LIB}/${PROJECT_PATH}" COMPONENT "python")
INSTALL (FILES "${PROJECT_SOURCE_DIR}/tools/python/Library/__init__.py" DESTINATION "${INSTALL_LIB}/Library" COMPONENT "python")
INSTALL (FILES "${PROJECT_SOURCE_DIR}/tools/python/${PROJECT_PATH}/__init__.py" DESTINATION "${INSTALL_LIB}/${PROJECT_PATH}" COMPONENT "python")

################################################################################################################################################################

FIND_PROGRAM (PYTHON "python3")

IF (NOT PYTHON)

    MESSAGE (FATAL_ERROR "Unable to find [python3] program.")

ENDIF (NOT PYTHON)

SET (PACKAGE_NAME ${PROJECT_PACKAGE_NAME})
SET (PACKAGE_TARGET "${PACKAGE_NAME}-python-package")

SET (SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/setup.py.in")
SET (SETUP_PY "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
SET (DEPS "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/${PROJECT_PATH}/__init__.py")
SET (OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/build/timestamp")

CONFIGURE_FILE (${SETUP_PY_IN} ${SETUP_PY})

ADD_CUSTOM_COMMAND (OUTPUT ${OUTPUT}
                    COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_PATH}"
                    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/Library/__init__.py" "${CMAKE_CURRENT_BINARY_DIR}/Library/__init__.py"
                    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/tools/python/${PROJECT_PATH}/__init__.py" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_PATH}/__init__.py"
                    COMMAND cp "${CMAKE_SOURCE_DIR}/lib/${SHARED_LIBRARY_TARGET}" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_PATH}/liblibrary-astrodynamics.so.0"
                    COMMAND cp "${CMAKE_SOURCE_DIR}/lib/${LIBRARY_TARGET}" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_PATH}/LibraryAstrodynamicsPy.so"
                    COMMAND ${PYTHON} ${SETUP_PY} bdist_wheel
                    COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
                    DEPENDS ${DEPS})

ADD_CUSTOM_TARGET (${PACKAGE_TARGET} ALL DEPENDS ${OUTPUT})

ADD_DEPENDENCIES (${PACKAGE_TARGET} ${LIBRARY_TARGET})

# INSTALL (CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install)")

################################################################################################################################################################

ADD_CUSTOM_TARGET ("publish" COMMAND python3 -m twine upload --repository-url https://test.pypi.org/legacy/ "${CMAKE_CURRENT_BINARY_DIR}/dist/*" DEPENDS ${OUTPUT})

################################################################################################################################################################