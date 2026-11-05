#!/bin/sh
# shot.sh scene_start offset : simulates from the scene start and renders frame start+offset
s=$1; d=${2:-350}
./sim $((s-5)) $((s+d)) 100000000 > /tmp/sh_$s.bin
../../tools/render /tmp/sh_$s.bin /tmp/sh_$s 960 540 6.0 | tail -1
