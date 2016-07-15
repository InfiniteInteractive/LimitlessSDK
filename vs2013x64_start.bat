call "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" amd64

call vs2013x64.bat

set SOLUTION_FILE=%BUILD_DIR%\LimitlessSDK.sln
rem cd ../../..
devenv "%SOLUTION_FILE%"