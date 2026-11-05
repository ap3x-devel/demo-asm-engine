; ============================================================
;  obj2.inc - physical objects: attractors, hypertorus, galaxy...
;  Per-point state: S0..S2 position / phase, S3.. free.
; ============================================================
section .data
; ---------- common attractor init: random point near the origin ----------
%macro ATT_INIT 3               ; amplitude_x amplitude_y offset_z
 ldv V_R1
 kk 0.5
 o_sub
 kk %1
 o_mul
 stv V_S0
 ldv V_R2
 kk 0.5
 o_sub
 kk %2
 o_mul
 stv V_S1
 ldv V_W
 kk 0.5
 o_sub
 kk %1
 o_mul
 kk %3
 o_add
 stv V_S2
 ldv V_U
 stv V_S3
 o_end
%endmacro
; ---------- Lorenz: sigma 10, rho 28, beta 8/3 ----------
%macro LORENZ_STEP 0
 ldv V_S1
 ldv V_S0
 o_sub
 kk 10.0
 o_mul
 kk 0.0036
 o_mul
 ldv V_S0
 o_add
 stv t0
 kk 28.0
 ldv V_S2
 o_sub
 ldv V_S0
 o_mul
 ldv V_S1
 o_sub
 kk 0.0036
 o_mul
 ldv V_S1
 o_add
 stv t1
 ldv V_S0
 ldv V_S1
 o_mul
 ldv V_S2
 kk 2.66667
 o_mul
 o_sub
 kk 0.0036
 o_mul
 ldv V_S2
 o_add
 stv t2
 ldv t0
 stv V_S0
 ldv t1
 stv V_S1
 ldv t2
 stv V_S2
%endmacro
lorenz_init:
 ATT_INIT 30.0,30.0,25.0
lorenz_frame:
 %rep 5
  LORENZ_STEP
 %endrep
 ldv V_S0
 kk 0.16
 o_mul
 stv t0
 ldv V_S2
 kk -25.0
 o_add
 kk 0.16
 o_mul
 stv t1
 ldv V_S1
 kk 0.16
 o_mul
 stv t2
 ROTV t0,t2,0.22
 ldv t0
 ldv t1
 ldv t2
 outxyz
 ldv V_S2                       ; color: height on the attractor
 kk 0.02
 o_mul
 ldv V_S0
 o_abs
 kk 0.012
 o_mul
 o_add
 o_sat
 stv V_C
 ldv V_S3
 ldv V_T
 kk 2.0
 o_mul
 o_add
 kk 40.0
 o_mul
 o_sin
 kk 0.25
 o_mul
 kk 0.75
 o_add
 ldv V_H0
 kk 0.5
 o_mul
 kk 1.0
 o_add
 o_mul
 stv V_B
 o_end

; ---------- Aizawa : a .95 b .7 c .6 d 3.5 e .25 f .1 ----------
%macro AIZAWA_STEP 0
 ldv V_S2
 kk 0.7
 o_sub
 ldv V_S0
 o_mul
 ldv V_S1
 kk 3.5
 o_mul
 o_sub
 kk 0.0045
 o_mul
 ldv V_S0
 o_add
 stv t0
 ldv V_S0
 kk 3.5
 o_mul
 ldv V_S2
 kk 0.7
 o_sub
 ldv V_S1
 o_mul
 o_add
 kk 0.0045
 o_mul
 ldv V_S1
 o_add
 stv t1
 ldv V_S2
 kk 0.95
 o_mul
 kk 0.6
 o_add
 ldv V_S2
 o_dup
 o_sq
 o_mul
 kk 0.333333
 o_mul
 o_sub
 ldv V_S0
 o_sq
 ldv V_S1
 o_sq
 o_add
 kk 1.0
 ldv V_S2
 kk 0.25
 o_mul
 o_add
 o_mul
 o_sub
 ldv V_S2
 ldv V_S0
 o_dup
 o_sq
 o_mul
 o_mul
 kk 0.1
 o_mul
 o_add
 kk 0.0045
 o_mul
 ldv V_S2
 o_add
 stv t2
 ldv t0
 stv V_S0
 ldv t1
 stv V_S1
 ldv t2
 stv V_S2
%endmacro
aizawa_init:
 ATT_INIT 1.0,1.0,0.0
