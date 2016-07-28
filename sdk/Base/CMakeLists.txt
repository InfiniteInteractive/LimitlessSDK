#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(Base)

include(AddPackage)

add_package("rapidjson" VERSION "v1.0.2" )

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(Base_Sources
	AttributeContainer.cpp
	Attribute.cpp
	Attributes.cpp
#	json.cpp
	JsonSerializer.cpp
	Log.cpp
	PluginFactory.cpp
	PluginObject.cpp
)

set(Base_Headers
	AttributeContainer.h
	Attribute.h
	Attributes.h
	base_define.h
	common.h
#	json.h
	JsonSerializer.h
	Log.h
	MemoryLocation.h
	PluginFactory.h
	PluginObject.h
	PluginView.h
	Serializer.h
	typeName.h
	types.h
)

set(Base_Libraries
	${Boost_LIBRARIES}
#	Utilities.lib
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_definitions(-DRAPIDJSON_HAS_STDSTRING)
#include_directories(${RAPIDJSON_INCLUDE_DIRS})
include_directories(${RAPIDJSON_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4800") #remove performance warnings

add_library(Base SHARED
	${Base_Sources}
	${Base_Headers}
)

target_link_libraries(Base ${Base_Libraries})
#qt5_use_modules(Base Core)

set_target_properties(Base PROPERTIES 
	FOLDER "sdk"
)

#add_dependencies(Base Utilities)
add_dependencies(Base ${RAPIDJSON_TARGET})

create_target_launcher(Base
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
