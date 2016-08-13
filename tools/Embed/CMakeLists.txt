#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(Embed)

#Set build directory
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TOPLEVEL_BINARY_DIR}/bin/tools/ )
# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${TOPLEVEL_BINARY_DIR}/bin/tools/ )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

if(WIN32)
	add_definitions(-DNOMINMAX)#remove min and max macro declares from windows builds, causes issues with std::min/max
	add_definitions(-DWINDOWS)
endif()

set(Embed_Sources
	getopt.cpp
	main.cpp
)

set(Embed_Headers
	getopt.h
)

set(Embed_Libraries
	${Boost_LIBRARIES}
)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

#copy boost filesystem to tools folder
file(GLOB Boost_Filesystem_Dlls ${Boost_LIBRARY_DIRS}/boost_filesystem-*.dll ${Boost_LIBRARY_DIRS}/boost_system-*.dll)
message(STATUS "Boost files: ${Boost_Filesystem_Dlls}")
file(COPY ${Boost_Filesystem_Dlls} DESTINATION ${CMAKE_BINARY_DIR}/bin/tools)

add_executable(Embed WIN32
	${Embed_Sources}
	${Embed_Headers}
	${Embed_Uis}
)
if(WIN32)
   set_target_properties(Embed PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
   set_target_properties(Embed PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
   set_target_properties(Embed PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
   set_target_properties(Embed PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
   set_target_properties(Embed PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
   set_target_properties(Embed PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:CONSOLE")
endif(WIN32)

set_target_properties(Embed PROPERTIES 
	FOLDER "tools"
)

create_target_launcher(Embed
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tools"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
