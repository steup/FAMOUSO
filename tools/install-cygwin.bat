@echo off
IF "%1" == "" GOTO :ERROR
set packages=%1/packages
echo Install cygwin in %1 
setup.exe -P wget,make,subversion,mc,vim,gcc-g++ -q -L -D -R %1 -l %packages% -s http://ftp-stud.hs-esslingen.de/pub/Mirrors/sources.redhat.com/cygwin/ -O -X


GOTO :EXIT

:ERROR
echo.
echo You have to provide an install location.
echo For example: install-cygwin.bat e:\cygwin
echo.


:EXIT
echo on
