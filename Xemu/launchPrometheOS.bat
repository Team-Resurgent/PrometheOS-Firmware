@echo off

cd %~dp0

IF NOT EXIST ".\roms\mcpx_1.0.bin" (
    echo mcpx_1.0.bin does not exist in roms folder
    pause
    exit /b 1
)

xemu.exe -device modchip-xenium,rom-path=..\Build\prometheos.bin
