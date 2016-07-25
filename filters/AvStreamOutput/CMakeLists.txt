#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
	#cmake_policy(SET CMP0020 NEW)
#endif()

project(AvStreamOutput)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

#direcshow
set(DIRECTSHOW_DIR $ENV{DIRECTSHOW_DIR})
set(DIRECTSHOW_BASECLASSES_INCLUDE_DIR ${DIRECTSHOW_DIR}/baseclasses/)
set(DIRECTSHOW_COMMON_INCLUDES_DIR ${DIRECTSHOW_DIR}/common/)
if(BUILD_x64)
	set(DIRECTSHOW_LIB_PATH ${DIRECTSHOW_DIR}/lib/x64)
else(BUILD_x64)
	set(DIRECTSHOW_LIB_PATH ${DIRECTSHOW_DIR}/lib/)
endif(BUILD_x64)

set(AvStreamOutput_Sources
	AvStreamOutput.cpp
	FMLEControl.cpp
	FMLEProfile.cpp
)

set(AvStreamOutput_Headers
	AvStreamOutput.h
	FMLEControl.h
	FMLEProfile.h
)

set(AvStreamOutput_Libraries
	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
	AvStream.lib
#	optimized DirectShowBases.lib
#	debug DirectShowBasesd.lib
	$<$<CONFIG:Release>:${DIRECTSHOW_LIB_PATH}/strmbase.lib>
	$<$<CONFIG:Debug>:${DIRECTSHOW_LIB_PATH}/strmbasd.lib>
	strmiids.lib
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_library(AvStreamOutput SHARED
	${AvStreamOutput_Sources}
	${AvStreamOutput_Headers}
)

target_link_libraries(AvStreamOutput ${AvStreamOutput_Libraries})
qt5_use_modules(AvStreamOutput Core Widgets)

set_target_properties(AvStreamOutput PROPERTIES 
	FOLDER "filters"
)

add_dependencies(AvStreamOutput Utilities)
add_dependencies(AvStreamOutput Base)
add_dependencies(AvStreamOutput Media)
add_dependencies(AvStreamOutput AvStream)

create_target_launcher(AvStreamOutput
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
