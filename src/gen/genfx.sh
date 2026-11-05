#!/bin/sh
# generates src/postfx_src.inc (embedded shader) and src/fxtab.inc (effects of each scene) -- no Python
cd "$(dirname "$0")/../.."
awk 'BEGIN{print "fx_src:"} {printf " db \"%s\",10\n", $0} END{print " db 0"}' src/gen/postfx.frag > src/postfx_src.inc
# per scene: objects (warp,color) then background (warp,color); the background never has the same effects as the objects
#  object warp = scene%6 ; object color = (scene*7 + scene/30)%10 ; background: warp+3, color+5 (modulo)
#  the 3 title scenes (text to read) have soft hand-picked effects
N=${1:-33}
awk -v n=$N 'BEGIN{print "fx_tab:"; for(i=0;i<n;i++){w=i%6; c=(i*7+int(i/30))%10; bw=(w+3)%6; bc=(c+5)%10;
 if(i==0){w=0;c=1;bw=5;bc=9} if(i==1){w=0;c=6;bw=3;bc=0} if(i==2){w=0;c=0;bw=4;bc=1}
 printf " dd %d.0,%d.0,%d.0,%d.0\n", w, c, bw, bc}}' > src/fxtab.inc
