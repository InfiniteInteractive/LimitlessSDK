cmake_minimum_required(VERSION 3.0.0)

project(ImageSource)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#hunter_add_package(Boost COMPONENTS system filesystem)
#find_package(Boost CONFIG REQUIRED system filesystem)

hunter_add_package(PNG)
find_package(PNG CONFIG REQUIRED)

#remove min and max macro declares from windows builds, causes issues with std::min/max
add_definitions(-DNOMINMAX)

set(ImageSourceSources
    imageSource.cpp
    ImageSourceAttributeView.cpp
)

set(ImageSourceHeaders
    imageSource.h
    ImageSourceAttributeView.h
)

set(ImageSourceLibraries
    Utilities.lib
    Base.lib
    Media.lib
    Boost::system
    Boost::filesystem
    PNG::png
)

QT5_WRAP_UI(ImageSourceUis
    ImageSourceAttributeView.ui
)

include_directories(${SDK_INCLUDES})
link_directories(${LIBRARY_OUTPUT_PATH})

add_library(ImageSource SHARED
	${ImageSourceSources}
	${ImageSourceHeaders}
	${ImageSourceUis}
)

target_link_libraries(ImageSource ${ImageSourceLibraries})
qt5_use_modules(ImageSource Core Gui Widgets)

set_target_properties(ImageSource PROPERTIES 
	FOLDER "filters"
)

add_dependencies(ImageSource Utilities)
add_dependencies(ImageSource Base)
add_dependencies(ImageSource Media)

create_target_launcher(ImageSource
	RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
