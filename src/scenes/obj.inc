; ============================================================
;  obj.inc - main objects (layer 1)
; ============================================================
section .data
; ---------- trefoil knot: ribbon of particles ----------
knot_init:
 ldv V_U
 kk 6.2832
 o_mul
 stv V_S0
 ldv V_R1
 kk 0.5
 o_sub
 stv V_S1
 ldv V_R2
 kk 0.5
 o_sub
 stv V_S2
 ldv V_W
 kk 0.5
 o_sub
 stv V_S3
 o_end
knot_frame:
 ldv V_S0
 kk 3.0
 o_mul
 o_cos
 kk 0.5
 o_mul
 kk 1.2
 o_add
 stv t0
 ldv t0
 ldv V_S0
 kk 2.0
 o_mul
 o_cos
 o_mul
 stv t1
 ldv t0
 ldv V_S0
 kk 2.0
 o_mul
 o_sin
 o_mul
 stv t2
 ldv V_S0
 kk 3.0
 o_mul
 o_sin
 kk 0.5
 o_mul
 stv t3
 ldv V_A0
 stv t4
 ldv t1
 ldv V_S1
 ldv t4
 o_mul
 o_add
 ldv t3
 ldv V_S2
 ldv t4
 o_mul
 o_add
 ldv t2
 ldv V_S3
 ldv t4
 o_mul
 o_add
 stv t2
 stv t3
 stv t1
 ldv t1
 ldv t2
 ldv V_T
 kk 0.45
 o_mul
 o_rot2
 stv t2
 stv t1
 ldv t1
 ldv t3
 ldv t2
 outxyz
 ldv V_U
 kk 3.0
 o_mul
 ldv V_T
 kk 0.08
 o_mul
 o_add
 o_frac
 stv V_C
 ldv V_U
 kk 60.0
 o_mul
 ldv V_T
 kk 5.0
 o_mul
 o_sub
 o_sin
 o_sq
 kk 0.5
 o_mul
 kk 0.5
 o_add
 ldv V_H0
 kk 0.6
 o_mul
 kk 0.7
 o_add
 o_mul
 stv V_B
 o_end
