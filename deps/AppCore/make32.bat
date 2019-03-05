@echo off
SETLOCAL
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64_x86
if "%1"=="clean" GOTO CLEAN
if "%1"=="build" GOTO BUILD
if not exist Build32 mkdir Build32
cd Build32
set CC=cl.exe
set CXX=cl.exe
echo(
echo  /````````````````````````````````````````````````````````````\
echo  ^|                                                            ^|
echo  ^|  Generator: Visual Studio 2015 (32-bit)                    ^|
echo  ^|                                                            ^|
echo  \____________________________________________________________/
echo(
cmake -G "Visual Studio 14 2015" ..
cd ..
GOTO DONE
:CLEAN
del /f /s /q Build32 >nul 2>&1
rmdir /s /q Build32 >nul 2>&1
echo Working directory clean.
GOTO DONE
:DONE