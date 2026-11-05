; ============================================================
;  vmmac.inc - compact writing of VM programs
; ============================================================
%macro ldv 1
 db OP_LD, %1
%endmacro
%macro stv 1
 db OP_ST, %1
%endmacro
%macro kk 1
 o_k %1
%endmacro
%define t0  (V_T0+0)
%define t1  (V_T0+1)
%define t2  (V_T0+2)
%define t3  (V_T0+3)
%define t4  (V_T0+4)
%define t5  (V_T0+5)
%define t6  (V_T0+6)
%define t7  (V_T0+7)
%define t8  (V_T0+8)
%define t9  (V_T0+9)
%define t10 (V_T0+10)
%define t11 (V_T0+11)
%define t12 (V_T0+12)
%define t13 (V_T0+13)
%define t14 (V_T0+14)
%define t15 (V_T0+15)
; standard output: stack = x y z  ->  V_X V_Y V_Z
; soft deformation of the object on the music: breathing (kick H0, bass L0)
; and slow undulation driven by the other channels (L1, L2, H3)
%include "lib/modshape.inc"
%macro outxyz 0
 stv V_Z
 stv V_Y
 stv V_X
 modshape
%endmacro
; stores the stack (x y z c b) into S0..S4 (init): reverse deposit order
%macro keep5 0
 stv V_S4
 stv V_S3
 stv V_S2
 stv V_S1
 stv V_S0
%endmacro
; static program: reads S0..S4 back and returns them as output
%macro static_prog 1
%1:
 ldv V_S0
 stv V_X
 ldv V_S1
 stv V_Y
 ldv V_S2
 stv V_Z
 ldv V_S3
 stv V_C
 ldv V_S4
 stv V_B
 o_end
%endmacro
; linear mapping u in [0,1] -> [a,b]
%macro lerpk 2
 kk (%2)-(%1)
 o_mul
 kk %1
 o_add
%endmacro
; rotates the pair (slot A, slot B) by an angle T*rate, in place
%macro ROTV 3
 ldv %1
 ldv %2
 ldv V_T
 kk %3
 o_mul
 o_rot2
 stv %2
 stv %1
%endmacro
; rotates the pair (A,B) by a fixed angle given by a variable
%macro ROTA 3
 ldv %1
 ldv %2
 ldv %3
 o_rot2
 stv %2
 stv %1
%endmacro
; Euler: slot += dt * (stack)
%macro EUL 2
 kk %2
 o_mul
 ldv %1
 o_add
%endmacro
