rem call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64

set CURRDIR=%CD%
set BUILD_DIR=%CURRDIR%\build\VS2015\x64
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

set QTDIR=D:/Qt/Qt5.6.0/5.6/msvc2015_64
set OPENGL_EX_DIR=D:/projects/3rdparty/opengl
set GLEW_ROOT_DIR=D:/projects/3rdparty/glew-1.13.0
set BOOST_DIR=D:/projects/3rdparty/boost_1_59_0
set ATISTREAMSDKROOT=D:\projects\3rdparty\AMD APP SDK\2.9
set FFMPEGDIR=D:/projects/3rdparty/ffmpeg-3.0.1
set DIRECTSHOW_DIR=D:\projects\3rdparty\directshow
set ICU_DIR=D:\projects\3rdparty\icu
set DECKLINK_DIR=D:/projects/3rdparty/Blackmagic DeckLink SDK 10.0/Win
set AJA_SDK_DIR=D:/projects/3rdparty/Aja/ntv2
set PYTHON_PATH=D:\Python34
set GLM_ROOT_DIR=D:\projects\3rdparty\glm
rem set GLEW_ROOT_DIR=D:\projects\3rdparty\AMD APP SDK\2.9
set RAPIDJSON_ROOT_DIR=D:\projects\3rdparty\rapidjson\include
set FFTW_ROOT=D:/projects/3rdparty/fftw-3.3.4
set DLIB_ROOT=D:/projects/3rdparty/dlib

set CMAKE_PREFIX_PATH=%QTDIR%/lib/cmake;%GLEW_ROOT_DIR%;%BOOST_DIR%

set path=%path%;%QTDIR%/bin;%BOOST_DIR%/lib64-msvc-14.0;%FFMPEGDIR%;%ICU_DIR%\bin64;%ATISTREAMSDKROOT%\bin\x86_64;;%GLEW_ROOT_DIR%/bin/x64

echo %CD%
set cmake_call=cmake -G "Visual Studio 14 2015 Win64" -DGLEW_ROOT_DIR=%GLEW_ROOT_DIR% -DFFMPEG_ROOT=%FFMPEGDIR% -DFFTW_ROOT=%FFTW_ROOT% -DDLIB_ROOT=%DLIB_ROOT% -DOPENGL_EX_INCLUDE_DIR=%OPENGL_EX_DIR% %CURRDIR% 
echo %cmake_call%

call %cmake_call%

cd ../../..