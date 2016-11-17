#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(ControlStructures)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

include(AddPackage)

add_package("opencl")
#add_package("glew" VERSION "2.0.0")

hunter_add_package(glew)
find_package(glew CONFIG REQUIRED)

hunter_add_package(Boost COMPONENTS system thread chrono date_time)
find_package(Boost CONFIG REQUIRED system thread chrono date_time)

set(ControlStructures_Sources
    branch.cpp
    combine.cpp
    gpuUploadSample.cpp
    gpuUpload.cpp
    join.cpp
    synchronize.cpp
    tee.cpp
)

set(ControlStructures_Headers
    branch.h
    combine.h
    gpuUploadSample.h
    gpuUpload.h
    join.h
    synchronize.h
    tee.h
)

set(ControlStructures_Libraries
    ${OPENCL_LIBRARIES}
    Utilities.lib
    Base.lib
    Media.lib
    opengl32.lib
    glew::glew
    Boost::system
    Boost::thread
    Boost::chrono
    Boost::date_time
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${OPENGL_EX_INCLUDE_DIR})

include_directories(${OPENCL_INCLUDE_DIRS})
#link_directories(${OPENCL_LIB_DIR})

add_library(ControlStructures SHARED
	${ControlStructures_Sources}
	${ControlStructures_Headers}
)

target_link_libraries(ControlStructures ${ControlStructures_Libraries})

set_target_properties(ControlStructures PROPERTIES 
	FOLDER "filters"
)

add_dependencies(ControlStructures Utilities)
add_dependencies(ControlStructures Base)
add_dependencies(ControlStructures Media)

create_target_launcher(ControlStructures
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
