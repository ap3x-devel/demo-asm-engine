; ============================================================
;  land.inc - particle landscapes (background layer), generic macro
;  TERRAIN name, relief, extent, z_curvature, water_level, land_threshold,
;          color_scale, color_offset, reflection_frac, star_frac,
;          sky_color, sky_lum, land_anim
;  State convention: S0..S2 base position, S3 color, S4 brightness,
;  S5 class (0 surface, 1 reflection, 2 sky), S6 animation amplitude, S7 phase.
;  Scene parameters: A0 wave amplitude, A1 brightness gain.
; ============================================================
section .data

; ---------- reliefs: stack -> height(x=slot %1, z=slot %2) ----------
%macro H_FJORD 2
 ldv %1
 kk 0.02
 o_mul
 ldv %2
 kk 0.02
 o_mul
 kk 3.7
 o_rdg 5
 kk 1.35
 o_pow
 ldv %1
 o_abs
 kk 6.0
 o_sub
 kk 0.06
 o_mul
 o_smooth
 ldv %2
 o_neg
 kk 60.0
 o_sub
 kk 0.03
 o_mul
 o_smooth
 o_max
 o_mul
 kk 40.0
 o_mul
%endmacro
%macro H_ALPINE 2               ; high sharp ridges, no water
 ldv %1
 kk 0.035
 o_mul
 ldv %2
 kk 0.035
 o_mul
 kk 1.9
 o_rdg 6
 kk 1.55
 o_pow
 ldv %2
 o_neg
 kk 8.0
 o_sub
 kk 0.045
 o_mul
 o_smooth
 o_mul
 kk 72.0
 o_mul
%endmacro
%macro H_DUNE 2                 ; dunes: asymmetric profile, furrows deformed by noise
 ldv %1
 kk 0.045
 o_mul
 ldv %2
 kk 0.03
 o_mul
 o_add
 ldv %1
 kk 0.008
 o_mul
 ldv %2
 kk 0.008
 o_mul
 kk 1.3
 o_fbm 3
 kk 7.0
 o_mul
 o_add
 o_sin
 kk 0.5
 o_mul
 kk 0.5
 o_add
 kk 1.7
 o_pow
 ldv %1
 kk 0.02
 o_mul
 ldv %2
 kk 0.02
 o_mul
 kk 4.1
 o_fbm 3
 kk 0.6
 o_mul
 kk 0.4
 o_add
 o_mul
 kk 17.0
 o_mul
%endmacro
%macro H_OCEAN 2                ; swell: three wave trains + noise foam
 ldv %1
 kk 0.09
 o_mul
 ldv %2
 kk 0.06
 o_mul
 o_add
 o_sin
 kk 1.6
 o_mul
 ldv %2
 kk 0.13
 o_mul
 ldv %1
 kk 0.05
 o_mul
 o_sub
 o_sin
 o_add
 ldv %1
 kk 0.25
 o_mul
 ldv %2
 kk 0.25
 o_mul
 kk 2.2
 o_fbm 4
 kk 1.2
 o_mul
 o_add
 kk 3.0
 o_add
%endmacro
%macro H_CANYON 2               ; strata: smooth steps over ridged noise
 ldv %1
 kk 0.016
 o_mul
 ldv %2
 kk 0.016
 o_mul
 kk 5.5
 o_rdg 4
 kk 5.0
 o_mul
 o_dup
 o_floor
 o_swap
 o_frac
 kk 3.0
 o_mul
 kk -1.8
 o_add
 o_smooth
 o_add
 kk 11.0
 o_mul
 ldv %2
 o_neg
 kk 10.0
 o_sub
 kk 0.04
 o_mul
 o_smooth
 o_mul
