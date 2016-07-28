#zlib puts a v in front of their version labels
set(ZLIB_GIT_TAG "v${ADD_PACKAGE_VERSION}")

add_git_package(${DEPENDENCY_PACKAGE_NAME} "https://github.com/madler/zlib.git" GIT_TAG ${ZLIB_GIT_TAG})

set(${DEPENDENCY_PACKAGE_NAME}_LIBRARIES ${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib/zlib.lib PARENT_SCOPE)

