message(STATUS "AudioControls CMake Started")

#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(AudioControls)

set(CMAKE_AUTOMOC ON)
find_package(Qt5OpenGL REQUIRED)
#find_package(GLM)
#find_package(GLEW)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(AudioControls_Sources
	AudioMeterView.cpp
	audioMeter.cpp
	AudioMixer.cpp
	AudioMixerInputView.cpp
	AudioMixerMixView.cpp
	AudioMixerOutputView.cpp
	AudioMixerView.cpp
	medialibHelpers.cpp
)

set(AudioControls_Headers
	AudioControls_global.h
	AudioMeterView.h
	audioMeter.h
	AudioMixer.h
	AudioMixerInfo.h
	AudioMixerInputView.h
	AudioMixerMixView.h
	AudioMixerOutputView.h
	AudioMixerView.h
	medialibHelpers.h
)

set(AudioControls_Libraries
	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
	QtComponents.lib
	medialib.lib
)

QT5_WRAP_UI(AudioControls_Uis
    AudioMeterView.ui
	AudioMixerInputView.ui
	AudioMixerMixView.ui
	AudioMixerOutputView.ui
	AudioMixerView.ui
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})
include_directories(${LIBRARIES_PATH}/medialib)

include_directories(${FILTERS_PATH})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER}})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${OPENCL_INCLUDE_DIRS})
link_directories(${OPENCL_LIB_DIR})

#include_directories(${GLEW_INCLUDE_DIRS})
#include_directories(${GLM_INCLUDE_DIRS})

add_library(AudioControls SHARED
	${AudioControls_Sources}
	${AudioControls_Headers}
	${AudioControls_Uis}
)

target_link_libraries(AudioControls ${AudioControls_Libraries})
qt5_use_modules(AudioControls Core Widgets OpenGL) 

set_target_properties(AudioControls PROPERTIES 
	FOLDER "filters"
)

add_dependencies(AudioControls Utilities)
add_dependencies(AudioControls Base)
add_dependencies(AudioControls Media)
add_dependencies(AudioControls ControlStructures)
add_dependencies(AudioControls QtComponents)
add_dependencies(AudioControls medialib)

create_target_launcher(AudioControls
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(AudioControls_BIN_DIRS ${medialib_BIN_DIRS} CACHE INTERNAL "")

