call "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" x64

echo %CD%

set CURRDIR=%CD%
set BUILD_DIR=%CURRDIR%\buildx64

mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo %CD%
set cmake_call=cmake -G "Visual Studio 9 2008 Win64" %CURRDIR%
echo %cmake_call%

cmake -G "Visual Studio 9 2008 Win64" %CURRDIR%

cd %CURRDIR%