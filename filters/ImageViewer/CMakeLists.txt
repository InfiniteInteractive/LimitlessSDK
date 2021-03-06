#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(ImageViewer)

set(CMAKE_AUTOMOC ON)

find_package(Qt5OpenGL REQUIRED)
#find_package(GLM)
#find_package(GLEW)
include(AddPackage)

hunter_add_package(glm)
find_package(glm)
hunter_add_package(glew)
find_package(glew CONFIG REQUIRED)
#hunter_add_package(Boost COMPONENTS system thread chrono date_time)
#find_package(Boost CONFIG REQUIRED system thread chrono date_time)

#add_package("glm" VERSION "0.9.7.6")
#add_package("glew" VERSION "2.0.0")
add_package("opencl")

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(ImageViewer_Sources
	glview.cpp
	glwidget.cpp
	imageviewer.cpp
)

set(ImageViewer_Headers
	glview.h
	glwidget.h
	imageviewer.h
	imageviewer_global.h
)

set(ImageViewer_Libraries
    ${OPENCL_LIBRARIES}
    opengl32.lib
    glu32.lib
    Utilities.lib
    Base.lib
    Media.lib
    glew::glew
    Boost::system
    Boost::thread
    Boost::chrono
    Boost::date_time
)

QT5_WRAP_UI(ImageViewer_Uis
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

include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

add_library(ImageViewer SHARED
	${ImageViewer_Sources}
	${ImageViewer_Headers}
	${ImageViewer_Uis}
)

target_link_libraries(ImageViewer ${ImageViewer_Libraries})
qt5_use_modules(ImageViewer Core Gui Widgets OpenGL) 

set_target_properties(ImageViewer PROPERTIES 
	FOLDER "filters"
)

add_dependencies(ImageViewer Utilities)
add_dependencies(ImageViewer Base)
add_dependencies(ImageViewer Media)
add_dependencies(ImageViewer ControlStructures)
#add_dependencies(ImageViewer ${GLM_TARGET})

create_target_launcher(ImageViewer
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ImageViewer_BIN_DIRS ${GLEW_BIN_DIRS} CACHE INTERNAL "")
