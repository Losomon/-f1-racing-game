@echo off
cd /d "%~dp0"
echo Generating IBL assets...
python ibl_baker.py assets/models/env/hdr_sunset.hdr
if %ERRORLEVEL% EQU 0 (
    echo IBL generation complete: brdfLUT.png, irradiance.cub, prefilter.cub in assets/shaders/textshaders/
) else (
    echo Generation failed!
)
pause

