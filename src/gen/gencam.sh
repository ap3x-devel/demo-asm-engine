#!/bin/sh
# generates src/camtab.inc: camera profile of each scene (33 rows of 12 floats) -- no Python
# columns: elevation(deg) heading(deg) distance_scale jump_min jump_range direction_mode elev_amplitude dolly_punch glide_rate slow_push 0 0
cd "$(dirname "$0")/../.."
N=${1:-33}
awk -v n=$N 'function fr(x){return x-int(x)}
BEGIN{print "cam_prof:";
 split("0 1 -1 2",mo," ");
 for(i=0;i<n;i++){
  el=10+48*fr(i*0.6180339+0.13);
  ya=360*fr(i*0.381966)-180;
  ds=0.9+0.55*fr(i*0.7548777+0.3);
  m=mo[(i*3+1)%4+1];
  jm=25+40*fr(i*0.4142+0.2);
  jr=20+60*fr(i*0.7321);
  ea=4+14*fr(i*0.5774);
  dl=0.04+0.16*fr(i*0.236+0.5);
  rt=3+11*fr(i*0.6823+0.1);
  pu=-0.10+0.28*fr(i*0.9143+0.4);
  printf " dd %.2f,%.2f,%.3f,%.1f,%.1f,%.1f,%.2f,%.3f,%.2f,%.3f,0.0,0.0\n",el,ya,ds,jm,jr,m,ea,dl,rt,pu}}' > src/camtab.inc
