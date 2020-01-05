################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           tutorials/cpp/sensor-modeling/CMakeLists.txt
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

## Project Properties

SET (PROJECT_NAME "LibraryAstrodynamics")
SET (PROJECT_DESCRIPTION "Sensor modeling tutorial.")
SET (PROJECT_PATH "OpenSpaceToolkit/Astrodynamics")
SET (PROJECT_PACKAGE_NAME "sensor-modeling")
SET (PROJECT_LICENSE "Apache License 2.0")
SET (PROJECT_VENDOR_ID "org.open-space-collective")
SET (PROJECT_VENDOR_NAME "Open Space Collective")
SET (PROJECT_VENDOR_CONTACT "contact@open-space-collective.org")
SET (PROJECT_VENDOR_URL "open-space-collective.org")

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

PROJECT (${PROJECT_NAME} LANGUAGES "C" "CXX")

SET (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Limited Configs" FORCE)

IF (BUILD_CODE_COVERAGE)
    SET (CMAKE_BUILD_TYPE "Debug")
ELSEIF (NOT CMAKE_BUILD_TYPE)
    SET (CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type [None|Debug|Release|RelWithDebInfo|MinSizeRel]." FORCE)
ENDIF ()

################################################################################################################################################################

## Flags

### Warnings

IF (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    IF (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)

        MESSAGE (FATAL_ERROR "GCC version must be at least 4.8")

    ENDIF ()

    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wshadow -Wno-deprecated -Wl,--no-undefined")

ENDIF ()

### C++ 14 support

SET (CMAKE_CXX_STANDARD 14)
SET (CMAKE_CXX_STANDARD_REQUIRED ON)
SET (CMAKE_CXX_EXTENSIONS OFF)

################################################################################################################################################################

## Debugging Options

SET (CMAKE_VERBOSE_MAKEFILE 0) # Use 1 for debugging, 0 for release

################################################################################################################################################################

## Dependencies

### Eigen [3]

FIND_PACKAGE ("Eigen3" REQUIRED)

IF (EIGEN3_FOUND)
    INCLUDE_DIRECTORIES (${EIGEN3_INCLUDE_DIR})
ELSE ()
    MESSAGE (SEND_ERROR "[Eigen 3] not found.")
ENDIF ()

### Open Space Toolkit ▸ Core [master]

FIND_PACKAGE ("LibraryCore" "0.1" REQUIRED)

### Open Space Toolkit ▸ Mathematics [master]

FIND_PACKAGE ("LibraryMathematics" "0.1" REQUIRED)

### Open Space Toolkit ▸ Physics [master]

FIND_PACKAGE ("LibraryPhysics" "0.1" REQUIRED)

### Open Space Toolkit ▸ Astrodynamics [master]

FIND_PACKAGE ("LibraryAstrodynamics" "0.1" REQUIRED)

################################################################################################################################################################

## Targets

SET (APPLICATION_NAME "${PROJECT_PACKAGE_NAME}")
SET (APPLICATION_TARGET "${APPLICATION_NAME}.exe")

FILE (GLOB APPLICATION_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
FILE (GLOB APPLICATION_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx")

ADD_EXECUTABLE (${APPLICATION_TARGET} ${APPLICATION_SRCS})

TARGET_LINK_LIBRARIES (${APPLICATION_TARGET} ${LibraryCore_LIBRARIES})
TARGET_LINK_LIBRARIES (${APPLICATION_TARGET} ${LibraryMathematics_LIBRARIES})
TARGET_LINK_LIBRARIES (${APPLICATION_TARGET} ${LibraryPhysics_LIBRARIES})
TARGET_LINK_LIBRARIES (${APPLICATION_TARGET} ${LibraryAstrodynamics_LIBRARIES})

################################################################################################################################################################
