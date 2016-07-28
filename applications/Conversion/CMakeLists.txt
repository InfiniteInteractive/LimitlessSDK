#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(Conversion)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(Conversion_Sources
	conversion.cpp
	main.cpp
)

set(Conversion_Headers
	conversion.h
)

set(Conversion_Libraries
	${Boost_LIBRARIES}
	Utilities.lib
	Base.lib
	Media.lib
	MediaPipeline.lib
	QtComponents.lib
)

QT5_WRAP_UI(Conversion_Uis
	Conversion.ui
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(Conversion WIN32
	${Conversion_Sources}
	${Conversion_Headers}
	${Conversion_Uis}
)

target_link_libraries(Conversion ${Conversion_Libraries} Qt5::WinMain)
qt5_use_modules(Conversion Core Gui Widgets)

set_target_properties(Conversion PROPERTIES 
	FOLDER "applications"
)

#SDK dependencies
add_dependencies(Conversion Base)
add_dependencies(Conversion Media)
add_dependencies(Conversion MediaPipeline)
add_dependencies(Conversion MediaQt)
add_dependencies(Conversion QtComponents)
add_dependencies(Conversion Utilities)
add_dependencies(Conversion AvStream)

#filter dependencies
add_dependencies(Conversion AvStreamOutput)
add_dependencies(Conversion ControlStructures)
add_dependencies(Conversion ImageViewer)

create_target_launcher(Conversion
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/debug"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
