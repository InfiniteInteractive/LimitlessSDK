# - Find the FFMPEG library
#
# Usage:
#   find_package(FFMPEG [REQUIRED] [QUIET] )
#     
# It sets the following variables:
#   FFMPEG_FOUND               ... true if FFMPEG is found on the system
#   FFMPEG_LIBRARIES           ... full path to FFMPEG library
#   FFMPEG_INCLUDES            ... FFMPEG include directory
#
# The following variables will be checked by the function
#   FFMPEG_ROOT               ... if set, the libraries are exclusively searched

#If environment variable FFMPEGDIR is specified, it has same effect as FFMPEG_ROOT

MACRO(FFMPEG_FIND varname shortname headername)
#	message(status "FFMPEG_FIND: ${varname}, ${shortname}, ${headername}")

	FIND_PATH(FFMPEG_${varname}_INCLUDE_DIRS ${headername}
	PATHS 
		${FFMPEG_ROOT}
		/usr/include
		/opt/include
	PATH_SUFFIXES 
		ffmpeg
		include
		include/lib${shortname}
	DOC "Location of FFMPEG Headers"
	)
#	message(STATUS "after find: FFMPEG_${varname}_INCLUDE_DIRS: ${FFMPEG_${varname}_INCLUDE_DIRS}")

	FIND_LIBRARY(FFMPEG_${varname}_LIBRARIES
	NAMES ${shortname}
	PATHS
	${FFMPEG_ROOT}/lib
	${FFMPEG_ROOT}/bin
	/usr/local/lib
	/usr/local/lib64
	/usr/lib
	/usr/lib64
	/opt/local/lib
	/opt/lib
	PATH_SUFFIXES
		x64
		lib64
	DOC "Location of FFMPEG Libraries"
	)

	if(FFMPEG_${varname}_LIBRARIES AND FFMPEG_${varname}_INCLUDE_DIRS)
		SET(FFMPEG_${varname}_FOUND 1)
	endif()
ENDMACRO(FFMPEG_FIND)

#message(STATUS "FFMPEG_ROOT: ${FFMPEG_ROOT}")
if( NOT FFMPEG_ROOT AND ENV{FFMPEGDIR} )
  set( FFMPEG_ROOT $ENV{FFMPEGDIR} )
endif()

# Check if we can use PkgConfig
find_package(PkgConfig QUIET)

#Determine from PKG
if( PKG_CONFIG_FOUND AND NOT FFMPEG_ROOT )
  pkg_check_modules( PKG_FFMPEG QUIET "FFMPEG" )
endif()

FFMPEG_FIND(LIBAVFORMAT avformat avformat.h)
FFMPEG_FIND(LIBAVDEVICE avdevice avdevice.h)
FFMPEG_FIND(LIBAVCODEC  avcodec  avcodec.h)
FFMPEG_FIND(LIBAVUTIL   avutil   avutil.h)
FFMPEG_FIND(LIBSWSCALE  swscale  swscale.h)

set(FFMPEG_FOUND "NO")

if (FFMPEG_LIBAVFORMAT_FOUND AND FFMPEG_LIBAVDEVICE_FOUND AND FFMPEG_LIBAVCODEC_FOUND AND FFMPEG_LIBAVUTIL_FOUND)

    set(FFMPEG_FOUND "YES")
	get_filename_component(FFMPEG_INCLUDE_DIRS ${FFMPEG_LIBAVFORMAT_INCLUDE_DIRS} DIRECTORY)
#    set(FFMPEG_INCLUDE_DIRS ${FFMPEG_LIBAVFORMAT_INCLUDE_DIRS}/..)
    set(FFMPEG_LIBRARY_DIRS ${FFMPEG_LIBAVFORMAT_LIBRARY_DIRS})

    set(FFMPEG_LIBRARIES
        ${FFMPEG_LIBAVFORMAT_LIBRARIES}
        ${FFMPEG_LIBAVDEVICE_LIBRARIES}
        ${FFMPEG_LIBAVCODEC_LIBRARIES}
        ${FFMPEG_LIBAVUTIL_LIBRARIES}
		${FFMPEG_LIBSWSCALE_LIBRARIES}
		)
endif()

FIND_PATH(FFMPEG_BIN_DIRS "ffmpeg.exe"
	PATHS
		${FFMPEG_ROOT}/lib
		${FFMPEG_ROOT}/bin
	PATH_SUFFIXES
		x64
		)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FFMPEG_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FFMPEG DEFAULT_MSG FFMPEG_LIBRARIES FFMPEG_INCLUDE_DIRS)

mark_as_advanced(FFMPEG_INCLUDES FFMPEG_LIBRARIES)

