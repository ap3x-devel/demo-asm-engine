; ============================================================
;  obj3.inc - minimal surfaces, vortex ring, magnetosphere, ringed-body model, dense-core model
; ============================================================
section .data
nop_init:
 o_end
; ---------- helicoid <-> catenoid (minimal surface, associate-family deformation) ----------
heli_frame:
 ldv V_V
 kk 0.5
 o_sub
 kk 4.4
 o_mul
 stv t0                         ; v
 ldv V_U
 kk 6.2832
 o_mul
 stv t1                         ; u
 ldv V_T
 kk 0.21
 o_mul
 o_sin
 kk 0.5
 o_mul
 kk 0.5
 o_add
 ldv V_A0
 o_add
 kk 1.5708
 o_mul
 stv t2                         ; alpha
 ldv t0
 o_exp
 stv t3
 ldv t0
 o_neg
 o_exp
 stv t4
 ldv t3
 ldv t4
 o_sub
 kk 0.5
 o_mul
 stv t5                         ; sinh v
 ldv t3
 ldv t4
 o_add
 kk 0.5
 o_mul
 stv t6                         ; cosh v
 ldv t2
 o_cos
 stv t7                         ; cos a
 ldv t2
 o_sin
 stv t8                         ; sin a
 ldv t7
 ldv t5
 o_mul
 ldv t1
 o_sin
 o_mul
 ldv t8
 ldv t6
 o_mul
 ldv t1
 o_cos
 o_mul
 o_add
 stv t9                         ; x
 ldv t7
 ldv t5
 o_mul
 ldv t1
 o_cos
 o_mul
 o_neg
 ldv t8
 ldv t6
 o_mul
 ldv t1
 o_sin
 o_mul
 o_add
 stv t10                        ; y
 ldv t1
 ldv t7
 o_mul
 ldv t0
 ldv t8
 o_mul
 o_add
 ldv t7
 kk 3.1416
 o_mul
 o_sub
 stv t11                        ; z
 ldv t9
 kk 0.62
 o_mul
 stv t9
 ldv t10
 kk 0.62
 o_mul
 stv t10
 ldv t11
 kk 0.62
 o_mul
 stv t11
 ROTV t9,t11,0.33
 ROTV t10,t11,0.19
 ldv t9
 ldv t10
 ldv t11
 outxyz
 ldv V_V
 ldv V_T
 kk 0.05
 o_mul
 o_add
 o_frac
 stv V_C
 ldv V_U
 kk 40.0
 o_mul
 o_sin
 o_sq
 kk 0.5
 o_mul
 ldv V_V
 kk 24.0
 o_mul
 ldv V_T
 kk 2.0
 o_mul
 o_sub
 o_sin
 o_sq
 kk 0.5
 o_mul
 o_add
 kk 0.25
 o_add
 ldv V_H0
 kk 0.5
 o_mul
 kk 0.9
 o_add
 o_mul
 stv V_B
 o_end

; ---------- vortex ring: poloidal circulation of a torus (Hill) ----------
vortex_init:
 ldv V_U
 kk 6.2832
 o_mul
 stv V_S0                       ; theta
 ldv V_R1
 o_sqrt
 kk 0.55
 o_mul
 kk 0.03
 o_add
 stv V_S1                       ; rho
 ldv V_R2
 kk 6.2832
 o_mul
 stv V_S2                       ; phi0
 ldv V_W
 stv V_S3
 o_end
vortex_frame:
 ldv V_S2
 ldv V_T
 kk 0.6
 ldv V_S1
 o_sq
 kk 0.03
 o_add
 o_div
 o_mul
 o_add
 stv t0                         ; phi
 ldv V_S0
 kk 4.0
 o_mul
 ldv V_T
 kk 2.6
 o_mul
 o_add
 o_sin
 kk 0.07
 o_mul
 kk 1.0
 o_add
 kk 1.25
 o_mul
 ldv V_S1
 ldv t0
 o_cos
 o_mul
 o_add
 stv t1                         ; R + rho cos(phi)
 ldv t1
 ldv V_S0
 o_cos
 o_mul
 stv t2
 ldv t1
 ldv V_S0
 o_sin
 o_mul
 stv t3
 ldv V_S1
 ldv t0
 o_sin
 o_mul
 stv t4
 ldv V_A0
 kk 0.8
 o_add
 stv t5
 ROTA t4,t3,t5
 ldv t2
 ldv t4
 ldv t3
 outxyz
 ldv V_S1
 kk 1.5
 o_mul
 ldv V_S3
 kk 0.2
 o_mul
 o_add
 stv V_C
 ldv V_S3
 ldv V_T
 kk 2.0
 o_mul
 o_add
 kk 41.0
 o_mul
 o_sin
 kk 0.3
 o_mul
 kk 0.7
 o_add
 ldv V_H0
 kk 0.7
 o_mul
 kk 1.0
 o_add
 o_mul
 stv V_B
 o_end

; ---------- magnetic dipole: field lines r = L sin^2(theta), trapped particles ----------
dipole_init:
 ldv V_U
 kk 18.0
 o_mul
 o_floor
 kk 0.349066
 o_mul
 stv V_S0                       ; longitude (18 meridians)
 ldv V_R1
 kk 9.0
 o_mul
 o_floor
 kk 0.111111
 o_mul
 kk 1.5
 o_pow
 kk 4.2
 o_mul
 kk 1.4
 o_add
 stv V_S1                       ; L (9 layers)
 ldv V_R2
 stv V_S2                       ; phase along the line
 ldv V_W
 stv V_S3
 o_end
dipole_frame:
 ldv V_S1
 o_inv
 o_sqrt
 stv t0
 ldv t0
 kk 1.0
 ldv t0
 o_sq
 o_sub
 o_sqrt
 o_atan2
 stv t1                         ; theta0 = asin(1/sqrt(L))
 ldv V_S2
 ldv V_T
 kk 0.22
 o_mul
 kk 1.0
 ldv V_S1
 o_sqrt
 o_div
 o_mul
 o_add
 o_frac
 kk 2.0
 o_mul
 kk 1.0
 o_sub
 o_abs
 o_neg
 kk 1.0
 o_add
 stv t2                         ; triangle 0..1 (bounce between the mirrors)
 ldv t1
 kk 3.1416
 kk 2.0
 ldv t1
 o_mul
 o_sub
 ldv t2
 o_mul
 o_add
 stv t3                         ; colatitude
 ldv V_S1
 ldv t3
 o_sin
 o_sq
 o_mul
 stv t4                         ; r
 ldv t4
 ldv t3
 o_sin
 o_mul
 stv t5                         ; cylindrical radius
 ldv t5
 ldv V_S0
 ldv V_T
 kk 0.1
 o_mul
 o_add
 o_cos
 o_mul
 kk 0.5
 o_mul
 stv t6
 ldv t5
 ldv V_S0
 ldv V_T
 kk 0.1
 o_mul
 o_add
 o_sin
 o_mul
 kk 0.5
 o_mul
 stv t7
 ldv t4
 ldv t3
 o_cos
 o_mul
 kk 0.5
 o_mul
 stv t8
 ROTA t8,t7,V_A0
 ldv t6
 ldv t8
 ldv t7
 outxyz
 ldv V_S1
 kk 0.18
 o_mul
 kk 0.1
 o_add
 stv V_C
 ldv V_S3
 ldv V_T
 kk 1.5
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
