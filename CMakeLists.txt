cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
message(STATUS "CMake prefix: $ENV{CMAKE_PREFIX_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)
message(STATUS "module path: ${CMAKE_MODULE_PATH}")

#load Hunter packager
#include(HunterPackage)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/Hunter/HunterGate.cmake)

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.18.43.tar.gz"
    SHA1 "d2c8c42cd07f7cefe18fd9a9b9c13114b1a15a27"
    LOCAL
)

project(LimitlessSDK)

set(CMAKE_DEBUG_POSTFIX "")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
set(QT5_BIN_DIR ${QT_ROOT_DIR}/bin)
set(QT5_PULGIN_DIR ${QT_ROOT_DIR}/plugins)

#find_package(Qt4 REQUIRED)
#find_package(OpenGl REQUIRED)
#message(STATUS "OpenGl dir:${OPENGL_INCLUDE_DIR}")
#find_package(GLEW REQUIRED)
#message(STATUS "Glew include dir:${GLEW_INCLUDE_DIRS}")
#message(STATUS "Glew libs:${GLEW_LIBRARIES}")
hunter_add_package(glew)
find_package(glew CONFIG REQUIRED)

#check for 64bit build
if( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
	set(BUILD_x64 TRUE)
else( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
    set(BUILD_x64 FALSE)
endif( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
  
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/../ CACHE PATH "Bin directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib CACHE PATH "Library directory" FORCE)
set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/resources/)
set(COMMON_INCLUDES ${CMAKE_SOURCE_DIR}/common/)
set(SDK_INCLUDES ${CMAKE_SOURCE_DIR}/sdk/)
set(FILTERS_PATH ${CMAKE_SOURCE_DIR}/filters/)
set(LIBRARIES_PATH ${CMAKE_SOURCE_DIR}/libraries/)

#Boost
#set(Boost_INCLUDE_DIR $ENV{BOOST_DIR}/include)
#if(BUILD_x64)
#	set(Boost_LIBRARY_DIRS $ENV{BOOST_DIR}/lib/x64)
#else(BUILD_x64)
#	set(Boost_LIBRARY_DIRS $ENV{BOOST_DIR}/lib/x86)
#endif(BUILD_x64)
#set(Boost_LIBRARIES)

#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREADED )
#set(Boost_USE_STATIC_RUNTIME OFF)
#set(BOOST_ALL_DYN_LINK ON)
add_definitions(-DBOOST_ALL_DYN_LINK)
#
#set(BOOST_ROOT $ENV{BOOST_DIR})
hunter_add_package(Boost COMPONENTS thread regex filesystem program_options system chrono date_time)
find_package(Boost REQUIRED COMPONENTS thread regex filesystem program_options system chrono date_time)

message(status "******Boost")
message(status "*Include Dir: ${Boost_INCLUDE_DIR}")
message(status "*Libraries Dir: ${Boost_LIBRARY_DIRS}")
message(status "*Libraries: ${Boost_LIBRARIES}")

#set(OPENCL_ROOT $ENV{OPENCL_DIR})
#find_package(OpenCL)
#message(status "******OpenCl")
#message(status "*Include Dir: ${OPENCL_INCLUDE_DIRS}")
#message(status "*Libraries Dir: ${OPENCL_LIBRARIES}")
hunter_add_package(OpenCL)
find_package(OpenCL CONFIG REQUIRED)

hunter_add_package(OpenCL-cpp)
find_package(OpenCL-cpp CONFIG REQUIRED)

#set(GLM_ROOT $ENV{GLM_DIR})
#find_package(GLM)
#message(status "******Glm")
#message(status "*Include Dir: ${GLM_INCLUDE_DIRS}")

#rapidjson
#find_package(rapidjson)

#forge
set(FORGE_DEBUG_DIR ${FORGE_ROOT_DIR}/_buildx64/src/backend/opengl/Debug)
set(FORGE_RELEASE_DIR ${FORGE_ROOT_DIR}/_buildx64/src/backend/opengl/Release)

set(TOPLEVEL_BINARY_DIR ${PROJECT_BINARY_DIR})

#Set build directory
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/ )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/ )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/ )
# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${PROJECT_BINARY_DIR}/bin/${OUTPUTCONFIG}/ )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${PROJECT_BINARY_DIR}/lib/${OUTPUTCONFIG}/ )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${PROJECT_BINARY_DIR}/lib/${OUTPUTCONFIG}/ )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

#default paths
set(RUNTIME_LIBS
	${QT_BINARY_DIR}
#	${CMAKE_SOURCE_DIR}/libraries/GStreamer/bin/
)

include(CreateLaunchers)

add_subdirectory(tools)
add_subdirectory(sdk)
add_subdirectory(libraries)
#add_subdirectory(plugins)
add_subdirectory(filters)
add_subdirectory(applications)