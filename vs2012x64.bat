call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" amd64

set CURRDIR=%CD%

set QTDIR=C:\Qt\Qt5.2.0\5.2.0\msvc2012_64_opengl
set BOOST_DIR=C:\projects\3rdparty\boost_1_55_0
set ATISTREAMSDKROOT=C:\projects\3rdparty\AMD APP SDK\2.9
set FFMPEGDIR=C:\projects\IIM\LimitlessSDK\libraries\ffmpeg\bin\x64
set DIRECTSHOW_DIR=C:\projects\3rdparty\directshow
set BUILD_DIR=%CURRDIR%\build\VS2012\x64
set ICU_DIR=C:\projects\3rdparty\icu
set DECKLINK_DIR=C:/projects/3rdparty/Blackmagic DeckLink SDK 10.0/Win
set CMAKE_PREFIX_PATH=%QTDIR%/lib/cmake

mkdir %BUILD_DIR%
cd %BUILD_DIR%
set path=%QTDIR%/bin;%BOOST_DIR%\lib;%FFMPEGDIR%;%ICU_DIR%\bin64;%path%

echo %CD%
set cmake_call=cmake -G "Visual Studio 11 Win64" %CURRDIR%
echo %cmake_call%

cmake -G "Visual Studio 11 Win64" %CURRDIR%

set SOLUTION_FILE=%BUILD_DIR%\LimitlessSDK.sln

devenv "%SOLUTION_FILE%"