#dlib puts a v in front of their version labels
set(DLIB_GIT_TAG "v${ADD_PACKAGE_VERSION}")

add_git_package(${DEPENDENCY_PACKAGE_NAME} "https://github.com/davisking/dlib.git" GIT_TAG ${DLIB_GIT_TAG})

set(${DEPENDENCY_PACKAGE_NAME}_INCLUDE_DIRS "${DEPENDENCY_PACKAGE_INSTALL_DIR}/include" PARENT_SCOPE)
set(${DEPENDENCY_PACKAGE_NAME}_LIBRARIES ${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib/dlib.lib PARENT_SCOPE)