aizawa_frame:
 %rep 8
  AIZAWA_STEP
 %endrep
 ldv V_S0
 kk 2.6
 o_mul
 stv t0
 ldv V_S2
 kk -0.2
 o_add
 kk 2.6
 o_mul
 stv t1
 ldv V_S1
 kk 2.6
 o_mul
 stv t2
 ROTV t0,t2,0.3
 ROTA t1,t2,V_A1
 ldv t0
 ldv t1
 ldv t2
 outxyz
 ldv V_S2
 kk 0.5
 o_mul
 kk 0.45
 o_add
 ldv V_S0
 o_sq
 ldv V_S1
 o_sq
 o_add
 o_sqrt
 kk 0.25
 o_mul
 o_add
 o_sat
 stv V_C
 ldv V_S3
 ldv V_T
 kk 1.5
 o_mul
 o_add
 kk 33.0
 o_mul
 o_sin
 kk 0.3
 o_mul
 kk 0.7
 o_add
 ldv V_H0
 kk 0.6
 o_mul
 kk 1.0
 o_add
 o_mul
 stv V_B
 o_end

; ---------- Thomas : dx = sin y - b x (b = 0.208) ----------
%macro THOMAS_STEP 0
 ldv V_S1
 o_sin
 ldv V_S0
 kk 0.208
 o_mul
 o_sub
 kk 0.07
 o_mul
 ldv V_S0
 o_add
 stv t0
 ldv V_S2
 o_sin
 ldv V_S1
 kk 0.208
 o_mul
 o_sub
 kk 0.07
 o_mul
 ldv V_S1
 o_add
 stv t1
 ldv V_S0
 o_sin
 ldv V_S2
 kk 0.208
 o_mul
 o_sub
 kk 0.07
 o_mul
 ldv V_S2
 o_add
 stv t2
 ldv t0
 stv V_S0
 ldv t1
 stv V_S1
 ldv t2
 stv V_S2
%endmacro
thomas_init:
 ATT_INIT 8.0,8.0,0.0
thomas_frame:
 %rep 3
  THOMAS_STEP
 %endrep
 ldv V_S0
 kk 0.75
 o_mul
 stv t0
 ldv V_S1
 kk 0.75
 o_mul
 stv t1
 ldv V_S2
 kk 0.75
 o_mul
 stv t2
 ROTV t0,t2,0.17
 ROTV t1,t2,0.11
 ldv t0
 ldv t1
 ldv t2
 outxyz
 ldv V_S0
 kk 0.07
 o_mul
 ldv V_S1
 kk 0.05
 o_mul
 o_add
 kk 0.5
 o_add
 o_sat
 stv V_C
 ldv V_S3
 ldv V_T
 kk 1.0
 o_mul
 o_add
 kk 29.0
 o_mul
 o_sin
 kk 0.3
 o_mul
 kk 0.7
 o_add
 ldv V_H0
 kk 0.6
 o_mul
 kk 1.0
 o_add
 o_mul
 stv V_B
 o_end

; ---------- Halvorsen : a 1.89 ----------
%macro HALV_STEP 0
 ldv V_S0
 kk -1.89
 o_mul
 ldv V_S1
 kk 4.0
 o_mul
 o_sub
 ldv V_S2
 kk 4.0
 o_mul
 o_sub
 ldv V_S1
 o_sq
 o_sub
 kk 0.0042
 o_mul
 ldv V_S0
 o_add
 stv t0
 ldv V_S1
 kk -1.89
 o_mul
 ldv V_S2
 kk 4.0
 o_mul
 o_sub
 ldv V_S0
 kk 4.0
 o_mul
 o_sub
 ldv V_S2
 o_sq
 o_sub
 kk 0.0042
 o_mul
 ldv V_S1
 o_add
 stv t1
 ldv V_S2
 kk -1.89
 o_mul
 ldv V_S0
 kk 4.0
 o_mul
 o_sub
 ldv V_S1
 kk 4.0
 o_mul
 o_sub
 ldv V_S0
 o_sq
 o_sub
 kk 0.0042
 o_mul
 ldv V_S2
 o_add
 stv t2
 ldv t0
 stv V_S0
 ldv t1
 stv V_S1
 ldv t2
 stv V_S2
%endmacro
halv_init:
 ATT_INIT 8.0,8.0,0.0
halv_frame:
 %rep 4
  HALV_STEP
 %endrep
 ldv V_S0
 kk 0.42
 o_mul
 stv t0
 ldv V_S1
 kk 0.42
 o_mul
 stv t1
 ldv V_S2
 kk 0.42
 o_mul
 stv t2
 ROTV t0,t2,0.2
 ROTV t1,t2,0.13
 ldv t0
 ldv t1
 ldv t2
 outxyz
 ldv V_S0
 ldv V_S1
 o_add
 kk 0.04
 o_mul
 kk 0.5
 o_add
 o_sat
 stv V_C
 ldv V_S3
 ldv V_T
 kk 1.2
 o_mul
 o_add
 kk 37.0
 o_mul
 o_sin
 kk 0.3
 o_mul
 kk 0.7
 o_add
 ldv V_H0
 kk 0.6
 o_mul
 kk 1.0
 o_add
 o_mul
 stv V_B
 o_end

