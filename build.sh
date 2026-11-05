#!/bin/sh
# builds the Windows exe from Linux (nasm + ld i386pe) -- no Python
set -e
cd "$(dirname "$0")"
mkdir -p build bin
nasm -f win32 -Ox -I src/ -I music/ src/intro.asm -o build/intro.obj

L=libs/imp32
ld -m i386pe -e _start --subsystem windows -s build/intro.obj src/res/icon.o $L/kernel32.lib $L/user32.lib $L/gdi32.lib $L/opengl32.lib $L/winmm.lib -o bin/demo.exe
ls -la bin/demo.exe
# optional exe compression with UPX
if command -v upx >/dev/null 2>&1; then upx --best --lzma -q bin/demo.exe; ls -la bin/demo.exe; fi
