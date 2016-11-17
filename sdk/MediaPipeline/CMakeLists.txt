#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(MediaPipeline)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)
#add_definitions(-DBOOST_ALL_DYN_LINK)

hunter_add_package(Boost COMPONENTS system thread)
find_package(Boost CONFIG REQUIRED system thread)

set(MediaPipeline_Sources
	MediaPipeline.cpp
)

set(MediaPipeline_Headers
	IPipelineCallback.h
	MediaPipeline.h
	mediapipeline_define.h
)

set(MediaPipeline_Libraries
	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
    Boost::system
    Boost::thread
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_library(MediaPipeline SHARED
	${MediaPipeline_Sources}
	${MediaPipeline_Headers}
)

target_link_libraries(MediaPipeline ${MediaPipeline_Libraries})

set_target_properties(MediaPipeline PROPERTIES 
	FOLDER "sdk"
)

add_dependencies(MediaPipeline Base)
add_dependencies(MediaPipeline Media)

create_target_launcher(MediaPipeline
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
