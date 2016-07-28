cmake_minimum_required (VERSION 2.8.9)
#cmake_policy(SET CMP0020 NEW)
cmake_policy(VERSION 3.1.0)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()
#if(POLICY CMP0026)
#	cmake_policy(SET CMP0026 OLD)
#endif()
#if(POLICY CMP0043)
#	cmake_policy(SET CMP0043 NEW)
#endif()
#if(POLICY CMP0053)
#	cmake_policy(SET CMP0053 NEW)
#endif()

project(LimitlessSDK)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

message(STATUS "CMake prefix: $ENV{CMAKE_PREFIX_PATH}")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
set(QT5_PULGIN_DIR $ENV{QTDIR}/plugins)

#find_package(Qt4 REQUIRED)
#find_package(OpenGl REQUIRED)
#message(STATUS "OpenGl dir:${OPENGL_INCLUDE_DIR}")
#find_package(GLEW REQUIRED)
#message(STATUS "Glew include dir:${GLEW_INCLUDE_DIRS}")
#message(STATUS "Glew libs:${GLEW_LIBRARIES}")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

message(STATUS "module path: ${CMAKE_MODULE_PATH}")

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

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED )
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK ON)
add_definitions(-DBOOST_ALL_DYN_LINK)

set(BOOST_ROOT $ENV{BOOST_DIR})
find_package(Boost REQUIRED COMPONENTS thread program_options system)
message(status "******Boost")
message(status "*Include Dir: ${Boost_INCLUDE_DIR}")
message(status "*Libraries Dir: ${Boost_LIBRARY_DIRS}")
message(status "*Libraries: ${Boost_LIBRARIES}")

#set(OPENCL_ROOT $ENV{OPENCL_DIR})
#find_package(OpenCL)
#message(status "******OpenCl")
#message(status "*Include Dir: ${OPENCL_INCLUDE_DIRS}")
#message(status "*Libraries Dir: ${OPENCL_LIBRARIES}")

#set(GLM_ROOT $ENV{GLM_DIR})
#find_package(GLM)
#message(status "******Glm")
#message(status "*Include Dir: ${GLM_INCLUDE_DIRS}")

#rapidjson
#find_package(rapidjson)

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