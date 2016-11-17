#cmake_minimum_required(VERSION 2.8.9)
#if(POLICY CMP0020)
#	cmake_policy(SET CMP0020 NEW)
#endif()

project(QtComponents)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

hunter_add_package(Boost COMPONENTS system filesystem)
find_package(Boost CONFIG REQUIRED system filesystem)

set(QtComponents_Sources
	customStyleSheet.cpp
	dialogWrapper.cpp
	dockWrapper.cpp
	filterpopup.cpp
	listpopup.cpp
	mediaStoreView.cpp
	qComboBoxHelper.cpp
	qSettingsHelper.cpp
	SerializableObject.cpp
	vuMeter.cpp
)

set(QtComponents_Headers
	customStyleSheet.h
	dialogWrapper.h
	dockWrapper.h
	filterpopup.h
	listpopup.h
	mediaStoreView.h
	qComboBoxHelper.h
	qSettingsHelper.h
	qtcomponents_define.h
	QtPluginView.h
	SerializableObject.h
	vuMeter.h
)

set(QtComponents_Libraries
	Utilities.lib
	Base.lib
	Media.lib
    Boost::system
    Boost::filesystem
)

QT5_WRAP_UI(QtComponents_Uis
	filterpopup.ui
	listpopup.ui
	mediaStoreView.ui
)

qt5_add_resources(QtComponents_Resources 
	qtcomponents.qrc
)

include(EmbedFile)
set(embedFiles
	darkorange.stylesheet
)

embed_files(embededFiles ${embedFiles} "Limitless")

message(STATUS "Embed")
message(STATUS "${embededFiles}")

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

add_library(QtComponents SHARED
	${QtComponents_Sources}
	${QtComponents_Headers}
	${QtComponents_Uis}
	${QtComponents_Resources}
	${embededFiles}
)

target_link_libraries(QtComponents ${QtComponents_Libraries})
qt5_use_modules(QtComponents Core Gui Widgets)

set_target_properties(QtComponents PROPERTIES 
	FOLDER "sdk"
)

add_dependencies(QtComponents Base)
add_dependencies(QtComponents Media)
add_dependencies(QtComponents Utilities)
add_dependencies(QtComponents Embed)

create_target_launcher(QtComponents
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
