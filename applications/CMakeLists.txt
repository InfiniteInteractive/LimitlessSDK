#set build folder to bin
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${PROJECT_BINARY_DIR}/bin/${OUTPUTCONFIG} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

add_subdirectory(Conversion)
add_subdirectory(DebugApp)
#add_subdirectory(MediaStreamer)
add_subdirectory(Sanctuary)


if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/privateProjects.cmake)
	include(${CMAKE_CURRENT_LIST_DIR}/privateProjects.cmake)
endif()