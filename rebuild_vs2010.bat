call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat"

set CURRDIR=%CD%
set BUILD_DIR=%CURRDIR%\build\VS2010\x86

mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo %CD%
set cmake_call=cmake -G "Visual Studio 10" %CURRDIR%
echo %cmake_call%

cmake -G "Visual Studio 10" %CURRDIR%

set SOLUTION_FILE=%BUILD_DIR%\LimitlessSDK.sln

cd %CURRDIR%