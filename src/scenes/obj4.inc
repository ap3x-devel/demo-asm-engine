; ============================================================
;  obj4.inc - fractals: Sierpinski, Menger (animated IFS), Mandelbulb, Julia, Apollonian
;  (the last three: ray marching by distance estimation at initialization)
; ============================================================
section .data

; ---------- animated IFS: the address (digits) of each point is fixed, the scaling
;            ratio and the twist vary with time and music ----------
sierp_init:
 ldv V_R1
 stv V_S0
 ldv V_U
 stv V_S3
 o_end
%macro SIERP_LEVEL 0
 ldv t3                         ; u <- u*4 ; d = floor(u)
 kk 4.0
 o_mul
 o_dup
 o_floor
 stv t4
 ldv t4
 o_sub
 stv t3
 ldv t4                         ; tetrahedron vertex: vx = 1-2*[d>=2], vy = 1-2*(d mod 2), vz = vx*vy
 kk 1.5
 o_sub
 kk 1000.0
 o_mul
 o_sat
 kk -2.0
 o_mul
 kk 1.0
 o_add
 stv t5
 ldv t4
 kk 2.0
 o_mod
 kk -2.0
 o_mul
 kk 1.0
 o_add
 stv t6
 ldv t5
 ldv t6
 o_mul
 stv t7
 ldv t5                         ; p = mix(v, p, lambda)
 ldv t0
 ldv t8
 o_mix
 stv t0
 ldv t6
 ldv t1
 ldv t8
 o_mix
 stv t1
 ldv t7
 ldv t2
 ldv t8
 o_mix
 stv t2
 ROTA t0,t1,t9                  ; twist per level
%endmacro
sierp_frame:
 ldv V_S0
 stv t3
 kk 0.0
 stv t0
 kk 0.0
 stv t1
 kk 0.0
 stv t2
 ldv V_T                        ; lambda = 0.5 +- 0.045
 kk 0.37
 o_mul
 o_sin
 kk 0.045
 o_mul
 kk 0.5
 o_add
 stv t8
 ldv V_T                        ; twist
 kk 0.21
 o_mul
 kk 1.0
 o_add
 o_sin
 kk 0.3
 o_mul
 ldv V_A0
 o_add
 stv t9
 %rep 10
  SIERP_LEVEL
 %endrep
 ldv t0
 kk 1.85
 o_mul
 stv t0
 ldv t1
 kk 1.85
 o_mul
 stv t1
 ldv t2
 kk 1.85
 o_mul
 stv t2
 ROTV t0,t2,0.27
 ROTV t1,t2,0.16
 ldv t0
 ldv t1
 ldv t2
 outxyz
 ldv V_S3
 kk 0.3
 o_mul
 ldv t0
 o_sq
 ldv t1
 o_sq
 o_add
 ldv t2
 o_sq
 o_add
 o_sqrt
 kk 0.3
 o_mul
 o_add
 o_sat
 stv V_C
 ldv V_S3
 ldv V_T
 kk 1.4
 o_mul
 o_add
 kk 43.0
 o_mul
 o_sin
 kk 0.2
 o_mul
 kk 0.8
 o_add
 ldv V_H0
 kk 0.6
 o_mul
 kk 1.0
 o_add
 o_mul
 stv V_B
 o_end

menger_init:
 ldv V_R1
 stv V_S0
 ldv V_R2
 stv V_S1
 ldv V_W
 stv V_S2
 ldv V_U
 stv V_S3
 o_end
%macro MENGER_LEVEL 0
 ldv t3                         ; digit a in {-1,0,1}
 kk 3.0
 o_mul
 o_dup
 o_floor
 stv t6
 ldv t6
 o_sub
 stv t3
 ldv t6
 kk -1.0
 o_add
 stv t10
 ldv t4
 kk 3.0
 o_mul
 o_dup
 o_floor
 stv t6
 ldv t6
 o_sub
 stv t4
 ldv t6
 kk -1.0
 o_add
 stv t11
 ldv t5
 kk 3.0
 o_mul
 o_dup
 o_floor
 stv t6
 ldv t6
 o_sub
 stv t5
 ldv t6
 kk -1.0
 o_add
 stv t12
 kk 0.0                         ; nz = number of non-zero digits
 ldv t10
 o_abs
 o_add
 ldv t11
 o_abs
 o_add
 ldv t12
 o_abs
 o_add
 kk 2.0
 o_swap
 o_sub
 o_sat
 stv t13
 ldv t10                        ; zero digit -> 1 if s
 kk 1.0
 ldv t10
 o_abs
 o_sub
 ldv t13
 o_mul
 o_add
 stv t10
 ldv t11
 kk 1.0
 ldv t11
 o_abs
 o_sub
 ldv t13
 o_mul
 o_add
 stv t11
 ldv t12
 kk 1.0
 ldv t12
 o_abs
 o_sub
 ldv t13
 o_mul
 o_add
 stv t12
 ldv t10                        ; p = mix(q, p, lambda)
 ldv t0
 ldv t8
 o_mix
 stv t0
 ldv t11
 ldv t1
 ldv t8
 o_mix
 stv t1
 ldv t12
 ldv t2
 ldv t8
 o_mix
 stv t2
 ROTA t0,t2,t9