%endmacro
%macro H_VOLCAN 2               ; cone + crater + roughness of the flows
 ldv %1
 kk -25.0
 o_add
 o_sq
 ldv %2
 kk 62.0
 o_add
 o_sq
 o_add
 o_sqrt
 o_dup
 kk -0.0155
 o_mul
 kk 1.0
 o_add
 o_sat
 kk 1.25
 o_pow
 kk 85.0
 o_mul
 o_swap
 kk -3.0
 o_add
 kk 0.22
 o_mul
 o_smooth
 o_neg
 kk 1.0
 o_add
 kk 11.0
 o_mul
 o_sub
 ldv %1
 kk 0.05
 o_mul
 ldv %2
 kk 0.05
 o_mul
 kk 5.1
 o_rdg 4
 kk 6.0
 o_mul
 o_add
 kk 0.0
 o_max
%endmacro

%macro H_GLACE 2                ; glacier: seracs and crevasses
 ldv %1
 kk 0.03
 o_mul
 ldv %2
 kk 0.03
 o_mul
 kk 2.7
 o_fbm 4
 kk 1.3
 o_pow
 kk 16.0
 o_mul
 ldv %1
 kk 0.12
 o_mul
 ldv %1
 kk 0.02
 o_mul
 ldv %2
 kk 0.02
 o_mul
 kk 1.1
 o_fbm 3
 kk 5.0
 o_mul
 o_add
 o_sin
 o_abs
 kk 3.0
 o_mul
 kk -0.3
 o_add
 o_smooth
 kk 0.65
 o_mul
 kk 0.35
 o_add
 o_mul
%endmacro
%macro H_TERRAS 2               ; terraced rice fields: tiered hills
 ldv %1
 kk 0.012
 o_mul
 ldv %2
 kk 0.012
 o_mul
 kk 5.5
 o_fbm 4
 kk 6.0
 o_mul
 o_dup
 o_floor
 o_swap
 o_frac
 kk 5.0
 o_mul
 kk -4.0
 o_add
 o_smooth
 o_add
 kk 3.4
 o_mul
%endmacro
%macro H_KARST 2                ; karst: limestone towers
 ldv %1
 kk 0.03
 o_mul
 ldv %2
 kk 0.03
 o_mul
 kk 8.1
 o_fbm 4
 o_dup
 kk 0.52
 o_sub
 kk 12.0
 o_mul
 o_smooth
 kk 40.0
 o_mul
 o_swap
 kk 3.0
 o_mul
 o_add
%endmacro
%include "scenes/volc.inc"
%macro HN 3
 %1 %2,%3
 ldv %3
 o_neg
 kk 32.0
 o_sub
 kk 0.03
 o_mul
 o_smooth
 o_mul
 kk 1.7
 o_mul                           ; perfectly flat ground up to r=32: the camera never crosses it
 HV %2,%3                          ; fixed volcanoes (outside the flat zone)
 o_add
