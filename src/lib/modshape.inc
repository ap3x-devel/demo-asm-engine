%macro modshape 0
 ldv V_X
 ldv V_X
 o_mul
 ldv V_Y
 ldv V_Y
 o_mul
 o_add
 ldv V_Z
 ldv V_Z
 o_mul
 o_add
 o_sqrt
 stv V_M0
 kk 1.0
 kk 0.035
 ldv V_KICK
 o_mul
 o_add
 kk 0.015
 ldv V_L0
 o_mul
 o_add
 kk 0.04
 ldv V_SNARE
 o_mul
 ldv V_M0
 kk 4.5
 o_mul
 kk 1.0
 ldv V_SNARE
 o_sub
 kk 10.0
 o_mul
 o_sub
 o_sin
 o_mul
 o_add
 stv V_M1
 ldv V_P1
 ldv V_P2
 o_add
 kk 0.5
 o_mul
 kk 0.4
 o_sub
 kk 1.0
 o_mul
 ldv V_Y
 o_mul
 ldv V_SPIN
 kk 0.35
 o_mul
 o_add
 stv V_M2
 ldv V_X
 ldv V_M2
 o_cos
 o_mul
 ldv V_Z
 ldv V_M2
 o_sin
 o_mul
 o_sub
 stv V_M3
 ldv V_X
 ldv V_M2
 o_sin
 o_mul
 ldv V_Z
 ldv V_M2
 o_cos
 o_mul
 o_add
 stv V_Z
 ldv V_M3
 ldv V_Y
 kk 2.7
 o_mul
 ldv V_T
 kk 1.9
 o_mul
 o_add
 o_sin
 ldv V_L1
 kk 0.6
 o_add
 o_mul
 kk 0.07
 o_mul
 o_add
 stv V_X
 ldv V_Z
 ldv V_X
 kk 2.3
 o_mul
 ldv V_T
 kk 1.3
 o_mul
 o_sub
 o_sin
 ldv V_L2
 kk 0.6
 o_add
 o_mul
 kk 0.07
 o_mul
 o_add
 stv V_Z
 ldv V_Y
 ldv V_Z
 kk 3.1
 o_mul
 ldv V_T
 kk 2.3
 o_mul
 o_sub
 o_sin
 ldv V_H3
 ldv V_H1
 o_add
 kk 0.4
 o_add
 o_mul
 kk 0.06
 o_mul
 o_add
 stv V_Y
 ldv V_HAT
 kk 0.012
 o_mul
 stv V_M4
 ldv V_X
 ldv V_IDX
 kk 0.37
 o_mul
 ldv V_T
 kk 40.0
 o_mul
 o_add
 o_sin
 ldv V_M4
 o_mul
 o_add
 ldv V_M1
 o_mul
 stv V_X
 ldv V_Y
 ldv V_IDX
 kk 0.61
 o_mul
 ldv V_T
 kk 53.0
 o_mul
 o_add
 o_sin
 ldv V_M4
 o_mul
 o_add
 ldv V_M1
 o_mul
 stv V_Y
 ldv V_Z
 ldv V_IDX
 kk 0.83
 o_mul
 ldv V_T
 kk 47.0
 o_mul
 o_add
 o_sin
 ldv V_M4
 o_mul
 o_add
 ldv V_M1
 o_mul
 stv V_Z
%endmacro
