#png needs zlib, current hack to just get a version
add_package("zlib" VERSION "1.2.8")

#png puts a v in front of their version labels
set(PNG_GIT_TAG "v${ADD_PACKAGE_VERSION}")

#need to tell png's cmake where to get zlib
add_git_package(${DEPENDENCY_PACKAGE_NAME} "https://github.com/glennrp/libpng.git" GIT_TAG ${PNG_GIT_TAG} CMAKE_ARGS "-DZLIB_ROOT=${ZLIB_ROOT}")

#set zlib as a dependency so it gets built as well
add_dependencies(${DEPENDENCY_PACKAGE_FULLNAME} ${ZLIB_TARGET})

set(${DEPENDENCY_PACKAGE_NAME}_LIBRARIES ${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib/libpng16d.lib PARENT_SCOPE)