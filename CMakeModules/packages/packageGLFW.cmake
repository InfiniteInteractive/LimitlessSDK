string(REPLACE "." ";" GLFW_VERSION_LIST ${ADD_PACKAGE_VERSION})
list(GET GLFW_VERSION_LIST 0 GLFW_VERSION_MAJOR)
list(GET GLFW_VERSION_LIST 1 GLFW_VERSION_MINOR)
list(GET GLFW_VERSION_LIST 2 GLFW_VERSION_PATCH)

set(GLFW_DOWNLOAD_PRE_COMPILED FALSE)

#pre 3.2.1 GLFW didn't use cmake so it needs to be downloaded
if(GLFW_VERSION_MAJOR LESS 3)
	set(GLFW_DOWNLOAD_PRE_COMPILED TRUE)
else()
	if(GLFW_VERSION_MAJOR EQUAL 3 AND GLFW_VERSION_MINOR LESS 2)
		set(GLFW_DOWNLOAD_PRE_COMPILED TRUE)
	else()
		if(GLFW_VERSION_MINOR EQUAL 2 AND GLFW_VERSION_PATCH LESS 1)
			set(GLFW_DOWNLOAD_PRE_COMPILED TRUE)
		endif()
	endif()
endif()

if(NOT ${GLFW_DOWNLOAD_PRE_COMPILED}) #get from github and add as project
	set(GLFW_CMAKE_ARGS "-GLFW_BUILD_EXAMPLES=OFF")
	set(GLFW_CMAKE_ARGS "${GLFW_CMAKE_ARGS};-GLFW_BUILD_TESTS=OFF")
	set(GLFW_CMAKE_ARGS "${GLFW_CMAKE_ARGS};-GLFW_INSTALL=ON")

	add_git_package(${DEPENDENCY_PACKAGE_NAME} "https://github.com/glfw/glfw.git" GIT_TAG ${ADD_PACKAGE_VERSION} CMAKE_ARGS ${GLFW_CMAKE_ARGS})

	set(${DEPENDENCY_PACKAGE_NAME}_INCLUDE "${DEPENDENCY_PACKAGE_INSTALL_DIR}/include" PARENT_SCOPE)
	set(${DEPENDENCY_PACKAGE_NAME}_LIBRARIES ${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib/glfw3.lib PARENT_SCOPE)
else()
	if(WIN32)
		set(GLFW_ZIP_URL "https://github.com/glfw/glfw/releases/download/${GLFW_VERSION_MAJOR}.${GLFW_VERSION_MINOR}")
		set(GLFW_ZIP_FOLDER "glfw-${GLFW_VERSION_MAJOR}.${GLFW_VERSION_MINOR}.bin.WIN64")
		set(GLFW_ZIP_FILE "${GLFW_ZIP_URL}/${GLFW_ZIP_FOLDER}.zip")

		set(GLFW_LIB "glfw3.lib")
		set(GLFW_LOCAL_LIB_DIR ${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib)
	
		#check if library exists, if not download, unzip and build as needed
		if(NOT EXISTS ${GLFW_LOCAL_LIB_DIR}/${GLFW_LIB})

			set(GLFW_LOCAL_ZIP ${DEPENDENCY_PACKAGE_INSTALL_DIR}/${GLFW_ZIP_FOLDER}.zip)
			#if no zip, download
			if(NOT EXISTS ${GLFW_LOCAL_ZIP})
				message(STATUS "Downloading GLFW Precompiled")
				file(DOWNLOAD ${GLFW_ZIP_FILE} ${GLFW_LOCAL_ZIP})
			endif()

			message(STATUS "Unzipping GLFW Precompiled")

			set(GLFW_ZIP_INCLUDE "${GLFW_LOCAL_ZIP}/${GLFW_ZIP_FOLDER}/include")
			set(GLFW_ZIP_LIB "${GLFW_LOCAL_ZIP}/${GLFW_ZIP_FOLDER}/lib-vc2015")

			set(GLFW_INCLUDE_DIR "${DEPENDENCY_PACKAGE_INSTALL_DIR}/include")
			set(GLFW_LIB_DIR "${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib")

			#command only works with .Net 4+
			execute_process(
				COMMAND powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('${GLFW_ZIP_INCLUDE}', '${GLFW_INCLUDE_DIR}'); }"
				WORKING_DIRECTORY ${FFTW_LOCAL_LIB_DIR}
			)

			execute_process(
				COMMAND powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('${GLFW_ZIP_LIB}', '${GLFW_LIB_DIR}'); }"
				WORKING_DIRECTORY ${FFTW_LOCAL_LIB_DIR}
			)

			if(EXISTS ${GLFW_LOCAL_ZIP})
				file(REMOVE ${GLFW_LOCAL_ZIP})
			endif()
		endif()
		
		set(${DEPENDENCY_PACKAGE_NAME}_INCLUDE_DIRS "${DEPENDENCY_PACKAGE_INSTALL_DIR}/include" PARENT_SCOPE)
		set(${DEPENDENCY_PACKAGE_NAME}_LIBRARIES ${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib/glfw3.lib PARENT_SCOPE)
		set(${DEPENDENCY_PACKAGE_NAME}_BIN_DIRS "${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib" PARENT_SCOPE)

	endif()
endif()

