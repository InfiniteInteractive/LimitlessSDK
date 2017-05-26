if defined VisualStudioVersion echo %VisualStudioVersion% ELSE call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64

rem setup 3rd party libs
set QTDIR=C:/Qt/Qt5.6.2/5.6/msvc2015_64
rem set BOOST_DIR=D:/projects/3rdparty/boost_1_60_0
set FFMPEGDIR=C:/projects/3rdparty/ffmpeg_build
set DIRECTSHOW_DIR=C:/projects/3rdparty/directshow
set ICU_DIR=C:/projects/3rdparty/icu
set DECKLINK_DIR=C:/projects/3rdparty/Blackmagic DeckLink SDK 10.8.6/Win
set AJA_SDK_DIR=C:/projects/3rdparty/Aja/APIandSamples
set PYTHON_PATH=C:/Python/Python36
set FORGE_DIR=C:/projects/3rdparty/forge
set ADD_PACKAGE_DIR=C:/projects/cmakePackages
rem set HUNTER_PACKAGE_DIR=D:/projects/hunterPackages

set CMAKE_PREFIX_PATH=%QTDIR%/lib/cmake

rem set path=%path%;%QTDIR%/bin;%FFMPEGDIR%/bin/x64;%ICU_DIR%/bin64;

set cmake_call=cmake -H. -B_buildx64 -G "Visual Studio 14 2015 Win64"^
 -DADD_PACKAGE_DIR=%ADD_PACKAGE_DIR%^
 -DHUNTER_STATUS_DEBUG=ON^
 -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%^
 -DFFMPEG_ROOT=%FFMPEGDIR%^
 -DDECKLINK_ROOT_DIR="%DECKLINK_DIR%"^
 -DAJA_ROOT_DIR="%AJA_SDK_DIR%"^
 -DQT_ROOT_DIR=%QTDIR%^
 -DFORGE_ROOT_DIR=%FORGE_DIR%

echo %cmake_call%
call %cmake_call%