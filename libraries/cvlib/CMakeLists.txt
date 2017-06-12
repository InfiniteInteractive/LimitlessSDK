project(cvlib)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

hunter_add_package(png)
find_package(png CONFIG REQUIRED)

hunter_add_package(Eigen)
find_package(Eigen3 REQUIRED)

hunter_add_package(dlib)
find_package(dlib CONFIG REQUIRED)

hunter_add_package(OpenCL)
find_package(OpenCL CONFIG REQUIRED)

#find_package(FFTW)
#find_package(dlib)

set(cvlib_sources
    convolve_cl.cpp
    image.cpp
    kernels_cl.cpp
    match.cpp
    png.cpp
    ppm.cpp
    simpleImage.cpp
    utils_cl.cpp
)

set(cvlib_headers
    cvlib/convolve_cl.h
    cvlib/cvlibDefine.h
    cvlib/image.h
    cvlib/imageDLib.h
    cvlib/kernels_cl.h
    cvlib/match.h
    cvlib/png.h
    cvlib/ppm.h
    cvlib/simpleImage.h
    cvlib/slidingWindow.h
    cvlib/utils_cl.h
)

set(cvlib_libraries
    dlib::dlib
    PNG::png
    OpenCL::OpenCL
    Eigen3::Eigen
)

include(EmbedFile)
set(embedFiles
    kernels/convolve.cl
    kernels/utils.cl
)
embed_files(embededFiles "${embedFiles}")

add_library(cvlib SHARED
    ${cvlib_sources}
    ${cvlib_headers}
    ${embededFiles}
)

target_link_libraries(cvlib ${cvlib_libraries})

set_target_properties(cvlib PROPERTIES FOLDER "libraries")

create_target_launcher(cvlib
    RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

target_include_directories(cvlib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
#add_dependencies(cvlib ${DLIB_TARGET})
