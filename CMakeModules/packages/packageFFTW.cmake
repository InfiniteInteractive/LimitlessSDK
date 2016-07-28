if(WIN32)
	if(${ADD_PACKAGE_VERSION} STREQUAL "head")
		set(ADD_PACKAGE_VERSION "3.3.4")
	endif()

	set(FFTW_GIT_TAG "fftw-${ADD_PACKAGE_VERSION}")

	#project does not use cmake, so send source to install folder
	set(DEPENDENCY_PACKAGE_SOURCE_DIR ${DEPENDENCY_PACKAGE_INSTALL_DIR})
	add_package_update_git(${DEPENDENCY_PACKAGE_NAME} "https://github.com/FFTW/fftw3.git" GIT_TAG ${FFTW_GIT_TAG})
	
	#go get the precompiled version, as there is no cmake for this one
	set(FFTW_DOWNLOAD_URL "ftp://ftp.fftw.org/pub/fftw/")

	message(STATUS "FFTW tag: ${ADD_PACKAGE_VERSION}")
	
	set(FFTW_DOWNLOAD_FILE "fftw-${ADD_PACKAGE_VERSION}-dll64.zip")
	if(NOT ${ADD_PACKAGE_VERSION} STREQUAL "3.3.4")
		set(FFTW_DOWNLOAD_URL "${FFTW_DOWNLOAD_URL}old/")
	endif()

	string(SUBSTRING ${ADD_PACKAGE_VERSION} 0 3 FFTW_LIB_NAME)
	
	string(REPLACE "." "l-" FFTW_LIB_L_NAME ${FFTW_LIB_NAME})
	set(FFTW_LIB_L_NAME "libfftw${FFTW_LIB_L_NAME}")

	string(REPLACE "." "f-" FFTW_LIB_F_NAME ${FFTW_LIB_NAME})
	set(FFTW_LIB_F_NAME "libfftw${FFTW_LIB_F_NAME}")

	string(REPLACE "." "-" FFTW_LIB_NAME ${FFTW_LIB_NAME})
	set(FFTW_LIB_NAME "libfftw${FFTW_LIB_NAME}")

	message(STATUS "FFTW lib name ${FFTW_LIB_NAME}, ${FFTW_LIB_L_NAME}, ${FFTW_LIB_F_NAME}")

	set(FFTW_LOCAL_LIB_DIR ${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib)
	
	#check if library exists, if not download, unzip and build as needed
	if(NOT EXISTS ${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_NAME}.lib)
		#if no def files then need to decompress zip
		if(NOT EXISTS ${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_NAME}.def)
			set(FFTW_LOCAL_ZIP ${DEPENDENCY_PACKAGE_INSTALL_DIR}/${FFTW_DOWNLOAD_FILE})
				
			#if no zip, download
			if(NOT EXISTS ${FFTW_LOCAL_ZIP})
				message(STATUS "Downloading FFTW Precompiled")
				file(DOWNLOAD ${FFTW_DOWNLOAD_URL}/${FFTW_DOWNLOAD_FILE} ${FFTW_LOCAL_ZIP})
			endif()

			message(STATUS "Unzipping FFTW Precompiled")
			#command only works with .Net 4+
			execute_process(
				COMMAND powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('${FFTW_LOCAL_ZIP}', '${FFTW_LOCAL_LIB_DIR}'); }"
				WORKING_DIRECTORY ${FFTW_LOCAL_LIB_DIR}
			)

			if(EXISTS ${FFTW_LOCAL_ZIP})
				file(REMOVE ${FFTW_LOCAL_ZIP})
			endif()
		endif()
		
		get_filename_component(COMPILER_PATH ${CMAKE_CXX_COMPILER} DIRECTORY)
		string(FIND ${COMPILER_PATH} "bin" COMPILER_PATH_BIN_POS)
		string(SUBSTRING ${COMPILER_PATH} 0 ${COMPILER_PATH_BIN_POS} COMPILER_BIN_PATH)
		set(COMPILER_BIN_PATH ${COMPILER_BIN_PATH}bin)

		message(STATUS "Converting FFTW .def to .lib")
		message(STATUS "compiler path: ${COMPILER_BIN_PATH}")

		#set path to compiler directory otherwise dlls will not get picked up
#		if(NOT "$ENV{PATH}" MATCHES "${COMPILER_PATH}")
#			set(ENV{PATH} "$ENV{PATH};${COMPILER_PATH}")
#			message(STATUS "added compiler path: $ENV{PATH}")
#		endif()

		execute_process(
			COMMAND ${COMPILER_PATH}/lib /def:${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_NAME}.def /machine:X64 /out:${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_NAME}.lib
			WORKING_DIRECTORY ${COMPILER_BIN_PATH}
		)
		execute_process(
			COMMAND ${COMPILER_PATH}/lib /def:${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_F_NAME}.def /machine:X64 /out:${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_F_NAME}.lib
			WORKING_DIRECTORY ${COMPILER_BIN_PATH}
		)
		execute_process(
			COMMAND ${COMPILER_PATH}/lib /def:${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_L_NAME}.def /machine:X64 /out:${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_L_NAME}.lib
			WORKING_DIRECTORY ${COMPILER_BIN_PATH}
		)
	endif()

	set(${DEPENDENCY_PACKAGE_NAME}_INCLUDE_DIRS "${DEPENDENCY_PACKAGE_INSTALL_DIR}/api" PARENT_SCOPE)
	set(${DEPENDENCY_PACKAGE_NAME}_BIN_DIRS "${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib" PARENT_SCOPE)

	set(FFTW_LIBS ${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_NAME}.lib)
	set(FFTW_LIBS ${FFTW_LIBS} ${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_F_NAME}.lib)
	set(FFTW_LIBS ${FFTW_LIBS} ${FFTW_LOCAL_LIB_DIR}/${FFTW_LIB_L_NAME}.lib)

	set(${DEPENDENCY_PACKAGE_NAME}_LIBRARIES ${FFTW_LIBS} PARENT_SCOPE)
else()

endif()