; ---------- Clifford hypertorus in 4D rotation + stereographic projection ----------
hyper_init:
 ldv V_U
 kk 6.2832
 o_mul
 stv V_S0                       ; angle a
 ldv V_R1
 kk 6.2832
 o_mul
 stv V_S1                       ; angle b
 ldv V_R2
 kk 0.12
 o_mul
 kk 0.707
 o_add
 stv V_S2                       ; radius (layer thickness)
 ldv V_W
 stv V_S3
 o_end
hyper_frame:
 ldv V_S0                       ; point on S3: (r cos a, r sin a, s cos b, s sin b)
 o_cos
 ldv V_S2
 o_mul
 stv t0                         ; x
 ldv V_S0
 o_sin
 ldv V_S2
 o_mul
 stv t1                         ; y
 ldv V_S1
 o_cos
 kk 1.0
 ldv V_S2
 o_sq
 o_sub
 o_sqrt
 o_mul
 stv t2                         ; z
 ldv V_S1
 o_sin
 kk 1.0
 ldv V_S2
 o_sq
 o_sub
 o_sqrt
 o_mul
 stv t3                         ; w
 ROTV t0,t3,0.31                ; 4D rotations: planes xw, yz and zw
 ROTV t1,t2,0.23
 ROTV t2,t3,0.17
 kk 1.0
 ldv t3
 kk 0.12
 ldv V_A1
 o_mul
 o_sub
 o_sub
 kk 0.18
 o_max
 o_inv
 stv t4                         ; stereographic factor
 ldv t0
 ldv t4
 o_mul
 kk 1.6
 o_mul
 ldv t1
 ldv t4
 o_mul
 kk 1.6
 o_mul
 ldv t2
 ldv t4
 o_mul
 kk 1.6
 o_mul
 outxyz
 ldv t3
 kk 0.5
 o_mul
 kk 0.5
 o_add
 stv V_C
 ldv t4
 kk 0.45
 o_mul
 o_sat
 ldv V_S3
 ldv V_T
 kk 1.5
 o_mul
 o_add
 kk 31.0
 o_mul
 o_sin
 kk 0.2
 o_mul
 kk 0.8
 o_add
 o_mul
 ldv V_H0
 kk 0.5
 o_mul
 kk 0.8
 o_add
 o_mul
 stv V_B
 o_end

; ---------- spiral galaxy: density wave + differential rotation (Kepler) ----------
gal_init:
 ldv V_U
 o_sqrt
 kk 1.0
 o_mul
 stv V_S0                       ; radius (density ~ 1/r)
 ldv V_R1
 kk 2.0
 o_mul
 o_floor
 kk 3.1416
 o_mul
 stv V_S1                       ; arm (0 or pi)
 ldv V_R2
 kk 0.5
 o_sub
 stv V_S2                       ; angular offset
 ldv V_W
 kk 0.5
 o_sub
 stv V_S3
 o_end
gal_frame:
 ldv V_S0
 kk 0.12
 o_add
 o_sqrt
 stv t0
 ldv V_S0
 kk 0.09
 o_add
 o_inv
 ldv V_S0
 o_sqrt
 o_mul
 stv t1                         ; angular velocity ~ r^-1.5 (flattened)
 ldv V_S1                       ; angle = arm + log spiral + rotation + dispersion
 ldv V_S0
 kk 0.04
 o_add
 o_log
 kk -2.6
 o_mul
 o_add
 ldv t1
 ldv V_T
 kk 0.35
 o_mul
 o_mul
 o_add
 ldv V_S2
 kk 0.9
 ldv V_S0
 o_sub
 kk 0.25
 o_max
 o_mul
 o_add
 stv t2
 ldv V_S0
 kk 5.5
 o_mul
 stv t3                         ; real radius
 ldv t2
 o_cos
 ldv t3
 o_mul
 stv t4
 ldv t2
 o_sin
 ldv t3
 o_mul
 stv t5
 ldv V_S3
 kk 0.9
 o_mul
 kk 1.0
 ldv V_S0
 kk 2.2
 o_mul
 o_sub
 kk 0.12
 o_max
 o_mul
 stv t6                         ; height: disc thickness decreases with r
 ldv t4
 ldv t6
 ldv t5
 outxyz
 ldv V_S0
 kk -1.0
 o_mul
 kk 1.0
 o_add
 kk 0.8
 o_mul
 ldv V_S2
 o_abs
 kk 0.15
 o_mul
 o_add
 stv V_C
 ldv V_S0
 kk -1.2
 o_mul
 kk 1.3
 o_add
 o_sat
 ldv V_S3
 ldv V_T
 o_add
 kk 70.0
 o_mul
 o_sin
 kk 0.2
 o_mul
 kk 0.8
 o_add
 o_mul
 kk 0.35
 o_add
 ldv V_H0
 kk 0.4
 o_mul
 kk 1.0
 o_add
 o_mul
 stv V_B
 o_end