%endmacro
%macro TERRAIN 13
%1_init:
 ldv V_V
 kk 1.3
 o_pow
 kk 330.0
 o_mul
 kk 0.05
 o_add
 ldv V_R2
 kk 0.5
 o_sub
 kk 0.8
 o_mul
 o_add
 stv t14                        ; radius (log-polar grid: infinite ground around the object)
 ldv V_U
 kk 6.2832
 o_mul
 ldv V_R1
 kk 0.5
 o_sub
 kk 0.0105
 o_mul
 o_add
 stv t15                        ; azimuth
 ldv t14
 ldv t15
 o_sin
 o_mul
 stv t1                         ; z
 ldv t14
 ldv t15
 o_cos
 o_mul
 stv t0                         ; x
 ldv t14
 o_neg
 stv t14                        ; depth = -radius
 HN %2,t0,t14
 stv t2                         ; h
 ldv t0
 kk 1.5
 o_add
 stv t3
 HN %2,t3,t14
 stv t4                         ; hx
 ldv t14
 kk 1.5
 o_add
 stv t3
 HN %2,t0,t3
 stv t5                         ; hz
 ldv t4
 ldv t2
 o_sub
 kk 0.6666667
 o_mul
 stv t6                         ; dh/dx
 ldv t5
 ldv t2
 o_sub
 kk 0.6666667
 o_mul
 stv t7                         ; dh/dz
 ldv t6
 kk 0.72
 o_mul
 kk 0.55
 o_add
 ldv t7
 kk 0.15
 o_mul
 o_sub
 kk 1.0
 ldv t6
 o_sq
 o_add
 ldv t7
 o_sq
 o_add
 o_sqrt
 o_div
 o_sat
 stv t8                         ; lighting 0..1
 ldv t2
 kk %6
 o_sub
 kk 4.0
 o_mul
 o_sat
 stv t9                         ; land (1) / water (0)
 ldv t2                         ; land color
 kk %7
 o_mul
 kk %8
 o_add
 o_sat
 kk 1.1
 o_pow
 kk 0.55
 o_mul
 kk 0.45
 o_add
 ldv t9
 o_mul
 kk 0.04
 o_add
 stv t10
 ldv t8                         ; land brightness
 kk 1.2
 o_pow
 kk 0.8
 o_mul
 kk 0.42
 o_add
 ldv V_V
 kk -0.55
 o_mul
 kk 1.0
 o_add
 o_mul
 ldv V_V
 kk 3.0
 o_mul
 o_sat
 kk 0.6
 o_mul
 kk 0.4
 o_add
 o_mul
 stv t11
 ldv V_V                        ; water brightness
 kk -0.45
 o_mul
 kk 1.0
 o_add
 kk 0.60
 o_mul
 ldv t11
 ldv t9
 o_mix
 stv t11
 ldv V_W                        ; reflection or sky
 kk %9
 o_sub
 kk 1000.0
 o_mul
 o_sat
 stv t12
 ldv V_W
 kk %10
 o_sub
 kk 1000.0
 o_mul
 o_sat
 stv t13                        ; sky
 ldv t12
 ldv t13
 o_sub
 kk 0.0
 o_max
 stv t12                        ; pure reflection
 ; sky: dome (stars, dust, glow)
 ldv V_U
 kk 0.5
 o_sub
 kk 6.2832
 o_mul
 ldv V_R1
 kk 0.5
 o_sub
 kk 0.05
 o_mul
 o_add
 stv t3                         ; azimuth
 ldv V_V
 kk 0.85
 o_mul
 kk 0.03
 o_add
 ldv V_R2
 kk 0.5
 o_sub
 kk 0.03
 o_mul
 o_add
 stv t4                         ; elevation
 ldv t4
 o_cos
 ldv t3
 o_sin
 o_mul
 kk 300.0
 o_mul
 stv t5                         ; sx
 ldv t4
 o_sin
 kk 300.0
 o_mul
 kk -2.0
 o_add
 stv t6                         ; sy
 ldv t4
 o_cos
 ldv t3
 o_cos
 o_mul
 kk -300.0
 o_mul
 stv t7                         ; sz
 ; final position
 ldv t0
 ldv t5
 ldv t13
 o_mix
 ldv t2                         ; y surface
 ldv t9
 o_mul
 kk %5
 o_add
 stv t8
 ldv t8                         ; y = mix(surface, mirror, reflection)
 kk %5
 kk 2.0
 o_mul
 ldv t8
 o_sub
 ldv t12
 o_mix
 ldv t6
 ldv t13
 o_mix
 ldv t1
 ldv t7
 ldv t13
 o_mix
 ; color and brightness
 ldv t10
 kk %11
 ldv V_R2
 kk 0.12
 o_mul
 o_add
 ldv t13
 o_mix
 ldv t11
 ldv t12
 kk 0.6
 o_mul
 kk 1.0
 o_sub
 o_neg
 o_mul
 kk 0.25
 ldv V_R1
 o_sq
 kk 0.9
 o_mul
 o_add
 kk %12
 o_mul
 ldv t13
 o_mix
 keep5
 ldv t12                        ; class
 ldv t13
 o_add
 ldv t13
 o_add
 stv V_S5
 ldv t9                         ; animation amplitude: water = 1, land = anim_land
 kk %13
 o_mul
 ldv t9
 o_neg
 kk 1.0
 o_add
 o_add
 stv V_S6
 ldv V_W
 stv V_S7
 LAVA_A                         ; lava points: part of the ground becomes projectiles
 HN %2,t3,t4
 kk %5
 o_add
 stv t5
 LAVA_B
 o_end
