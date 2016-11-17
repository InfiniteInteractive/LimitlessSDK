project(cvlib)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

#include(AddPackage)

#add_package("dlib" VERSION "19.0")

hunter_add_package(dlib)
find_package(dlib CONFIG REQUIRED)

#find_package(FFTW)
#find_package(dlib)

set(cvlib_sources
	match.cpp
	ppm.cpp
	simpleImage.cpp
)

set(cvlib_headers
	cvlib/cvlibDefine.h
	cvlib/image.h
	cvlib/imageDLib.h
	cvlib/match.h
	cvlib/ppm.h
	cvlib/simpleImage.h
	cvlib/slidingWindow.h
)

set(cvlib_libraries
#	${FFTW_LIBRARIES}
#	${DLIB_LIBRARIES}
	dlib::dlib
	)

add_library(cvlib SHARED
	${cvlib_sources}
	${cvlib_headers}
)

target_link_libraries(cvlib ${cvlib_libraries})
#include_directories(${FFTW_INCLUDES})
#include_directories(${DLIB_INCLUDES})

set_target_properties(cvlib PROPERTIES FOLDER "libraries")

create_target_launcher(cvlib
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

#add_dependencies(cvlib ${DLIB_TARGET})
