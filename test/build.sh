#!/bin/sh
# 32-bit Linux test bench: ./build.sh then ./sim <start_frame> <end_frame> <step> > dump.bin
cd "$(dirname "$0")"
nasm -f elf32 $NASMX -Ox -I ../src/ -I ../music/ sim.asm -o /tmp/sim.o && ld -m elf_i386 /tmp/sim.o -o sim && echo "sim OK"
