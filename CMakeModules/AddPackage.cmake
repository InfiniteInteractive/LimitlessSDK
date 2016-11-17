set(ADD_PACKAGE_FILE_DIR ${CMAKE_CURRENT_LIST_DIR})

include(ExternalProject)

#macro(ADD_PACKAGE_initCPM)
	if(NOT DEFINED ADD_PACKAGE_INIT_CPM)

		if(NOT DEFINED ADD_PACKAGE_DIR) #not set assume part of source tree
			set(ADD_PACKAGE_DIR "${CMAKE_SOURCE_DIR}/packages/" CACHE STRING)
		endif()
		set(CPM_DIR "${ADD_PACKAGE_DIR}/CPM")
#		set(CPM_ROOT_BIN_DIR "${ADD_PACKAGE_DIR}/bin")

		find_package(Git)

		if(NOT GIT_FOUND)
			message(FATAL_ERROR "CPM requires Git.")
		endif()
		if (NOT EXISTS ${CPM_DIR}/CPM.cmake)
			message(STATUS "Cloning repo (https://github.com/iauns/cpm)")
			execute_process(
				COMMAND "${GIT_EXECUTABLE}" clone https://github.com/iauns/cpm ${CPM_DIR}
				RESULT_VARIABLE error_code
				OUTPUT_QUIET ERROR_QUIET
			)
			if(error_code)
				message(FATAL_ERROR "CPM failed to get the hash for HEAD")
			endif()
		endif()

		include(${CPM_DIR}/CPM.cmake)
		CPM_Finish()

		set(ADD_PACKAGE_INIT_CPM TRUE)
	endif()
#endmacro(ADD_PACKAGE_initCPM)

