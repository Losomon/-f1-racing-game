@echo off
set RAYLIB_PATH=C:/Users/solom/Downloads/raylib-5.5_win64_mingw-w64/raylib-5.5_win64_mingw-w64
set OUT_PATH=c:/Users/solom/OneDrive/Desktop/ALL/F1 Racing/f1-game/out
set SRC_PATH=c:/Users/solom/OneDrive/Desktop/ALL/F1 Racing/f1-game/src

echo Compiling F1 Racing Phase 2...

REM Create output directory if not exists
if not exist "%OUT_PATH%" mkdir "%OUT_PATH%"

REM Compile all source files&#10;"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/input/InputManager.cpp" -o "%OUT_PATH%/InputManager.o" -I"%RAYLIB_PATH%/include" -std=c++17&#10;"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/audio/AudioManager.cpp" -o "%OUT_PATH%/AudioManager.o" -I"%RAYLIB_PATH%/include" -std=c++17&#10;"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/steam/SteamManager.cpp" -o "%OUT_PATH%/SteamManager.o" -I"%RAYLIB_PATH%/include" -I f1-game/lib/steam/public -std=c++17&#10;REM Existing compiles...
"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/core/Engine.cpp" -o "%OUT_PATH%/Engine.o" -I"%RAYLIB_PATH%/include" -std=c++17
"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/physics/Suspension.cpp" -o "%OUT_PATH%/Suspension.o" -I"%RAYLIB_PATH%/include" -std=c++17
"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/physics/VehicleBody.cpp" -o "%OUT_PATH%/VehicleBody.o" -I"%RAYLIB_PATH%/include" -std=c++17
"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/game/Track.cpp" -o "%OUT_PATH%/Track.o" -I"%RAYLIB_PATH%/include" -std=c++17
"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/game/Car.cpp" -o "%OUT_PATH%/Car.o" -I"%RAYLIB_PATH%/include" -std=c++17
"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/game/RaceManager.cpp" -o "%OUT_PATH%/RaceManager.o" -I"%RAYLIB_PATH%/include" -std=c++17
"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/render/Renderer.cpp" -o "%OUT_PATH%/Renderer.o" -I"%RAYLIB_PATH%/include" -std=c++17
"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" -c "%SRC_PATH%/main.cpp" -o "%OUT_PATH%/main.o" -I"%RAYLIB_PATH%/include" -std=c++17

REM Link everything
"C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe" "%OUT_PATH%/*.o" -o "%OUT_PATH%/f1_racing.exe" -L"%RAYLIB_PATH%/lib" -lraylib -lgdi32 -lwinmm -mconsole -std=c++17

echo.
echo Build complete!
echo Run: %OUT_PATH%\f1_racing.exe
pause
</parameter>
</create_file>
