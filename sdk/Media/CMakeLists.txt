#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(Media)

include(AddPackage)

add_package("opencl")
add_package("glew" VERSION "2.0.0")

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(Media_Sources
	AudioSample.cpp
	BufferSample.cpp
	EventSample.cpp
	GPUBufferSample.cpp
#	GPUBufferGLSample.cpp
	GPUContext.cpp
#	GPUImageGLSample.cpp
#	GpuImageGLSampleSet.cpp
	GPUImageSample.cpp
	GpuImageSampleSet.cpp
	IAudioSample.cpp
	IFileSource.cpp
	IImageSample.cpp
	IImageSource.cpp
	ImageSample.cpp
	ImageSampleSet.cpp
	IMedia.cpp
	IMediaFilter.cpp
	IMediaSource.cpp
	MediaClock.cpp
	MediaFactory.cpp
	MediaFilterContainer.cpp
	MediaFormat.cpp
	MediaInterface.cpp
	MediaPad.cpp
	MediaPluginFactory.cpp
	MediaSample.cpp
	MediaSampleFactory.cpp
	MediaSampleFlags.cpp
	MediaSinkPad.cpp
	MediaSourcePad.cpp
	MediaStore.cpp
	OpenClTask.cpp
	OpenClThread.cpp
	SystemMediaClock.cpp
)

set(Media_Headers
	AudioSample.h
	BufferSample.h
	EventSample.h
	GPUBufferSample.h
#	GPUBufferGLSample.h
	GPUContext.h
#	GPUImageGLSample.h
#	GpuImageGLSampleSet.h
	GPUImageSample.h
	GpuImageSampleSet.h
	IAudioSample.h
	IFileSource.h
	IImageSample.h
	IImageSampleSet.h
	IImageSource.h
	ImageSample.h
	ImageSampleSet.h
	IMedia.h
	IMediaFilter.h
	IMediaSource.h
	MediaClock.h
	MediaContainer.h
	MediaFactory.h
	MediaFilterContainer.h
	MediaFormat.h
	MediaInterface.h
	MediaPad.h
	MediaPluginFactory.h
	MediaSample.h
	MediaSampleFactory.h
	MediaSampleFlags.h
	MediaSinkPad.h
	MediaSourcePad.h
	MediaStore.h
	media_define.h
	OpenClTask.h
	OpenClThread.h
	SystemMediaClock.h
)

set(Media_Libraries
	${Boost_LIBRARIES}
	${OPENCL_LIBRARIES}
	Utilities.lib
	Base.lib
	${OPENGL_gl_LIBRARY}
	${GLEW_LIBRARIES}
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

message(STATUS "GLEW include: ${GLEW_INCLUDE_DIRS}")
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${OPENGL_EX_INCLUDE_DIR})

message(STATUS "OpenCl include: ${OPENCL_INCLUDE_DIRS}")
include_directories(${OPENCL_INCLUDE_DIRS})
#link_directories(${OPENCL_LIB_DIR})

add_definitions(-DRAPIDJSON_HAS_STDSTRING)
include_directories(${RAPIDJSON_INCLUDE_DIRS})

add_library(Media SHARED
	${Media_Sources}
	${Media_Headers}
#	${Media_Qt_Headers_Moc}
#	${Media_Qt_Forms_Headers} 
#    ${Media_Qt_Resources_Rcc}
)

target_link_libraries(Media ${Media_Libraries})
#qt5_use_modules(Media Core)

#disable dll export warnings
add_definitions("/wd4251")

set_target_properties(Media PROPERTIES 
	FOLDER "sdk"
)

add_dependencies(Media Base)
add_dependencies(Media Utilities)
add_dependencies(Media ${OPENCL_TARGET})

create_target_launcher(Media
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
