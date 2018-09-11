# Find NLopt

# This sets the following variables:
# NLopt_FOUND
# NLopt_INCLUDE_DIRS
# NLopt_LIBRARIES
# NLOPT_DEFINITIONS
# NLOPT_VERSION

find_package(PkgConfig QUIET)

# Check to see if pkgconfig is installed.
pkg_check_modules(PC_NLopt nlopt QUIET)

# Definitions
set(NLopt_DEFINITIONS ${PC_NLopt_CFLAGS_OTHER})

# Include directories
find_path(NLopt_INCLUDE_DIRS
    NAMES nlopt.h
    HINTS ${PC_NLopt_INCLUDEDIR}
    PATHS "${CMAKE_INSTALL_PREFIX}/include")

# Libraries
find_library(NLopt_LIBRARIES
    NAMES nlopt nlopt_cxx
    HINTS ${PC_NLopt_LIBDIR})

# Version
set(NLopt_VERSION ${PC_NLopt_VERSION})

# Set (NAME)_FOUND if all the variables and the version are satisfied.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NLopt
    FAIL_MESSAGE  DEFAULT_MSG
    REQUIRED_VARS NLopt_INCLUDE_DIRS NLopt_LIBRARIES
    VERSION_VAR   NLopt_VERSION)
