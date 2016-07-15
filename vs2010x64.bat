call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" x64

set CURRDIR=%CD%

set QTDIR=C:/Qt/Qt5.1.1/5.1.1/msvc2010
set DIRECTSHOW_DIR=C:\projects\3rdparty\directshow
set BOOST_DIR=C:\projects\3rdparty\boost_1_55_0
set ATISTREAMSDKROOT=C:\projects\3rdparty\AMD APP SDK\2.9
set FFMPEGDIR=C:\projects\IIM\LimitlessSDK\libraries\ffmpeg\bin\x64
set BUILD_DIR=%CURRDIR%\build\VS2010\x64
set ICU_DIR=C:\projects\3rdparty\icu
set CMAKE_PREFIX_PATH=%QTDIR%/lib/cmake

mkdir %BUILD_DIR%
cd %BUILD_DIR%
set path=%QTDIR%/bin;%BOOST_DIR%\lib;%FFMPEGDIR%;%ICU_DIR%\bin64;%path%

echo %CD%
set cmake_call=cmake -G "Visual Studio 10 Win64" %CURRDIR%
echo %cmake_call%

cmake -G "Visual Studio 10 Win64" %CURRDIR%

set SOLUTION_FILE=%BUILD_DIR%\LimitlessSDK.sln

devenv "%SOLUTION_FILE%"