%endmacro
menger_frame:
 ldv V_S0
 stv t3
 ldv V_S1
 stv t4
 ldv V_S2
 stv t5
 kk 0.0
 stv t0
 kk 0.0
 stv t1
 kk 0.0
 stv t2
 ldv V_T                        ; lambda = 1/3 +- 0.03
 kk 0.31
 o_mul
 o_sin
 kk 0.03
 o_mul
 kk 0.3333
 o_add
 stv t8
 ldv V_T
 kk 0.19
 o_mul
 o_sin
 kk 0.22
 o_mul
 ldv V_A0
 o_add
 stv t9
 %rep 3
  MENGER_LEVEL
 %endrep
 ldv t0
 kk 1.35
 o_mul
 stv t0
 ldv t1
 kk 1.35
 o_mul
 stv t1
 ldv t2
 kk 1.35
 o_mul
 stv t2
 ldv t3
 kk 0.5
 o_sub
 kk 0.07
 o_mul
 ldv t0
 o_add
 stv t0
 ldv t4
 kk 0.5
 o_sub
 kk 0.07
 o_mul
 ldv t1
 o_add
 stv t1
 ldv t5
 kk 0.5
 o_sub
 kk 0.07
 o_mul
 ldv t2
 o_add
 stv t2
 ROTV t0,t2,0.23
 ROTV t1,t2,0.14
 ldv t0
 ldv t1
 ldv t2
 outxyz
 ldv V_S3
 kk 0.25
 o_mul
 ldv t0
 o_sq
 ldv t1
 o_sq
 o_add
 ldv t2
 o_sq
 o_add
 o_sqrt
 kk 0.38
 o_mul
 o_add
 o_sat
 stv V_C
 ldv V_S3
 ldv V_T
 kk 1.3
 o_mul
 o_add
 kk 47.0
 o_mul
 o_sin
 kk 0.2
 o_mul
 kk 0.8
 o_add
 ldv V_H0
 kk 0.6
 o_mul
 kk 1.0
 o_add
 o_mul
 stv V_B
 o_end

; ---------- ray marching: origin on a sphere of radius 2.8 aiming at a point near
;            the center. Output: origin -> S0..S2, direction -> t0..t2 ----------
%macro RAYINIT 0
 ldv V_R1
 kk 2.0
 o_mul
 kk -1.0
 o_add
 stv t3
 ldv t3
 o_sq
 kk -1.0
 o_mul
 kk 1.0
 o_add
 o_sqrt
 stv t4
 ldv V_R2
 kk 6.2832
 o_mul
 stv t5
 ldv t4
 ldv t5
 o_cos
 o_mul
 kk 2.8
 o_mul
 stv V_S0
 ldv t4
 ldv t5
 o_sin
 o_mul
 kk 2.8
 o_mul
 stv V_S1
 ldv t3
 kk 2.8
 o_mul
 stv V_S2
 ldv V_U
 kk 0.5
 o_sub
 kk 1.8
 o_mul
 ldv V_S0
 o_sub
 stv t0
 ldv V_V
 kk 0.5
 o_sub
 kk 1.8
 o_mul
 ldv V_S1
 o_sub
 stv t1
 ldv V_W
 kk 0.5
 o_sub
 kk 1.8
 o_mul
 ldv V_S2
 o_sub
 stv t2
 ldv t0
 o_sq
 ldv t1
 o_sq
 o_add
 ldv t2
 o_sq
 o_add
 o_sqrt
 o_inv
 stv t3
 ldv t0
 ldv t3
 o_mul
 stv t0
 ldv t1
 ldv t3
 o_mul
 stv t1
 ldv t2
 ldv t3
 o_mul
 stv t2
 kk 0.0
 stv V_S3
 kk 0.0
 stv V_S4
%endmacro
; p = origine + t * direction  ->  t3,t4,t5
%macro RAYPOS 0
 ldv V_S0
 ldv t0
 ldv V_S3
 o_mul
 o_add
 stv t3
 ldv V_S1
 ldv t1
 ldv V_S3
 o_mul
 o_add
 stv t4
 ldv V_S2
 ldv t2
 ldv V_S3
 o_mul
 o_add
 stv t5
