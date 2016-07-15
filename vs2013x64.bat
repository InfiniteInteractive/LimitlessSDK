rem call "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" amd64

set CURRDIR=%CD%

set BUILD_DIR=%CURRDIR%\build\VS2013\x64

set QTDIR=D:/Qt/Qt5.4.0/5.4/msvc2013_64_opengl
set CMAKE_PREFIX_PATH=%QTDIR%/lib/cmake
set BOOST_DIR=D:\projects\3rdparty\boost_1_55_0_vs12
set ATISTREAMSDKROOT=D:\projects\3rdparty\AMD APP SDK\2.9
set FFMPEGDIR=D:\projects\IIM\LimitlessSDK\libraries\ffmpeg\bin\x64
set DIRECTSHOW_DIR=D:\projects\3rdparty\directshow
set ICU_DIR=D:\projects\3rdparty\icu
set DECKLINK_DIR=D:/projects/3rdparty/Blackmagic DeckLink SDK 10.0/Win
set AJA_SDK_DIR=D:/projects/3rdparty/Aja/ntv2
set PYTHON_PATH=D:\Python34
set GLM_ROOT_DIR=D:\projects\3rdparty\glm
set GLEW_ROOT_DIR=D:\projects\3rdparty\AMD APP SDK\2.9
set RAPIDJSON_ROOT_DIR=D:\projects\3rdparty\rapidjson\include

mkdir %BUILD_DIR%
cd %BUILD_DIR%

set path=%path%;%QTDIR%/bin;%BOOST_DIR%\lib;%FFMPEGDIR%;%ICU_DIR%\bin64;%ATISTREAMSDKROOT%\bin\x86_64

echo %CD%
set cmake_call=cmake -G "Visual Studio 12 2013 Win64" %CURRDIR%
echo %cmake_call%

cmake -G "Visual Studio 12 2013 Win64" %CURRDIR%

cd ../../..
rem set SOLUTION_FILE=%BUILD_DIR%\LimitlessSDK.sln
rem devenv "%SOLUTION_FILE%"