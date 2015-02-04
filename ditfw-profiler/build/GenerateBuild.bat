@echo off
goto menu

:menu
echo DigiPen Assignment Project Generator:
echo.
echo [1] Visual C++ 2013
echo.

:choice
set /P C="Choice: "
if "%C%"=="1" goto vs2013

:vs2013
"../tools/premake5" --file=premake/premake5-dit.lua vs2013
goto quit

:quit
pause
:end