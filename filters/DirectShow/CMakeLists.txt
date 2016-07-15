#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(DirectShow)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(DirectShow_Sources
	DirectShowCapture.cpp
)

set(DirectShow_Headers
	DirectShowCapture.h
	QEdit.h
)

set(DirectShow_Libraries
	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
	AvStream.lib
#	optimized DirectShowBases.lib
#	debug DirectShowBasesd.lib
	$<$<CONFIG:Release>:strmbase.lib>
	$<$<CONFIG:Debug>:strmbasd.lib>
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

#include_directories(${CMAKE_SOURCE_DIR}/libraries/DirectShow)
#link_directories(${CMAKE_SOURCE_DIR}/libraries/DirectShowBases/lib)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${DIRECTSHOW_BASECLASSES_INCLUDE_DIR})
include_directories(${DIRECTSHOW_COMMON_INCLUDES_DIR})
link_directories(${DIRECTSHOW_LIB_PATH})

add_library(DirectShow SHARED
	${DirectShow_Sources}
	${DirectShow_Headers}
)

target_link_libraries(DirectShow ${DirectShow_Libraries})
qt5_use_modules(DirectShow Core Widgets)

set_target_properties(DirectShow PROPERTIES 
	FOLDER "filters"
)

add_dependencies(DirectShow Utilities)
add_dependencies(DirectShow Base)
add_dependencies(DirectShow Media)
add_dependencies(DirectShow AvStream)

create_target_launcher(DirectShow
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
