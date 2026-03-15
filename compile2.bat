@echo off
set "GXX=C:\Users\solom\Downloads\winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1\mingw64\bin\g++.exe"
set "SRC=c:\Users\solom\OneDrive\Desktop\ALL\F1 Racing\f1-game\src\main.cpp"
set "OUT=c:\Users\solom\OneDrive\Desktop\ALL\F1 Racing\f1-game\main.o"
set "INC=C:\Users\solom\Downloads\raylib-5.5_win64_mingw-w64\raylib-5.5_win64_mingw-w64\include"
"%GXX%" -c "%SRC%" -o "%OUT%" -I"%INC%" -std=c++17
if errorlevel 1 pause
