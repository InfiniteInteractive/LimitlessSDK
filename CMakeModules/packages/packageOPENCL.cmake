
#khronos does not tag, they branch instaed. So for the moment lets just take the head
set(OPENCL_GIT_TAG "head")

#OpenCl CmakeLists does not have a install so lets add one
set(OPENCL_INSTALL "\n\
file(COPY ${DEPENDENCY_PACKAGE_SOURCE_DIR}/inc/CL DESTINATION ${DEPENDENCY_PACKAGE_INSTALL_DIR}/include FILES_MATCHING PATTERN \"*.h\")\n\
file(COPY ${DEPENDENCY_PACKAGE_BUILD_DIR}/bin/Debug/OpenCl.dll DESTINATION ${DEPENDENCY_PACKAGE_INSTALL_DIR}/bin)\n\
file(COPY ${DEPENDENCY_PACKAGE_BUILD_DIR}/Debug/OpenCl.lib DESTINATION ${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib)\n\
\n")
set(OPENCL_INSTALL_FILE ${DEPENDENCY_PACKAGE_BUILD_DIR}/install.cmake)
file(WRITE ${OPENCL_INSTALL_FILE} ${OPENCL_INSTALL})
set(OPENCL_INSTALL_COMMAND ${CMAKE_COMMAND} -DBUILD_STEP=install -P ${OPENCL_INSTALL_FILE})

add_git_package(${DEPENDENCY_PACKAGE_NAME} "https://github.com/KhronosGroup/OpenCL-ICD-Loader.git" GIT_TAG "head" INSTALL_COMMAND ${OPENCL_INSTALL_COMMAND})
#add_git_package(${DEPENDENCY_PACKAGE_NAME} "https://github.com/KhronosGroup/OpenCL-ICD-Loader.git" GIT_TAG "head")
#put headers under include/CL
set(OPENCL_SOURCE_DIR ${DEPENDENCY_PACKAGE_SOURCE_DIR})
set(DEPENDENCY_PACKAGE_SOURCE_DIR ${DEPENDENCY_PACKAGE_SOURCE_DIR}/inc/CL)
add_package_update_git(${DEPENDENCY_PACKAGE_NAME} "https://github.com/KhronosGroup/OpenCL-Headers.git" GIT_TAG "head")
set(DEPENDENCY_PACKAGE_SOURCE_DIR ${OPENCL_SOURCE_DIR})

#CmakeList does not have a install so lets add one
#set(OPENCL_CMAKE_INSTALL "\n\
#install(DIRECTORY \${CMAKE_CURRENT_SOURCE_DIR}/inc/CL DESTINATION include\
# FILES_MATCHING PATTERN \"*.h\"\
# REGEX \"\${CMAKE_CURRENT_BINARY_DIR}\" EXCLUDE)\n\
# \n\
#install(TARGETS OpenCl\
# EXPORT OpenCl\
# RUNTIME DESTINATION bin\
# LIBRARY DESTINATION lib\
# ARCHIVE DESTINATION lib)\n")
#file(APPEND "${DEPENDENCY_PACKAGE_SOURCE_DIR}/CmakeLists.txt" ${OPENCL_CMAKE_INSTALL})


#hack, fetching cpp implementation as well here, need to add seperate but needs python to generate from git and dont want to (losing steam for this) create a package for python just yet
set(OPENCL_CPP_HEADER "https://www.khronos.org/registry/cl/api/2.1/cl.hpp")
#set(OPENCL_CPP2_HEADER "https://www.khronos.org/registry/cl/api/2.1/cl.hpp")

file(DOWNLOAD ${OPENCL_CPP_HEADER} ${DEPENDENCY_PACKAGE_INSTALL_DIR}/include/CL/cl.hpp)


set(${DEPENDENCY_PACKAGE_NAME}_LIBRARIES ${DEPENDENCY_PACKAGE_INSTALL_DIR}/lib/OpenCl.lib PARENT_SCOPE)

