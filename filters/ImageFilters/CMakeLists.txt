project(ImageFilters)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins/ImageFilters )
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG}/plugins/ImageFilters )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

message(STATUS "prefix:${CMAKE_PREFIX_PATH}")
find_package(PNG REQUIRED)

set(ImageFilters_Sources
	ColorConversion.cpp
	MoveToCpu.cpp
	MoveToGpu.cpp
	Overlay.cpp
)

set(ImageFilters_Headers
	ColorConversion.h
	MoveToCpu.h
	MoveToGpu.h
	Overlay.h
)

set(ImageFilters_Libraries
	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
	OpenCL.lib
	${PNG_LIBRARY}
	)


include(EmbedFile)
set(embedFiles
	kernels/colorConversion.cl
	kernels/overlay.cl
)

message(STATUS "ImageFilters embed: ${embedFiles}")
embed_files(embededFiles "${embedFiles}")

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${OPENGL_EX_INCLUDE_DIR})

include_directories(${OPENCL_INCLUDE_DIRS})
link_directories(${OPENCL_LIB_DIR})

include_directories(${PNG_INCLUDE_DIR})

add_library(ImageFilters SHARED
	${ImageFilters_Sources}
	${ImageFilters_Headers}
	${embededFiles}
)

target_link_libraries(ImageFilters ${ImageFilters_Libraries})
qt5_use_modules(ImageFilters Core Widgets)

set_target_properties(ImageFilters PROPERTIES 
	FOLDER "filters"
)

add_dependencies(ImageFilters Utilities)
add_dependencies(ImageFilters Base)
add_dependencies(ImageFilters Media)

create_target_launcher(ImageFilters
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
