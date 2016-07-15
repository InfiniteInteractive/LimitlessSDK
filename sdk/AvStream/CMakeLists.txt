#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(AvStream)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(AvStream_Sources
	AvClientConnection.cpp
	AvStreamClient.cpp
	AvStreamPipe.cpp
	AvStreamServer.cpp
)

set(AvStream_Headers
	AvClientConnection.h
	AvPackets.h
	AvStreamClient.h
	AvStreamPipe.h
	AvStreamServer.h
	avstream_define.h
)

set(AvStream_Libraries
#	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_library(AvStream SHARED
	${AvStream_Sources}
	${AvStream_Headers}
	${AvStream_Qt_Headers_Moc}
	${AvStream_Qt_Forms_Headers} 
    ${AvStream_Qt_Resources_Rcc}
)

target_link_libraries(AvStream ${AvStream_Libraries})

set_target_properties(AvStream PROPERTIES 
	FOLDER "sdk"
)

add_dependencies(AvStream Base)
add_dependencies(AvStream Media)
add_dependencies(AvStream Utilities)

create_target_launcher(AvStream
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
