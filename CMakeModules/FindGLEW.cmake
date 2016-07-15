#.rst:
# FindGLEW
# --------
#
# Find the OpenGL Extension Wrangler Library (GLEW)
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``GLEW::GLEW``,
# if GLEW has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   GLEW_INCLUDE_DIRS - include directories for GLEW
#   GLEW_LIBRARIES - libraries to link against GLEW
#   GLEW_FOUND - true if GLEW has been found and can be used

#=============================================================================
# Copyright 2012 Benjamin Eikel
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PACKAGE( PackageHandleStandardArgs )

SET(_GLEW_ENV_ROOT_DIR "$ENV{GLEW_ROOT_DIR}")

IF(NOT GLEW_ROOT_DIR AND _GLEW_ENV_ROOT_DIR)
    SET(GLEW_ROOT_DIR "${_GLEW_ENV_ROOT_DIR}")
ENDIF(NOT GLEW_ROOT_DIR AND _GLEW_ENV_ROOT_DIR)

find_path(GLEW_INCLUDE_DIR
  NAMES
    GL/glew.h
  PATHS
	${GLEW_ROOT_DIR}
	ENV ATISTREAMSDKROOT
  PATH_SUFFIXES
    include)

if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	find_library(GLEW_LIBRARY 
	  NAMES
		glew32.lib glew.lib glew32s.lib
	  PATHS
		${GLEW_ROOT_DIR}
		ENV ATISTREAMSDKROOT
	  PATH_SUFFIXES 
		lib
		lib/x86)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	find_library(GLEW_LIBRARY 
	  NAMES
		glew64.lib glew.lib glew64s.lib 
	  PATHS
		${GLEW_ROOT_DIR}
		ENV ATISTREAMSDKROOT
	  PATH_SUFFIXES 
		lib
		lib/x86_64
		lib/x64)
	message(status "GLEW Win64: ${GLEW_LIBRARY}")
  endif()
else()
  find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s PATH_SUFFIXES lib lib64)
endif()

set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
set(GLEW_LIBRARIES ${GLEW_LIBRARY})

find_package_handle_standard_args(GLEW
                                  REQUIRED_VARS GLEW_INCLUDE_DIR GLEW_LIBRARY)

if(GLEW_FOUND AND NOT TARGET GLEW::GLEW)
  add_library(GLEW::GLEW UNKNOWN IMPORTED)
  set_target_properties(GLEW::GLEW PROPERTIES
    IMPORTED_LOCATION "${GLEW_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIRS}")
endif()

mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)
