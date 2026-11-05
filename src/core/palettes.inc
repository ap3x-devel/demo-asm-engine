; ============================================================
;  palettes.inc - natural gradients with 5 stops (0, 25, 50, 75, 100 %)
;  No neon, no synthwave: rock, water, ice, fire, auroras, skies.
; ============================================================
%define PAL_FJORD   0
%define PAL_GOLD    1
%define PAL_ALPINE   2
%define PAL_DUNE    3
%define PAL_OCEAN   4
%define PAL_CANYON  5
%define PAL_VOLCANO  6
%define PAL_GALAX   7
%define PAL_JADE    8
%define PAL_COPPER  9
%define PAL_STEEL   10
%define PAL_DAWN    11
%define PAL_EMBERS  12
section .data
align 16
gp_tab:
 PAL5   22,32,53,62,83,100,110,136,143,198,212,217,255,255,252    ; 0 fjord: dark rock -> snow
 PAL5   68,34,13,160,93,25,239,168,53,255,219,134,255,249,219    ; 1 gold / amber
 PAL5   53,65,68,93,114,99,160,164,160,212,216,219,252,252,255    ; 2 alpine: forest -> rock -> snow
 PAL5   104,68,44,184,129,81,229,184,124,246,219,168,255,246,219    ; 3 dunes: sand
 PAL5   19,42,73,34,93,124,53,151,168,151,209,212,242,252,252    ; 4 ocean: abyss -> foam
 PAL5   93,53,38,160,93,60,209,134,81,233,184,124,249,229,191    ; 5 canyon: red strata
 PAL5   44,38,38,104,87,81,176,68,34,246,143,44,255,226,134    ; 6 volcano: ash -> lava
 PAL5   34,34,81,81,104,176,219,219,239,255,233,191,255,198,134    ; 7 galaxy: indigo -> warm white -> amber
 PAL5   22,68,53,53,143,114,124,198,160,219,239,212,252,252,239    ; 8 jade
 PAL5   81,53,34,184,124,81,229,176,134,246,219,191,255,249,233    ; 9 copper
 PAL5   44,53,68,104,124,151,168,184,198,219,229,236,252,253,255    ; 10 steel / ice
 PAL5   81,44,60,184,93,104,239,160,143,252,212,184,255,246,233    ; 11 pink dawn
 PAL5   68,22,22,176,53,44,242,134,60,255,205,104,255,252,219    ; 12 embers
