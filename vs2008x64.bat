call "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" x64

set CURRDIR=%CD%
set BUILD_DIR=%CURRDIR%\buildx64

mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo %CD%
set cmake_call=cmake -G "Visual Studio 9 2008 Win64" %CURRDIR%
echo %cmake_call%

cmake -G "Visual Studio 9 2008 Win64" %CURRDIR%

set SOLUTION_FILE=%BUILD_DIR%\LimitlessSDK.sln

devenv "%SOLUTION_FILE%"