add_subdirectory(cvlib)
add_subdirectory(medialib)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/privateProjects.cmake)
	include(${CMAKE_CURRENT_LIST_DIR}/privateProjects.cmake)
endif()