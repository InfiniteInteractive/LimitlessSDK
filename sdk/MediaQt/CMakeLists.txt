#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(MediaQt)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(MediaQt_Sources
	QImageSample.cpp
)

set(MediaQt_Headers
	mediaqt_define.h
	QImageSample.h
	qtMediaDefines.h
)

set(MediaQt_Libraries
	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

add_library(MediaQt SHARED
	${MediaQt_Sources}
	${MediaQt_Headers}
)

target_link_libraries(MediaQt ${MediaQt_Libraries})
qt5_use_modules(MediaQt Core Widgets)

set_target_properties(MediaQt PROPERTIES 
	FOLDER "sdk"
)

add_dependencies(MediaQt Base)
add_dependencies(MediaQt Media)
add_dependencies(MediaQt Utilities)

create_target_launcher(MediaQt
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
