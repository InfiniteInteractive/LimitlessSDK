project(medialib)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

find_package(FFTW)
find_package(dlib)

set(medialib_sources
	audioBuffer.cpp
	audioBufferWrapper.cpp
	audioConversion.cpp
	audioOperations.cpp
	simpleAudioBuffer.cpp
	spectrogram.cpp
	synchronize.cpp
)

set(medialib_headers
	medialib/audioBuffer.h
	medialib/audioBufferWrapper.h
	medialib/audioConversion.h
	medialib/audioOperations.h
	medialib/medialibDefine.h
	medialib/simpleAudioBuffer.h
	medialib/spectrogram.h
	medialib/synchronize.h
)

set(medialib_libraries
	${FFTW_LIBRARIES}
	${DLIB_LIBRARIES}
	cvlib.lib
	)

include_directories(${LIBRARIES_PATH}/cvlib)
include_directories(${FFTW_INCLUDES})
include_directories(${DLIB_INCLUDES})

message(STATUS "Lib output path:${LIBRARY_OUTPUT_PATH}")
link_directories(${LIBRARY_OUTPUT_PATH})

add_library(medialib SHARED
	${medialib_sources}
	${medialib_headers}
)

target_link_libraries(medialib ${medialib_libraries})
set_target_properties(medialib PROPERTIES FOLDER "libraries")

create_target_launcher(medialib
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

add_dependencies(medialib cvlib)