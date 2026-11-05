#!/bin/sh
# generates the scene table (song orders x backgrounds x objects x cameras)
O="0 4 8 12 16 20 24 28 32 34 36 38 40 42 44 45 46 47 48 49 50 51 53 55 57 59 61 63 64 66 68 70 72 74 76 78 80 82 83 85 87 89 91"
BGS="fjord alpine dune ocean canyon volcano ice terrace karst"
PALBG="PAL_FJORD PAL_ALPINE PAL_DUNE PAL_OCEAN PAL_CANYON PAL_VOLCANO PAL_STEEL PAL_JADE PAL_ALPINE"
WAMP="0.06 0.06 0.5 0.9 0.2 0.2 0.1 0.2 0.1"
OBJ="KNOT THOMAS LORENZ HYPER GAL AIZAWA HELI VORTEX DIPOLE HALV SIERP MENGER MBULB JULIA APOL"
SIZ="2.2 1.1 1.1 1.8 1.5 1.2 1.6 1.8 1.3 1.1 1.3 1.4 1.4 1.4 1.4"
DST="7.0 7.0 7.5 7.0 8.0 7.0 7.0 7.0 7.0 7.0 7.0 7.0 7.0 7.0 7.0"
i=0
for o in $O; do
  b=$(( (i*4+1) % 9 )); f=$(( (i*7+3) % 15 ))
  bg=$(echo $BGS | cut -d' ' -f$((b+1))); pb=$(echo $PALBG | cut -d' ' -f$((b+1))); wa=$(echo $WAMP | cut -d' ' -f$((b+1)))
  ob=$(echo $OBJ | cut -d' ' -f$((f+1))); sz=$(echo $SIZ | cut -d' ' -f$((f+1))); ds=$(echo $DST | cut -d' ' -f$((f+1)))
  case $ob in
   KNOT) pl="PAL_GOLD PAL_EMBERS";; THOMAS) pl="PAL_COPPER PAL_GALAX PAL_STEEL";; LORENZ) pl="PAL_EMBERS PAL_COPPER";;
   HYPER) pl="PAL_JADE PAL_STEEL";; GAL) pl="PAL_GALAX";; AIZAWA) pl="PAL_GALAX PAL_DAWN";;
   HELI) pl="PAL_JADE PAL_COPPER PAL_DAWN";; VORTEX) pl="PAL_DAWN PAL_STEEL";; DIPOLE) pl="PAL_GALAX PAL_STEEL";; HALV) pl="PAL_STEEL PAL_EMBERS";; SIERP) pl="PAL_EMBERS PAL_COPPER PAL_STEEL";; MENGER) pl="PAL_COPPER PAL_GOLD";; MBULB) pl="PAL_COPPER PAL_DAWN PAL_JADE";; JULIA) pl="PAL_GALAX PAL_STEEL";; APOL) pl="PAL_EMBERS PAL_GALAX PAL_JADE";;
  esac
  np=$(echo $pl | wc -w); pf=$(echo $pl | cut -d' ' -f$(( i % np + 1 )))
  case $((i % 4)) in 0) rx=-4.0; ry=-10.0; ra=8.0;; 1) rx=-2.5; ry=6.0; ra=-7.0;; 2) rx=-5.0; ry=0.0; ra=9.0;; 3) rx=-1.5; ry=12.0; ra=-6.0;; esac
  echo "; ---- scene $i : order $o : $bg + $ob"
  echo " SCN $o,$rx,$ry,$ra,$ds,$sz"
  echo " BGL $bg,$pb,3.0,$((i*7+11)),$wa"
  echo " F_$ob $pf,$((i*5+3))"
  i=$((i+1))
done
