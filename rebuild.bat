call "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat"

echo %CD%

set CURRDIR=%CD%
set BUILD_DIR=%CURRDIR%\build

mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo %CD%
set cmake_call=cmake -G "Visual Studio 9 2008" %CURRDIR%
echo %cmake_call%

cmake -G "Visual Studio 9 2008" %CURRDIR%

cd %CURRDIR%