@echo off
set path12345=%path%
rem change the path of the OS file here
rem this is for compatibility with notepad++ shortcuts
rem if you don't use them then change it to %cd%
set path=z:\nanoshell
rem ==================================================


cd %path%
set NSPath=%path%

echo.
echo.
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo Building kernel assembly code...
set path=%nspath%\tools\nasm
nasm -felf32 boot.asm -o build\boot.o
nasm -felf32 kernel.asm -o build\kernas.o
echo Done!

echo.
echo.
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

echo Building kernel C code...
set path=%nspath%\tools\i686-gcc\bin
i686-elf-gcc -DHDDEBUG -c kernel\kernel.c -o build\kernel.o -ffreestanding -O0 -Wall -Wextra -fno-exceptions -std=c99
echo Done!

echo.
echo.
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo Linking everything together...
i686-elf-gcc -T linker.ld -o nanoshell.bin -ffreestanding -O0 -nostdlib build\boot.o build\kernas.o build\kernel.o -lgcc
echo Done! Press any key...
pause>nul

rem insert your qemu location here
set qemupath=C:\Program Files\qemu
rem ==============================

c:
cd %qemupath%
set path=%NSPath%
qemu-system-i386.exe -kernel %path%\nanoshell.bin -m 32M
exit