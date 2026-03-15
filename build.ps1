$ErrorActionPreference = "Stop"
$projDir = "c:/Users/solom/OneDrive/Desktop/ALL/F1 Racing/f1-game"
$gxx = "C:/Users/solom/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1/mingw64/bin/g++.exe"
$src = Join-Path $projDir "src/main.cpp"
$out = Join-Path $projDir "main.o"
$inc = "C:/Users/solom/Downloads/raylib-5.5_win64_mingw-w64/raylib-5.5_win64_mingw-w64/include"
$lib = "C:/Users/solom/Downloads/raylib-5.5_win64_mingw-w64/raylib-5.5_win64_mingw-w64/lib"

& $gxx -c $src -o $out -I$inc -std=c++17
if ($LASTEXITCODE -ne 0) { 
    Write-Host "Compilation failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE 
}

Write-Host "Compilation successful!"

& $gxx $out -o (Join-Path $projDir "f1_racing.exe") -L$lib -lraylib -lgdi32 -lwinmm -mconsole
if ($LASTEXITCODE -ne 0) { 
    Write-Host "Linking failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE 
}

Write-Host "Build complete!"
