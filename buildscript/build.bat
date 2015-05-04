
set DEPLOY=DuiVisionDemo
set OUTPUT_DIR=%WORKSPACE%\out\%DEPLOY%
set DEPLOY_DIR=%WORKSPACE%\out\deploy
set PROJECT=DuiVisionDemo.2010.sln
set ZIPEXE=C:\PROGRA~1\7-Zip\7z.exe

set VC10VARS="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
set VC11VARS="C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"

@if exist %VC10VARS% (
	@call %VC10VARS% x86
	@goto runbuild
)

@if exist %VC11VARS% (
	@call %VC11VARS% x86
	@goto runbuild
)

:runbuild
msbuild.exe %WORKSPACE%\%PROJECT% /p:Configuration=Release /p:Platform=Win32 /t:Rebuild

@if "%1"=="WithDebugLib" (
	
msbuild.exe %WORKSPACE%\%PROJECT% /p:Configuration=Debug /p:Platform=Win32 /t:Rebuild

)

rmdir /S/Q %OUTPUT_DIR%
rmdir /S/Q %DEPLOY_DIR%
mkdir %OUTPUT_DIR%
mkdir %DEPLOY_DIR%
del /Q %WORKSPACE%\out\%DEPLOY%_%BUILD_ID%.zip 
echo clear cache finished

:copy %WORKSPACE%\build\deploy.bat %DEPLOY_DIR%

:复制生成的文件
xcopy /C/Y/E "%WORKSPACE%\bin\*.*" "%OUTPUT_DIR%\"
del /Q %OUTPUT_DIR%\*.pdb 
del /Q %OUTPUT_DIR%\*.exp 
del /Q %OUTPUT_DIR%\*.ilk 

%ZIPEXE% a -tzip %WORKSPACE%\out\%DEPLOY%_%BUILD_ID%.zip %OUTPUT_DIR%

echo build installer starting...

:::call %~dp0buildmsi.bat 32bit 64bit
call %~dp0deploy.bat
echo build installer finished