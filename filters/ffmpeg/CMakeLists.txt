#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(ffmpeg)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(ffmpeg REQUIRED)

if(FFMPEG_FOUND)
	message(STATUS "******FFMpeg Found")
	message(STATUS "******  include: ${FFMPEG_INCLUDE_DIRS}")
	message(STATUS "******  libs: ${FFMPEG_LIBRARIES}")
else()
	message(FATAL_ERROR "******Error FFMpeg no found")
endif()

##ffmpeg
#set(FFMPEG_INCLUDES ${CMAKE_SOURCE_DIR}/libraries/ffmpeg/include)
#if(BUILD_x64)
#	set(FFMPEG_LIB_PATH ${CMAKE_SOURCE_DIR}/libraries/ffmpeg/bin/x64)
#else(BUILD_x64)
#	set(FFMPEG_LIB_PATH ${CMAKE_SOURCE_DIR}/libraries/ffmpeg/bin/x86)
#endif(BUILD_x64)
#message(STATUS "******FFMpeg")

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(ffmpeg_Sources
	codecDescription.cpp
	ffmpegAudioSample.cpp
	ffmpegcontrols.cpp
	ffmpegDecoder.cpp
	ffmpegEncoder.cpp
	ffmpegFrameSample.cpp
	ffmpegInput.cpp
	ffmpegInputAttributeView.cpp
	ffmpegMediaFilter.cpp
	ffmpegOutput.cpp
	ffmpegPacketSample.cpp
	ffmpegResources.cpp
	ffmpegSwScale.cpp
	formatDescription.cpp
)

set(ffmpeg_Headers
	codecDescription.h
	ffmpegAudioSample.h
	ffmpegcontrols.h
	ffmpegDecoder.h
	ffmpegEncoder.h
	ffmpegFrameSample.h
	ffmpegInput.h
	ffmpegInputAttributeView.h
	ffmpegMediaFilter.h
	ffmpegOutput.h
	ffmpegPacketSample.h
	ffmpegResources.h
	ffmpegSwScale.h
	formatDescription.h
)

set(ffmpeg_Libraries
	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
#	avcodec.lib
#	avformat.lib
#	avutil.lib
#	swscale.lib
	${FFMPEG_LIBRARIES}
)

QT5_WRAP_UI(ffmpeg_Uis
	ffmpegcontrols.ui
	ffmpegInputAttributeView.ui
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

#include_directories(${FFMPEG_INCLUDES})
#link_directories(${FFMPEG_LIB_PATH})
include_directories(${FFMPEG_INCLUDE_DIRS})

add_library(ffmpeg SHARED
	${ffmpeg_Sources}
	${ffmpeg_Headers}
	${ffmpeg_Uis}
)

target_link_libraries(ffmpeg ${ffmpeg_Libraries})
qt5_use_modules(ffmpeg Core Gui Widgets)

set_target_properties(ffmpeg PROPERTIES 
	FOLDER "filters"
)

add_dependencies(ffmpeg Utilities)
add_dependencies(ffmpeg Base)
add_dependencies(ffmpeg Media)
add_dependencies(ffmpeg QtComponents)

create_target_launcher(ffmpeg
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
