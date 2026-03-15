@echo off
setlocal

set "RAYLIB_PATH=C:/Users/solom/Downloads/raylib-5.5_win64_mingw-w64/raylib-5.5_win64_mingw-w64"
set "OUT_PATH=c:/Users/solom/OneDrive/Desktop/ALL/F1 Racing/f1-game/out"
set "SRC_PATH=c:/Users/solom/OneDrive/Desktop/ALL/F1 Racing/f1-game/src"
set "CXX=C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe"

echo Compiling F1 Racing...

REM Create output directory if not exists
if not exist "%OUT_PATH%" mkdir "%OUT_PATH%"

REM Compile all source files
"%CXX%" -c "%SRC_PATH%/steam/SteamManager.cpp"   -o "%OUT_PATH%/SteamManager.o"   -I"%RAYLIB_PATH%/include" -std=c++17
"%CXX%" -c "%SRC_PATH%/core/Engine.cpp"          -o "%OUT_PATH%/Engine.o"         -I"%RAYLIB_PATH%/include" -std=c++17
"%CXX%" -c "%SRC_PATH%/physics/VehicleBody.cpp"  -o "%OUT_PATH%/VehicleBody.o"    -I"%RAYLIB_PATH%/include" -std=c++17
"%CXX%" -c "%SRC_PATH%/game/Track.cpp"           -o "%OUT_PATH%/Track.o"          -I"%RAYLIB_PATH%/include" -std=c++17
"%CXX%" -c "%SRC_PATH%/game/Car.cpp"             -o "%OUT_PATH%/Car.o"            -I"%RAYLIB_PATH%/include" -std=c++17
"%CXX%" -c "%SRC_PATH%/game/RaceManager.cpp"     -o "%OUT_PATH%/RaceManager.o"    -I"%RAYLIB_PATH%/include" -std=c++17
"%CXX%" -c "%SRC_PATH%/render/Renderer.cpp"      -o "%OUT_PATH%/Renderer.o"       -I"%RAYLIB_PATH%/include" -std=c++17
"%CXX%" -c "%SRC_PATH%/render/ShaderManager.cpp" -o "%OUT_PATH%/ShaderManager.o"  -I"%RAYLIB_PATH%/include" -std=c++17
"%CXX%" -c "%SRC_PATH%/main.cpp"                 -o "%OUT_PATH%/main.o"           -I"%RAYLIB_PATH%/include" -std=c++17

IF ERRORLEVEL 1 (
    echo Compilation failed.
    pause
    exit /b 1
)

REM Link everything
"%CXX%" "%OUT_PATH%\*.o" -o "%OUT_PATH%/f1_racing.exe" -L"%RAYLIB_PATH%/lib" -lraylib -lgdi32 -lwinmm -mconsole -std=c++17

IF ERRORLEVEL 1 (
    echo Linking failed.
    pause
    exit /b 1
)

echo.
echo Build complete!
echo Run: "%OUT_PATH%\f1_racing.exe"
pause

