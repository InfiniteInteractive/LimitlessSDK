#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(DebugApp)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(DebugApp_Sources
	debugapp.cpp
	main.cpp
)

set(DebugApp_Headers
	debugapp.h
)

set(DebugApp_Libraries
	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
	MediaPipeline.lib
	QtComponents.lib
	opengl32.lib
	glew64.lib
	${OPENCL_LIBRARIES}
)

QT5_WRAP_UI(DebugApp_Uis
	debugapp.ui
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${OPENCL_INCLUDE_DIRS})
link_directories(${OPENCL_LIB_DIR})

add_executable(DebugApp WIN32
	${DebugApp_Sources}
	${DebugApp_Headers}
	${DebugApp_Uis}
)

target_link_libraries(DebugApp ${DebugApp_Libraries} Qt5::WinMain)
qt5_use_modules(DebugApp Core Gui Widgets)

set_target_properties(DebugApp PROPERTIES 
	FOLDER "applications"
)

#SDK dependencies
add_dependencies(DebugApp Base)
add_dependencies(DebugApp Media)
add_dependencies(DebugApp MediaPipeline)
add_dependencies(DebugApp MediaQt)
add_dependencies(DebugApp QtComponents)
add_dependencies(DebugApp Utilities)
add_dependencies(DebugApp AvStream)

#filter dependencies
add_dependencies(DebugApp AvStreamOutput)
add_dependencies(DebugApp ControlStructures)
add_dependencies(DebugApp GPUStitcher)
add_dependencies(DebugApp ImageViewer)

create_target_launcher(DebugApp
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/debug"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
