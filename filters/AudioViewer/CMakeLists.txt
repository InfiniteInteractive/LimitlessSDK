#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(AudioViewer)

find_package(Qt5OpenGL REQUIRED)
#find_package(GLM)
#find_package(GLEW)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(AudioViewer_Sources
	glview.cpp
	glwidget.cpp
	AudioViewer.cpp
)

set(AudioViewer_Headers
	glview.h
	glwidget.h
	AudioViewer.h
	AudioViewer_global.h
)

set(AudioViewer_Libraries
	${Boost_LIBRARIES}
	${OPENCL_LIBRARIES}
	opengl32.lib
	glu32.lib
	Utilities.lib
	Base.lib
	Media.lib
	
	${GLEW_LIBRARIES}
)

QT5_WRAP_UI(AudioViewer_Uis
    glview.ui
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${FILTERS_PATH})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER}})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${OPENCL_INCLUDE_DIRS})
link_directories(${OPENCL_LIB_DIR})

#include_directories(${GLEW_INCLUDE_DIRS})
#include_directories(${GLM_INCLUDE_DIRS})

add_library(AudioViewer SHARED
	${AudioViewer_Sources}
	${AudioViewer_Headers}
	${AudioViewer_Uis}
)

target_link_libraries(AudioViewer ${AudioViewer_Libraries})
qt5_use_modules(AudioViewer Core Widgets OpenGL) 

set_target_properties(AudioViewer PROPERTIES 
	FOLDER "filters"
)

add_dependencies(AudioViewer Utilities)
add_dependencies(AudioViewer Base)
add_dependencies(AudioViewer Media)
add_dependencies(AudioViewer ControlStructures)

create_target_launcher(AudioViewer
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
