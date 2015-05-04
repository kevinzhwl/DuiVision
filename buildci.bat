
cd %~dp0
svn update
call buildscript\test-build.bat WithDebugLib
pause