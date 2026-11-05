@echo off
rem Building demo.exe with NASM + ld (MinGW) only
rem coder : Ivandoe
if not exist build mkdir build
if not exist bin mkdir bin
nasm -f win32 -Ox -I src\ -I music\ src\intro.asm -o build\intro.obj || goto fin
set L=libs\imp32
ld -m i386pe -e _start --subsystem windows -s build\intro.obj src\res\icon.o %L%\kernel32.lib %L%\user32.lib %L%\gdi32.lib %L%\opengl32.lib %L%\winmm.lib -o bin\demo.exe || goto fin
echo OK : bin\demo.exe

where upx >nul 2>nul && upx --best --lzma -q bin\demo.exe
:fin
