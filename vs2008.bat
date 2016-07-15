call "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat"

set CURRDIR=%CD%
set BUILD_DIR=%CURRDIR%\build

mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo %CD%
set cmake_call=cmake -G "Visual Studio 9 2008" %CURRDIR%
echo %cmake_call%

cmake -G "Visual Studio 9 2008" %CURRDIR%

set SOLUTION_FILE=%BUILD_DIR%\LimitlessSDK.sln

devenv "%SOLUTION_FILE%"