%endmacro
%macro RAYSTEP 1                ; distance estimation macro
 RAYPOS
 %1
 o_dup
 kk 400.0
 o_mul
 o_sat
 ldv V_S4
 o_add
 stv V_S4
 kk 0.85
 o_mul
 ldv V_S3
 o_add
 stv V_S3
%endmacro
; end: position, color, brightness
%macro RAYEND 1
 RAYENDT %1,0.02,150.0
%endmacro
%macro RAYENDT 3                ; DE, contact threshold, gain (3/threshold)
 RAYPOS
 %1
 kk %2
 o_swap
 o_sub
 kk %3
 o_mul
 o_sat
 ldv t3
 o_sq
 ldv t4
 o_sq
 o_add
 ldv t5
 o_sq
 o_add
 o_sqrt
 stv t6
 kk 1.55
 ldv t6
 o_sub
 kk 10.0
 o_mul
 o_sat
 o_mul
 ldv V_S4
 kk -0.03
 o_mul
 kk 1.15
 o_add
 o_sat
 o_mul
 stv t7                         ; brightness
 ldv t3
 ldv t4
 ldv t5
 stv V_S2
 stv V_S1
 stv V_S0
 ldv t6
 kk 0.55
 o_mul
 ldv V_S4
 kk 0.04
 o_mul
 o_add
 o_sat
 stv V_S3
 ldv t7
 stv V_S4
 o_end
%endmacro
; common output: slow rotation, color, pulse on the kick drum
%macro RAYFRAME 2               ; rot_speed scale
 ldv V_S0
 kk %2
 o_mul
 stv t0
 ldv V_S1
 kk %2
 o_mul
 stv t1
 ldv V_S2
 kk %2
 o_mul
 stv t2
 ROTV t0,t2,%1
 ROTV t1,t2,0.11
 ldv t0
 ldv t1
 ldv t2
 outxyz
 ldv V_S3
 ldv V_T
 kk 0.03
 o_mul
 o_add
 stv V_C
 ldv V_S4
 ldv V_H0
 kk 0.5
 o_mul
 kk 0.9
 o_add
 o_mul
 stv V_B
 o_end
%endmacro

; ---------- Mandelbulb, power 8 (iteration frozen as soon as r > 2, like the classic "break") ----------
%macro MBULB_DE 0
 ldv t3
 stv t6
 ldv t4
 stv t7
 ldv t5
 stv t8
 kk 1.0
 stv t9
 %rep 6
  ldv t6
  o_sq
  ldv t7
  o_sq
  o_add
  ldv t8
  o_sq
  o_add
  o_sqrt
  stv t10                       ; r
  kk 2.0
  ldv t10
  o_sub
  kk 1000.0
  o_mul
  o_sat
  stv t12                       ; m = 1 while r < 2
  ldv t6
  o_sq
  ldv t7
  o_sq
  o_add
  o_sqrt
  ldv t8
  o_atan2
  kk 8.0
  o_mul
  stv t11                       ; 8 theta
  ldv t7
  ldv t6
  o_atan2
  kk 8.0
  o_mul
  stv V_S5                      ; 8 phi
  ldv t10
  o_sq
  o_dup
  o_sq
  o_mul
  ldv t10
  o_mul
  stv V_S6                      ; r^7
  ldv t9
  ldv V_S6
  kk 8.0
  o_mul
  ldv t9
  o_mul
  kk 1.0
  o_add
  kk 1.0e20
  o_min
  ldv t12
  o_mix
  stv t9                        ; dr
  ldv V_S6
  ldv t10
  o_mul
  stv V_S6                      ; r^8
  ldv t11
  o_sin
  ldv V_S6
  o_mul
  stv V_S7                      ; r^8 sin(8 theta)
  ldv V_S7
  ldv V_S5
  o_cos
  o_mul
  ldv t3
  o_add
  stv t13
  ldv V_S7
  ldv V_S5
  o_sin
  o_mul
  ldv t4
  o_add
  stv t14
  ldv t11
  o_cos
  ldv V_S6
  o_mul
  ldv t5
  o_add
  stv t15
  ldv t6
  ldv t13
  ldv t12
  o_mix
  stv t6
  ldv t7
  ldv t14
  ldv t12
  o_mix
  stv t7
  ldv t8
  ldv t15
  ldv t12
  o_mix
  stv t8
 %endrep
 ldv t6
 o_sq
 ldv t7
 o_sq
 o_add
 ldv t8
 o_sq
 o_add
 o_sqrt
 stv t10
 ldv t10
 o_log
 ldv t10
 o_mul
 kk 0.5
 o_mul
 ldv t9
 o_div
