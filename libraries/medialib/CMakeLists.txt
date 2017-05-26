project(medialib)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

include(AddPackage)

add_package("fftw" VERSION "3.3.4")

hunter_add_package(dlib)
find_package(dlib CONFIG REQUIRED)

#find_package(FFTW)
#find_package(dlib)

set(medialib_sources
    audioBuffer.cpp
    audioBufferWrapper.cpp
    audioChannels.cpp
    audioConversion.cpp
    audioMix.cpp
    audioOperations.cpp
    audioPeak.cpp
    simpleAudioBuffer.cpp
    spectrogram.cpp
    synchronize.cpp
)

set(medialib_headers
    medialib/audioBuffer.h
    medialib/audioBufferWrapper.h
    medialib/audioChannels.h
    medialib/audioConversion.h
    medialib/audioMix.h
    medialib/audioOperations.h
    medialib/audioPeak.h
    medialib/medialibDefine.h
    medialib/simpleAudioBuffer.h
    medialib/spectrogram.h
    medialib/synchronize.h
)

set(medialib_libraries
    ${FFTW_LIBRARIES}
#    optimized cvlib.lib
#    debug cvlibd.lib
    cvlib
    dlib::dlib
    )

message(STATUS "medialib_libraries: ${medialib_libraries}")

include_directories(${LIBRARIES_PATH}/cvlib)
include_directories(${FFTW_INCLUDE_DIRS})
#include_directories(${DLIB_INCLUDES})

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
#add_dependencies(medialib ${DLIB_TARGET})

set(medialib_BIN_DIRS ${FFTW_BIN_DIRS} ${DLIB_BIN_DIRS} CACHE INTERNAL "")