#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(Sanctuary)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

set(Sanctuary_Sources
	main.cpp
	pluginattributes.cpp
	sanctuary.cpp
)

set(Sanctuary_Headers
	pluginattributes.h
	sanctuary.h
)

set(Sanctuary_Libraries
	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
	MediaPipeline.lib
	MediaQt.lib
)

QT5_WRAP_UI(Sanctuary_Uis
	pluginattributes.ui
	sanctuary.ui
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(Sanctuary WIN32
	${Sanctuary_Sources}
	${Sanctuary_Headers}
	${Sanctuary_Uis}
)

target_link_libraries(Sanctuary ${Sanctuary_Libraries} Qt5::WinMain)
qt5_use_modules(Sanctuary Core Gui Widgets Network)

set_target_properties(Sanctuary PROPERTIES 
	FOLDER "applications"
)

#SDK dependencies
add_dependencies(Sanctuary Base)
add_dependencies(Sanctuary Media)
add_dependencies(Sanctuary MediaPipeline)
add_dependencies(Sanctuary MediaQt)
add_dependencies(Sanctuary Utilities)

#filter dependencies
add_dependencies(Sanctuary AvStreamOutput)
add_dependencies(Sanctuary ControlStructures)
add_dependencies(Sanctuary ffmpeg)
add_dependencies(Sanctuary ImageViewer)

create_target_launcher(Sanctuary
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/debug"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