%endmacro
mbulb_init:
 RAYINIT
 %rep 30
  RAYSTEP MBULB_DE
 %endrep
 RAYEND MBULB_DE
mbulb_frame:
 RAYFRAME 0.2,1.15

; ---------- quaternionic Julia c = (-0.2, 0.6, 0.2, -0.2), iteration frozen if r > 4 ----------
%macro JULIA_DE 0
 ldv t3
 stv t6
 ldv t4
 stv t7
 ldv t5
 stv t8
 kk 0.0
 stv t10
 kk 1.0
 stv t9
 %rep 8
  ldv t6
  o_sq
  ldv t7
  o_sq
  o_add
  ldv t8
  o_sq
  o_add
  ldv t10
  o_sq
  o_add
  o_sqrt
  stv V_S5                      ; r
  kk 4.0
  ldv V_S5
  o_sub
  kk 1000.0
  o_mul
  o_sat
  stv t13                       ; m
  ldv t9
  ldv V_S5
  ldv t9
  o_mul
  kk 2.0
  o_mul
  kk 1.0e20
  o_min
  ldv t13
  o_mix
  stv t9                        ; dr
  ldv t6
  o_sq
  ldv t7
  o_sq
  o_sub
  ldv t8
  o_sq
  o_sub
  ldv t10
  o_sq
  o_sub
  kk -0.2
  o_add
  stv V_S6
  ldv t6
  ldv t7
  o_mul
  kk 2.0
  o_mul
  kk 0.6
  o_add
  stv V_S7
  ldv t6
  ldv t8
  o_mul
  kk 2.0
  o_mul
  kk 0.2
  o_add
  stv t11
  ldv t6
  ldv t10
  o_mul
  kk 2.0
  o_mul
  kk -0.2
  o_add
  stv t12
  ldv t6
  ldv V_S6
  ldv t13
  o_mix
  stv t6
  ldv t7
  ldv V_S7
  ldv t13
  o_mix
  stv t7
  ldv t8
  ldv t11
  ldv t13
  o_mix
  stv t8
  ldv t10
  ldv t12
  ldv t13
  o_mix
  stv t10
 %endrep
 ldv t6
 o_sq
 ldv t7
 o_sq
 o_add
 ldv t8
 o_sq
 o_add
 ldv t10
 o_sq
 o_add
 o_sqrt
 stv V_S5
 ldv V_S5
 o_log
 ldv V_S5
 o_mul
 kk 0.5
 o_mul
 ldv t9
 o_div
%endmacro
julia_init:
 RAYINIT
 %rep 30
  RAYSTEP JULIA_DE
 %endrep
 RAYEND JULIA_DE
julia_frame:
 RAYFRAME 0.17,1.25

; ---------- Apollonian: folding + sphere inversion, 8 iterations ----------
%macro APOL_DE 0
 ldv t3
 stv t6
 ldv t4
 stv t7
 ldv t5
 stv t8
 kk 1.0
 stv t9
 %rep 8
  ldv t6
  kk 0.5
  o_mul
  kk 0.5
  o_add
  o_frac
  kk 2.0
  o_mul
  kk -1.0
  o_add
  stv t6
  ldv t7
  kk 0.5
  o_mul
  kk 0.5
  o_add
  o_frac
  kk 2.0
  o_mul
  kk -1.0
  o_add
  stv t7
  ldv t8
  kk 0.5
  o_mul
  kk 0.5
  o_add
  o_frac
  kk 2.0
  o_mul
  kk -1.0
  o_add
  stv t8
  ldv t6
  o_sq
  ldv t7
  o_sq
  o_add
  ldv t8
  o_sq
  o_add
  kk 0.00001
  o_max
  stv t10
  kk 1.17
  ldv t10
  o_div
  stv t10
  ldv t6
  ldv t10
  o_mul
  stv t6
  ldv t7
  ldv t10
  o_mul
  stv t7
  ldv t8
  ldv t10
  o_mul
  stv t8
  ldv t9
  ldv t10
  o_mul
  stv t9
 %endrep
 ldv t7
 o_abs
 kk 0.25
 o_mul
 ldv t9
 o_div
 ldv t3
 o_sq
 ldv t4
 o_sq
 o_add
 ldv t5
 o_sq
 o_add
 o_sqrt
 kk -1.4
 o_add
 o_max
%endmacro
apol_init:
 RAYINIT
 %rep 40
  RAYSTEP APOL_DE
 %endrep
 RAYENDT APOL_DE,0.006,500.0
apol_frame:
 RAYFRAME 0.14,1.9