function(add_package DEPENDENCY)
#	ADD_PACKAGE_initCPM() #init if not already
	set(options "")
	set(oneValueArgs VERSION)
	set(multiValueArgs "")
	cmake_parse_arguments(ADD_PACKAGE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	#set cpm folders
	if(NOT DEFINED ADD_PACKAGE_VERSION)
		set(ADD_PACKAGE_VERSION "head")
	endif()

	message(STATUS "Add package: ${DEPENDENCY} ${ADD_PACKAGE_VERSION}")

	set(DEPENDENCY_PACKAGE_NAME "${DEPENDENCY}")
	string(TOUPPER ${DEPENDENCY_PACKAGE_NAME} DEPENDENCY_PACKAGE_NAME)

	set(DEPENDENCY_PACKAGE_FULLNAME "${DEPENDENCY}-${ADD_PACKAGE_VERSION}")
	
	if(NOT DEFINED ADD_PACKAGE_${DEPENDENCY_PACKAGE_NAME})
		set(DEPENDENCY_PACKAGE_SOURCE_DIR "${ADD_PACKAGE_DIR}/source/${DEPENDENCY_PACKAGE_FULLNAME}")
		set(DEPENDENCY_PACKAGE_BUILD_DIR "${ADD_PACKAGE_DIR}/build/${DEPENDENCY_PACKAGE_FULLNAME}")
		set(DEPENDENCY_PACKAGE_INSTALL_DIR "${ADD_PACKAGE_DIR}/${DEPENDENCY_PACKAGE_FULLNAME}")

		get_filename_component(DEPENDENCY_PACKAGE_SOURCE_DIR ${DEPENDENCY_PACKAGE_SOURCE_DIR} ABSOLUTE)
		get_filename_component(DEPENDENCY_PACKAGE_BUILD_DIR ${DEPENDENCY_PACKAGE_BUILD_DIR} ABSOLUTE)
		get_filename_component(DEPENDENCY_PACKAGE_INSTALL_DIR ${DEPENDENCY_PACKAGE_INSTALL_DIR} ABSOLUTE)

#		message(STATUS "add_package: ${DEPENDENCY}")
#		message(STATUS "Pkg Source Dir: ${DEPENDENCY_PACKAGE_SOURCE_DIR}")
#		message(STATUS "Pkg Build Dir: ${DEPENDENCY_PACKAGE_BUILD_DIR}")
#		message(STATUS "Pkg Install Dir: ${DEPENDENCY_PACKAGE_INSTALL_DIR}")

#		set(CPM_LOCAL_PACKAGE "${CMAKE_MODULE_PATH}/packages/package${DEPENDENCY}.cmake")
		set(CPM_LOCAL_PACKAGE "${ADD_PACKAGE_FILE_DIR}/packages/package${DEPENDENCY}.cmake")
		if(EXISTS ${CPM_LOCAL_PACKAGE})
			include(${CPM_LOCAL_PACKAGE})
		else() #try default loading the package from github
			if(NOT DEFINED ADD_PACKAGE_GIT_REPOSITORY) #not set assume it is from github
				set(ADD_PACKAGE_GIT_REPOSITORY "https://github.com/${DEPENDENCY}/${DEPENDENCY}.git")
			endif()

		#		message(STATUS "Git Repo: ${ADD_PACKAGE_GIT_REPOSITORY}")
		#		message(STATUS "Git Tag: ${ADD_PACKAGE_VERSION}")

			add_git_package(${DEPENDENCY} ${ADD_PACKAGE_GIT_REPOSITORY} GIT_TAG ${ADD_PACKAGE_VERSION})

			set(${DEPENDENCY_PACKAGE_NAME}_ROOT "${DEPENDENCY_PACKAGE_INSTALL_DIR}" PARENT_SCOPE)
			set(${DEPENDENCY_PACKAGE_NAME}_INCLUDE_DIRS "${DEPENDENCY_PACKAGE_DIR}/include" PARENT_SCOPE)
			set(${DEPENDENCY_PACKAGE_NAME}_BIN_DIRS "${DEPENDENCY_PACKAGE_INSTALL_DIR}/bin" PARENT_SCOPE)
			set(${DEPENDENCY_PACKAGE_NAME}_LIBRARIES "${DEPENDENCY_PACKAGE_DIR}/lib/${DEPENDENCY}.lib" PARENT_SCOPE)

		#		set_target_properties(${DEPENDENCY} PROPERTIES FOLDER "packages")
		endif()
		set(ADD_PACKAGE_${DEPENDENCY_PACKAGE_NAME} TRUE PARENT_SCOPE)
	else()
#		message(STATUS "add_package: Package ${DEPENDENCY} already added")
	endif()
endfunction(add_package)

macro(add_package_update_git DEPENDENCY GIT_REPOSITORY)
	set(options "")
	set(oneValueArgs GIT_TAG)
	set(multiValueArgs "")
	cmake_parse_arguments(ADD_PACKAGE_UPDATE_GIT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	if(NOT DEFINED ADD_PACKAGE_UPDATE_GIT_GIT_TAG OR ${ADD_PACKAGE_UPDATE_GIT_GIT_TAG} STREQUAL "head") #not set or head, set to origin master
		set(ADD_PACKAGE_UPDATE_GIT_GIT_TAG "origin/master")
	endif()

#	message(STATUS "add_package_update_git: ${DEPENDENCY} ${GIT_REPOSITORY}")
#	message(STATUS "Pkg Source Dir: ${DEPENDENCY_PACKAGE_SOURCE_DIR}")
#	message(STATUS "Pkg Build Dir: ${DEPENDENCY_PACKAGE_BUILD_DIR}")
#	message(STATUS "Pkg Instal Dir: ${DEPENDENCY_PACKAGE_INSTALL_DIR}")
#	message(STATUS "Git Repo: ${GIT_REPOSITORY}")
#	message(STATUS "Git Tag: ${ADD_PACKAGE_UPDATE_GIT_GIT_TAG}")

	CPM_EnsureRepoIsCurrent(TARGET_DIR ${DEPENDENCY_PACKAGE_SOURCE_DIR}
		GIT_REPOSITORY ${GIT_REPOSITORY}
		GIT_TAG ${ADD_PACKAGE_UPDATE_GIT_GIT_TAG}
		USE_CACHING TRUE
	)
endmacro(add_package_update_git)

macro(add_package_project)
	if(NOT TARGET ${DEPENDENCY_PACKAGE_FULLNAME})
		set(options "")
		set(oneValueArgs "")
		set(multiValueArgs CMAKE_ARGS INSTALL_COMMAND)
		cmake_parse_arguments(ADD_PACKAGE_PROJECT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

#		message(STATUS "add_package_project CMAKE_ARGS: ${ADD_GIT_PACKAGE_CMAKE_ARGS}")

		set(CMAKE_INSTALL_PREFIX ${DEPENDENCY_PACKAGE_INSTALL_DIR})
		set(TMP_CMAKE_ARGS "${CMAKE_ARGS};-DCMAKE_INSTALL_PREFIX=${DEPENDENCY_PACKAGE_INSTALL_DIR};${ADD_PACKAGE_PROJECT_CMAKE_ARGS}")
#		message(STATUS "CMAKE_ARGS: ${TMP_CMAKE_ARGS}")

		if(DEFINED ADD_PACKAGE_PROJECT_INSTALL_COMMAND)
			ExternalProject_Add(${DEPENDENCY_PACKAGE_FULLNAME} 
				SOURCE_DIR ${DEPENDENCY_PACKAGE_SOURCE_DIR} 
				BINARY_DIR ${DEPENDENCY_PACKAGE_BUILD_DIR} 
				INSTALL_COMMAND ${ADD_PACKAGE_PROJECT_INSTALL_COMMAND}
				CMAKE_ARGS "${CMAKE_ARGS};-DCMAKE_INSTALL_PREFIX=${DEPENDENCY_PACKAGE_INSTALL_DIR};${ADD_PACKAGE_PROJECT_CMAKE_ARGS}"
			)
		else()
			ExternalProject_Add(${DEPENDENCY_PACKAGE_FULLNAME} 
				SOURCE_DIR ${DEPENDENCY_PACKAGE_SOURCE_DIR} 
				BINARY_DIR ${DEPENDENCY_PACKAGE_BUILD_DIR} 
				CMAKE_ARGS "${CMAKE_ARGS};-DCMAKE_INSTALL_PREFIX=${DEPENDENCY_PACKAGE_INSTALL_DIR};${ADD_PACKAGE_PROJECT_CMAKE_ARGS}"
			)
		endif()
		set(${DEPENDENCY_PACKAGE_FULLNAME}_EXTERNALPROJECT_ADD TRUE PARENT_SCOPE)
	endif()
	set(${DEPENDENCY_PACKAGE_NAME}_TARGET ${DEPENDENCY_PACKAGE_FULLNAME} PARENT_SCOPE)
endmacro(add_package_project)

macro(add_git_package DEPENDENCY GIT_REPOSITORY)
	set(options "")
	set(oneValueArgs GIT_TAG)
	set(multiValueArgs CMAKE_ARGS INSTALL_COMMAND)
	cmake_parse_arguments(ADD_GIT_PACKAGE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

#	message(STATUS "add_git_package CMAKE_ARGS: ${ADD_GIT_PACKAGE_CMAKE_ARGS}")

	add_package_update_git(${DEPENDENCY} ${GIT_REPOSITORY} GIT_TAG ${ADD_GIT_PACKAGE_GIT_TAG})
	if(DEFINED ADD_GIT_PACKAGE_INSTALL_COMMAND)
		add_package_project(CMAKE_ARGS ${ADD_GIT_PACKAGE_CMAKE_ARGS} INSTALL_COMMAND ${ADD_GIT_PACKAGE_INSTALL_COMMAND})
	else()
		add_package_project(CMAKE_ARGS ${ADD_GIT_PACKAGE_CMAKE_ARGS})
	endif()
	
	#set default directories, package file can override after call
	set(${DEPENDENCY_PACKAGE_NAME}_ROOT "${DEPENDENCY_PACKAGE_INSTALL_DIR}" PARENT_SCOPE)
	set(${DEPENDENCY_PACKAGE_NAME}_INCLUDE_DIRS "${DEPENDENCY_PACKAGE_INSTALL_DIR}/include" PARENT_SCOPE)
	set(${DEPENDENCY_PACKAGE_NAME}_BIN_DIRS "${DEPENDENCY_PACKAGE_INSTALL_DIR}/bin" PARENT_SCOPE)

endmacro(add_git_package)