%endmacro

; ---------- common animation: swell / undulation + sky twinkle ----------
terrain_frame:
 ldv V_S1
 ldv V_S0
 kk 0.9
 o_mul
 ldv V_S2
 kk 0.55
 o_mul
 o_add
 ldv V_T
 kk 1.2
 o_mul
 o_add
 ldv V_S7
 kk 6.2832
 o_mul
 o_add
 o_sin
 ldv V_S6
 o_mul
 ldv V_A0
 o_mul
 ldv V_S5
 o_dup
 kk 2.0
 o_swap
 o_sub
 o_mul
 kk 1.2
 o_mul
 kk 1.0
 o_add
 o_mul
 o_add
 stv V_Y
 ldv V_S0
 stv V_X
 ldv V_S2
 stv V_Z
 ldv V_S3
 stv V_C
 ldv V_S4
 ldv V_A1
 o_mul
 ldv V_T                        ; sky twinkle
 kk 3.0
 o_mul
 ldv V_S7
 kk 40.0
 o_mul
 o_add
 o_sin
 kk 0.3
 o_mul
 kk 0.7
 o_add
 kk 1.0
 ldv V_S5
 o_dup
 kk 1.0
 o_sub
 o_mul
 kk 0.5
 o_mul
 o_mix
 o_mul
 stv V_B
 ; the objects light the ground: halo pulsing with the kick and the energy
 ldv V_S0
 ldv V_S0
 o_mul
 ldv V_S2
 ldv V_S2
 o_mul
 o_add
 o_neg
 kk 0.0022
 o_mul
 o_exp
 stv t0
 ldv V_B
 kk 1.0
 ldv t0
 kk 1.4
 kk 1.6
 ldv V_KICK
 o_mul
 o_add
 kk 1.0
 ldv V_E
 o_mul
 o_add
 o_mul
 o_add
 o_mul
 stv V_B
 ldv V_C
 kk 0.93
 ldv V_C
 o_sub
 ldv t0
 kk 0.5
 o_mul
 o_sat
 o_mul
 o_add
 stv V_C
 LAVA_F
 o_end

;               name    relief    extent curv  water thresh scale  offset refl  sky   color   lumC  animL
TERRAIN fjord,  H_FJORD,  170.0,  90.0, -2.6,  0.6, 0.04545, 0.0,  0.5,  0.82,  0.88,  1.0,  0.0
TERRAIN alpine,  H_ALPINE, 190.0,  95.0, -3.0, -1000.0, 0.02174, 0.0, 0.85, 0.85, 0.88,  0.7,  0.04
TERRAIN dune,   H_DUNE,   220.0,  85.0, -5.0, -1000.0, 0.09, 0.0, 0.85, 0.85,  0.30,  0.35, 0.10
TERRAIN ocean,  H_OCEAN,  240.0,  125.0, -6.0, -1000.0, 0.11, -0.10, 0.85, 0.85, 0.88,  0.9,  0.55
TERRAIN canyon, H_CANYON, 200.0,  85.0, -4.0, -1000.0, 0.035, 0.0, 0.85, 0.85, 0.30,  0.35, 0.05
TERRAIN volcano, H_VOLCAN, 200.0,  95.0, -3.0, -1000.0, 0.0172, 0.0, 0.85, 0.85, 0.86,  0.6,  0.12
TERRAIN ice,  H_GLACE,  220.0,  85.0, -4.0, -1000.0, 0.05, 0.0, 0.85, 0.85, 0.88,  0.7,  0.04
TERRAIN terrace, H_TERRAS, 220.0,  85.0, -4.0, -1000.0, 0.05, 0.0, 0.85, 0.85, 0.88,  0.5,  0.06
TERRAIN karst,  H_KARST,  220.0,  85.0, -4.0, -1000.0, 0.025, 0.0, 0.85, 0.85, 0.88, 0.5, 0.04
