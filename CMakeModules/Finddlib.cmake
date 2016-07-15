# - Find the dlib library
#
# Usage:
#   find_package(dlib [REQUIRED] [QUIET] )
#     
# It sets the following variables:
#   DLIB_FOUND               ... true if dlib is found on the system
#   DLIB_LIBRARIES           ... full path to dlib library
#   DLIB_INCLUDES            ... dlib include directory
#
# The following variables will be checked by the function
#   DLIB_USE_STATIC_LIBS    ... if true, only static libraries are found
#   DLIB_ROOT               ... if set, the libraries are exclusively searched
#                               under this path
#   DLIB_LIBRARY            ... dlib library to use
#   DLIB_INCLUDE_DIR        ... dlib include directory
#

#If environment variable DLIBDIR is specified, it has same effect as DLIB_ROOT
if( NOT DLIB_ROOT AND ENV{DLIBDIR} )
  set( DLIB_ROOT $ENV{DLIBDIR} )
endif()

# Check if we can use PkgConfig
find_package(PkgConfig QUIET)

#Determine from PKG
if( PKG_CONFIG_FOUND AND NOT DLIB_ROOT )
  pkg_check_modules( PKG_DLIB QUIET "fftw3" )
endif()

#Check whether to search static or dynamic libs
set( CMAKE_FIND_LIBRARY_SUFFIXES_SAV ${CMAKE_FIND_LIBRARY_SUFFIXES} )

if( ${DLIB_USE_STATIC_LIBS} )
  set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX} )
else()
	if( ${MSVC} )
		set( CMAKE_FIND_LIBRARY_SUFFIXES ".lib" )
	else()
		set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_LIBRARY_SUFFIX} )
    endif()
endif()

if( DLIB_ROOT )

  #find libs
  find_library(
    DLIB_LIB
    NAMES "dlib"
    PATHS ${DLIB_ROOT}
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH
  )

  #find includes
  find_path(
    DLIB_INCLUDES
    NAMES "dlib/svm.h"
    PATHS ${DLIB_ROOT}
    PATH_SUFFIXES "include"
    NO_DEFAULT_PATH
  )

else()

  find_library(
    DLIB_LIB
    NAMES "dlib"
    PATHS ${PKG_DLIB_LIBRARY_DIRS} ${LIB_INSTALL_DIR}
  )

  find_path(
    DLIB_INCLUDES
    NAMES "svm.h"
    PATHS ${PKG_DLIB_INCLUDE_DIRS} ${INCLUDE_INSTALL_DIR}
  )

endif( DLIB_ROOT )

set(DLIB_LIBRARIES ${DLIB_LIB})

set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_SAV} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DLIB DEFAULT_MSG
                                  DLIB_INCLUDES DLIB_LIBRARIES)

mark_as_advanced(DLIB_INCLUDES DLIB_LIBRARIES)

