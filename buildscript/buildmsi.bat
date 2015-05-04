
set MSI_NAME=AnG_DesktopAdv.msi
set AI_PROJ=AnG_DesktopAdv.aip
set AIEXE="C:\Program Files (x86)\Caphyon\Advanced Installer 11.4.1\bin\x86\AdvancedInstaller.com"
set AIPROJ_DIR=%WORKSPACE%\installer

@if "%1"=="32bit" (
	
	%AIEXE% /rebuild %AIPROJ_DIR%\x86\%AI_PROJ%
	xcopy /E/C/Y "%AIPROJ_DIR%\x86\out\*.*" "%WORKSPACE%\out\installer\x86\"

)
@if "%2"=="64bit" (

	%AIEXE% /rebuild %AIPROJ_DIR%\x64\%AI_PROJ%
	xcopy /E/C/Y "%AIPROJ_DIR%\x64\out\*.*" "%WORKSPACE%\out\installer\x64\"

)
@goto end


@REM -----------------------------------------------------------------------
:end
