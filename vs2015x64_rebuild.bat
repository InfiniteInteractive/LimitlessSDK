rem call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64

set CURRDIR=%CD%
set BUILD_DIR=%CURRDIR%\build\VS2015\x64
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

rem setup 3rd party libs
set QTDIR=D:/Qt/Qt5.6.0/5.6/msvc2015_64
set FFMPEGDIR=D:/projects/3rdparty/ffmpeg-3.0.1
set DIRECTSHOW_DIR=D:\projects\3rdparty\directshow
set ICU_DIR=D:\projects\3rdparty\icu
set DECKLINK_DIR=D:/projects/3rdparty/Blackmagic DeckLink SDK 10.0/Win
set AJA_SDK_DIR=D:/projects/3rdparty/Aja/APIandSamples
set PYTHON_PATH=D:\Python34

set ADD_PACKAGE_DIR=D:/projects/cmakePackages
set HUNTER_PACKAGE_DIR=D:/projects/hunterPackages

if not exist %HUNTER_PACKAGE_DIR% mkdir %HUNTER_PACKAGE_DIR%

set CMAKE_PREFIX_PATH=%QTDIR%/lib/cmake

set path=%path%;%QTDIR%/bin;%BOOST_DIR%/lib64-msvc-14.0;%FFMPEGDIR%/bin;%ICU_DIR%/bin64;

echo %CD%
set cmake_call=cmake -G "Visual Studio 14 2015 Win64"^
 -DADD_PACKAGE_DIR=%ADD_PACKAGE_DIR%^
 -DHUNTER_STATUS_DEBUG=ON^
 -DHUNTER_PACKAGE_DIR=%HUNTER_PACKAGE_DIR%^
 -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%^
 -DFFMPEG_ROOT=%FFMPEGDIR%^
 %CURRDIR% 

rem -DBOOST_ROOT=%BOOST_DIR%^
 
echo %cmake_call%

call %cmake_call%

cd ../../..