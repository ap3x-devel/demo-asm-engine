; ============================================================
;  scenes.inc - scene table, aligned on the music's pattern orders
;  (generated once by tools/gen_scenes.sh, then edited by hand)
; ============================================================
%include "lib/vmmac.inc"
%include "core/palettes.inc"
%include "scenes/land.inc"
%include "scenes/obj.inc"
%include "scenes/obj2.inc"
%include "scenes/obj3.inc"
%include "scenes/obj4.inc"
%include "scenes/obj5.inc"
%include "scenes/obj6.inc"
%include "scenes/land2.inc"
%include "scenes/title.inc"


section .data
align 4
%macro BGL 5                    ; init palette gain seed wave_amplitude
 LAYER 1,300000,DOM_GRID,600,%1_init,terrain_frame,%2,%3,%4,1.7
  PRM %5,V_L1,0.12
  PRM 1.0,V_E,0.35
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
 ENDLAYER
%endmacro
%macro FGL 8                    ; init frame palette n domain seed size gain
 LAYER 1,%4,%5,1,%1_init,%2,%3,%8,%6,%7
  PRM 0.1,V_L0,0.12
  PRM 1.0,V_E,0.2
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
 ENDLAYER
%endmacro
; physical objects (palette, seed)
%macro F_KNOT 2
 LAYER 1,12288,DOM_LINE,1,knot_init,knot_frame,%1,1.0,%2,1.0
  PRM 0.07,V_L0,0.12
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
 ENDLAYER
%endmacro
%macro F_LORENZ 2
 FGL lorenz,lorenz_frame,%1,24576,DOM_RAND,%2,0.8,0.6
%endmacro
%macro F_AIZAWA 2
 FGL aizawa,aizawa_frame,%1,32768,DOM_RAND,%2,0.8,0.5
%endmacro
%macro F_THOMAS 2
 FGL thomas,thomas_frame,%1,32768,DOM_RAND,%2,0.8,0.45
%endmacro
%macro F_HALV 2
 FGL halv,halv_frame,%1,32768,DOM_RAND,%2,0.8,0.45
%endmacro
%macro F_HYPER 2
 FGL hyper,hyper_frame,%1,22000,DOM_RAND,%2,0.7,0.1
%endmacro
%macro F_GAL 2
 FGL gal,gal_frame,%1,70000,DOM_RAND,%2,0.7,0.3
%endmacro
%macro F_SIERP 2
 FGL sierp,sierp_frame,%1,30000,DOM_RAND,%2,0.8,0.45
%endmacro
%macro F_MENGER 2
 FGL menger,menger_frame,%1,22000,DOM_RAND,%2,0.7,0.1
%endmacro
%macro F_MBULB 2
 FGL mbulb,mbulb_frame,%1,22000,DOM_RAND,%2,0.7,0.1
%endmacro
%macro F_JULIA 2
 FGL julia,julia_frame,%1,14000,DOM_RAND,%2,0.65,0.05
%endmacro
%macro F_APOL 2
 FGL apol,apol_frame,%1,60000,DOM_RAND,%2,0.8,0.5
%endmacro
%macro F_HELI 2
 LAYER 1,61440,DOM_GRID,256,nop_init,heli_frame,%1,0.2,%2,0.8
  PRM 0.1,V_L0,0.12
  PRM 1.0,V_E,0.2
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
 ENDLAYER
%endmacro
%macro F_TEXT 2                      ; text number, palette
 LAYER 2,28000,DOM_RAND,1,txt_init,txt_frame,%2,0.9,%1,1.0
 %rep 8
  NOPRM
 %endrep
 ENDLAYER
%endmacro
%macro F_VORTEX 2
 FGL vortex,vortex_frame,%1,40000,DOM_RAND,%2,0.8,0.16
%endmacro
%macro F_DIPOLE 2
 FGL dipole,dipole_frame,%1,40000,DOM_RAND,%2,0.8,0.35
%endmacro
; one scene: order, rx0, ry0, ryAmpl, distance, point_size
%macro SCN 7
 SCENE %1,%2,1.8
 CAMERA %3,1.0,0.11, %4,%5,0.07, %6,0.3,0.13, %7,0.04
%endmacro
%macro SCNR 8                    ; order row morph rx0 ry0 ryA distance size
 SCENE %1,%2,%3
 CAMERA %4,0.5,0.11, %5,%6,0.07, %7,0.3,0.13, %8,0.04
%endmacro
; ---- new objects (generated: src/gen/gen.sh and atts.sh) ----
%define PTS_G 61440
%macro FGRID 5                  ; name palette seed n_points size
 LAYER 1,%4,DOM_GRID,256,nop_init,%1_frame,%2,0.2,%3,%5
  PRM 0.1,V_L0,0.12
  PRM 1.0,V_E,0.2
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
  NOPRM
 ENDLAYER
%endmacro
%macro F_MOBIUS 2
 FGRID mobius,%1,%2,61440,0.8
%endmacro
%macro F_KLEIN 2
 FGRID klein,%1,%2,61440,0.8
%endmacro
%macro F_SHELL 2
 FGRID shell,%1,%2,61440,0.8
%endmacro
%macro F_SUPER 2
 FGRID supershape,%1,%2,61440,0.8
%endmacro
%macro F_DINI 2
 FGRID dini,%1,%2,61440,0.8
%endmacro
%macro F_KUEN 2
 FGRID kuen,%1,%2,61440,0.8
%endmacro
%macro F_ENNEPER 2
 FGRID enneper,%1,%2,61440,0.8
%endmacro
%macro F_ROMAN 2
 FGRID roman,%1,%2,61440,0.8
%endmacro
%macro F_HARMO 2
 FGRID harmo,%1,%2,61440,0.8
%endmacro
%macro F_COIL 2
 FGRID coil,%1,%2,61440,0.8
%endmacro
%macro F_CHLADNI 2
 FGRID chladni,%1,%2,61440,0.8
%endmacro
%macro F_HOPF 2
 FGRID hopf,%1,%2,90112,0.7
%endmacro
%macro F_DNA 2
 FGRID dna,%1,%2,61440,0.8
%endmacro
%macro F_RIPPLE 2
 FGRID ripple,%1,%2,61440,0.8
%endmacro
%macro F_RTORUS 2
 FGRID rippletorus,%1,%2,61440,0.8
%endmacro
%macro F_GYRO 2
 FGRID gyro,%1,%2,61440,0.8
%endmacro
%macro F_SPHH 2
 FGRID sphharm,%1,%2,61440,0.8
%endmacro
%macro F_LISSA 2
 FGRID lissa,%1,%2,61440,0.8
%endmacro
%macro F_SCHWARZ 2
 FGRID schwarz,%1,%2,61440,0.8
%endmacro
%macro F_ROSSLER 2
 FGL rossler,rossler_frame,%1,40000,DOM_RAND,%2,0.8,0.5
%endmacro
%macro F_CHUA 2
 FGL chua,chua_frame,%1,40000,DOM_RAND,%2,0.8,0.5
%endmacro
%ifdef TESTLIST
%include "scenes/test.inc"
%else
%include "scenes/list.inc"
%endif
