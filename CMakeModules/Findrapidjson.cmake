# Findrapidjson - attempts to locate the rapidjson library.
#
# This module defines the following variables (on success):
#   RAPIDJSON_INCLUDE_DIRS  - where to find rapidjson/rapidjson.h
#   RAPIDJSON_FOUND         - if the library was successfully located
#
# It is trying a few standard installation locations, but can be customized
# with the following variables:
#   RAPIDJSON_ROOT_DIR      - root directory of a rapidjson installation
#                       Headers are expected to be found in either:
#                       <RAPIDJSON_ROOT_DIR>/rapidjson/rapidjson.h
#                       This variable can either be a cmake or environment
#                       variable. Note however that changing the value
#                       of the environment varible will NOT result in
#                       re-running the header search and therefore NOT
#                       adjust the variables set by this module.

# default search dirs
SET(_RAPIDJSON_HEADER_SEARCH_DIRS
    "/usr/include"
    "/usr/local/include")

# check environment variable
SET(_RAPIDJSON_ENV_ROOT_DIR "$ENV{RAPIDJSON_ROOT_DIR}")

IF(NOT RAPIDJSON_ROOT_DIR AND _RAPIDJSON_ENV_ROOT_DIR)
    SET(RAPIDJSON_ROOT_DIR "${_RAPIDJSON_ENV_ROOT_DIR}")
ENDIF(NOT RAPIDJSON_ROOT_DIR AND _RAPIDJSON_ENV_ROOT_DIR)

# put user specified location at beginning of search
IF(RAPIDJSON_ROOT_DIR)
    SET(_RAPIDJSON_HEADER_SEARCH_DIRS "${RAPIDJSON_ROOT_DIR}"
                                "${RAPIDJSON_ROOT_DIR}/include"
                                 ${_RAPIDJSON_HEADER_SEARCH_DIRS})
ENDIF(RAPIDJSON_ROOT_DIR)

# locate header
FIND_PATH(RAPIDJSON_INCLUDE_DIR "rapidjson/rapidjson.h"
    PATHS ${_RAPIDJSON_HEADER_SEARCH_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RAPIDJSON DEFAULT_MSG
    RAPIDJSON_INCLUDE_DIR)

IF(RAPIDJSON_FOUND)
    SET(RAPIDJSON_INCLUDE_DIRS "${RAPIDJSON_INCLUDE_DIR}")

    MESSAGE(STATUS "RAPIDJSON_INCLUDE_DIR = ${RAPIDJSON_INCLUDE_DIR}")
ENDIF(RAPIDJSON_FOUND)