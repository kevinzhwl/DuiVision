
set RC_DIR=%WORKSPACE%\rc\
xcopy /C/Y/E "%WORKSPACE%\DuiVision\include\*.*" "%RC_DIR%\include\include\"
xcopy /C/Y/E "%WORKSPACE%\DuiVision\common\*.*" "%RC_DIR%\include\common\"
xcopy /C/Y/E "%WORKSPACE%\DuiVision\activex\*.*" "%RC_DIR%\include\activex\"
xcopy /C/Y/E "%WORKSPACE%\DuiVision\pugixml\*.*" "%RC_DIR%\include\pugixml\"

xcopy /C/Y/E "%WORKSPACE%\lib\*.*" "%RC_DIR%\lib\"