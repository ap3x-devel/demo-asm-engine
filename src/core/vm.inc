; ============================================================
;  vm.inc - SSE stack machine (4 points at a time) describing
;  landscapes and objects with formulas.
;
;  A program = a sequence of bytes (o_xxx macros). It reads variables
;  (u, v, time, music channel levels, scene parameters...) and
;  leaves X Y Z (position), C (palette coordinate 0..1) and B (brightness).
;  "init" pass:  once at the start of the scene, stores state in S0..S7.
;  "frame" pass: every frame, reads S0..S7 back and animates.
; ============================================================

; ---------- variables (each = 4 lanes of 32 bits) ----------
%define V_U     0
%define V_V     1
%define V_W     2          ; random 0..1
%define V_R1    3
%define V_R2    4
%define V_IDX   5
%define V_T     6          ; time in the scene (s)
%define V_TT    7          ; global time (s)
%define V_DT    8
%define V_ROWPH 9          ; phase inside the row 0..1
%define V_ROW   10         ; row inside the pattern 0..64 (continuous)
%define V_PATPH 11         ; phase inside the pattern 0..1
%define V_PROG  12         ; scene progress 0..1
%define V_L0    13         ; smoothed level of the 4 channels
%define V_L1    14
%define V_L2    15
%define V_L3    16
%define V_H0    17         ; attack envelope of the 4 channels (1 -> 0)
%define V_H1    18
%define V_H2    19
%define V_H3    20
%define V_P0    21         ; smoothed pitch of the 4 channels 0..1
%define V_P1    22
%define V_P2    23
%define V_P3    24
%define V_KICK  25
%define V_SNARE 26
%define V_HAT   27
%define V_E     28         ; global energy
%define V_A0    29         ; scene parameters 0..7 (base + gain * source)
%define V_A1    30
%define V_A2    31
%define V_A3    32
%define V_A4    33
%define V_A5    34
%define V_A6    35
%define V_A7    36
%define V_S0    37         ; per-point state 0..7
%define V_S1    38
%define V_S2    39
%define V_S3    40
%define V_S4    41
%define V_S5    42
%define V_S6    43
%define V_S7    44
%define V_X     45
%define V_Y     46
%define V_Z     47
%define V_C     48
%define V_B     49
%define V_T0    50         ; 16 temporaries 50..65
%define V_SPIN  66         ; accumulated phase driven by the music
%define V_BEAT  67         ; row counter since the start of the scene
%define V_M0    68
%define V_M1    69
%define V_M2    70
%define V_M3    71
%define V_M4    72
%define VM_NVAR 80

; ---------- instruction set ----------
%assign _op 0
%macro defop 1
 %assign OP_%1 _op
 %assign _op _op+1
%endmacro
defop END
defop K
defop LD
defop ST
defop DUP
defop DROP
defop SWAP
defop OVER
defop ROT
defop ADD
defop SUB
defop MUL
defop DIV
defop MIN
defop MAX
defop POW
defop MOD
defop ATAN2
defop NEG
defop ABS
defop SQRT
defop SIN
defop COS
defop EXP
defop LOG
defop FLOOR
defop FRAC
defop SAT
defop SMOOTH
defop SQ
defop INV
defop MIX
defop CLAMP
defop NOISE
defop FBM
defop RDG
defop HASH
defop ROT2
defop POL
defop NOP
%assign OP_COUNT _op

%macro o_end 0
 db OP_END
%endmacro
%macro o_k 1
 db OP_K
 dd __float32__(%1)
%endmacro
%macro o_ld 1
 db OP_LD, %1
%endmacro
%macro o_st 1
 db OP_ST, %1
%endmacro
%macro o_fbm 1
 db OP_FBM, %1
%endmacro
%macro o_rdg 1
 db OP_RDG, %1
%endmacro
%macro o_dup 0
 db OP_DUP
%endmacro
%macro o_drop 0
 db OP_DROP
%endmacro
%macro o_swap 0
 db OP_SWAP
%endmacro
%macro o_over 0
 db OP_OVER
%endmacro
%macro o_rot 0
 db OP_ROT
%endmacro
%macro o_add 0
 db OP_ADD
%endmacro
%macro o_sub 0
 db OP_SUB
%endmacro
%macro o_mul 0
 db OP_MUL
%endmacro
%macro o_div 0
 db OP_DIV
%endmacro
%macro o_min 0
 db OP_MIN
%endmacro
%macro o_max 0
 db OP_MAX
%endmacro
%macro o_pow 0
 db OP_POW
%endmacro
%macro o_mod 0
 db OP_MOD
%endmacro
%macro o_atan2 0
 db OP_ATAN2
%endmacro
%macro o_neg 0
 db OP_NEG
%endmacro
%macro o_abs 0
 db OP_ABS
%endmacro
%macro o_sqrt 0
 db OP_SQRT
%endmacro
%macro o_sin 0
 db OP_SIN
%endmacro
%macro o_cos 0
 db OP_COS
%endmacro
%macro o_exp 0
 db OP_EXP
%endmacro
%macro o_log 0
 db OP_LOG
%endmacro
%macro o_floor 0
 db OP_FLOOR
%endmacro
%macro o_frac 0
 db OP_FRAC
%endmacro
%macro o_sat 0
 db OP_SAT
%endmacro
%macro o_smooth 0
 db OP_SMOOTH
%endmacro
%macro o_sq 0
 db OP_SQ
%endmacro
%macro o_inv 0
 db OP_INV
%endmacro
%macro o_mix 0
 db OP_MIX
%endmacro
%macro o_clamp 0
 db OP_CLAMP
%endmacro
%macro o_noise 0
 db OP_NOISE
%endmacro
%macro o_hash 0
 db OP_HASH
%endmacro
%macro o_rot2 0
 db OP_ROT2
%endmacro
%macro o_pol 0
 db OP_POL
%endmacro

section .data
align 16
vc_abs   times 4 dd 0x7FFFFFFF
vc_sign  times 4 dd 0x80000000
vc_one   times 4 dd 1.0
vc_half  times 4 dd 0.5
vc_two   times 4 dd 2.0
vc_three times 4 dd 3.0
vc_pi    times 4 dd 3.14159265
vc_halfpi times 4 dd 1.57079633
vc_2pi   times 4 dd 6.28318531
vc_inv2pi times 4 dd 0.15915494
vc_s3    times 4 dd -0.16666667
vc_s5    times 4 dd 0.0083333333
vc_s7    times 4 dd -0.0001984127
vc_s9    times 4 dd 0.0000027557
vc_127i  times 4 dd 127
vc_mant  times 4 dd 0x007FFFFF
vc_ln2   times 4 dd 0.69314718
vc_log2e times 4 dd 1.44269504
vc_l2k   times 4 dd 2.88539008
vc_l3    times 4 dd 0.33333333
vc_l5    times 4 dd 0.2
vc_l7    times 4 dd 0.14285714
vc_e1    times 4 dd 0.69314718
vc_e2    times 4 dd 0.24022651
vc_e3    times 4 dd 0.05550411
vc_e4    times 4 dd 0.00961813
vc_e5    times 4 dd 0.00133336
vc_e6    times 4 dd 0.00015403
vc_m126  times 4 dd -126.0
vc_p126  times 4 dd 126.0
vc_tiny  times 4 dd 1.0e-20
vc_a1    times 4 dd -0.0464964749
vc_a2    times 4 dd 0.15931422
vc_a3    times 4 dd -0.327622764
vc_zero  times 4 dd 0.0
vc_h1    times 4 dd 0x9E3779B1
vc_h2    times 4 dd 0x85EBCA6B
vc_h3    times 4 dd 0x5bd1e995
vc_n1    times 4 dd 73856093
vc_n2    times 4 dd 19349663
vc_n3    times 4 dd 83492791
vc_expo  times 4 dd 0x3F800000
vc_i1    times 4 dd 1
vc_m3    times 4 dd 1.5
vc_lac   times 4 dd 2.03
vc_nrm   dd 0.0

vm_jt:
 dd vo_end, vo_k, vo_ld, vo_st, vo_dup, vo_drop, vo_swap, vo_over, vo_rot
 dd vo_add, vo_sub, vo_mul, vo_div, vo_min, vo_max, vo_pow, vo_mod, vo_atan2
 dd vo_neg, vo_abs, vo_sqrt, vo_sin, vo_cos, vo_exp, vo_log, vo_floor, vo_frac
 dd vo_sat, vo_smooth, vo_sq, vo_inv, vo_mix, vo_clamp, vo_noise, vo_fbm, vo_rdg
 dd vo_hash, vo_rot2, vo_pol, vo_nop

section .bss
alignb 16
vm_var   resd 4*VM_NVAR
vm_stk   resd 4*48
nz_c     resd 4*8
nz_fx    resd 4
nz_fy    resd 4
nz_fz    resd 4
nz_ix    resd 4
nz_iy    resd 4
nz_iz    resd 4
fb_sum   resd 4
fb_x     resd 4
fb_y     resd 4
fb_z     resd 4
fb_amp   resd 4
fb_n     resd 1
fb_rdg   resd 1

section .text
%macro POP1 1
 sub ebx,16
 movaps %1,[ebx]
%endmacro
%macro PUSH1 1
 movaps [ebx],%1
 add ebx,16
%endmacro
%macro TOP1 1
 movaps %1,[ebx-16]
%endmacro
%macro SETTOP 1
 movaps [ebx-16],%1
%endmacro
%macro NEXT 0
 movzx eax,byte [esi]
 inc esi
 jmp [vm_jt+eax*4]
%endmacro

; ---------------- vector functions ----------------
; xmm0 = x -> xmm0 = sin(x)   (clobbers xmm1-xmm3)
vm_sinps:
 movaps xmm1,xmm0
 mulps xmm1,[vc_inv2pi]
 roundps xmm1,xmm1,0
 mulps xmm1,[vc_2pi]
 subps xmm0,xmm1
 movaps xmm1,xmm0
 andps xmm1,[vc_abs]
 movaps xmm3,[vc_halfpi]
 cmpltps xmm3,xmm1
 movaps xmm1,xmm0
 andps xmm1,[vc_sign]
 movaps xmm2,[vc_pi]
 orps xmm2,xmm1
 subps xmm2,xmm0
 andps xmm2,xmm3
 andnps xmm3,xmm0
 orps xmm2,xmm3
 movaps xmm0,xmm2
 movaps xmm1,xmm0
 mulps xmm1,xmm1
 movaps xmm2,[vc_s9]
 mulps xmm2,xmm1
 addps xmm2,[vc_s7]
 mulps xmm2,xmm1
 addps xmm2,[vc_s5]
 mulps xmm2,xmm1
 addps xmm2,[vc_s3]
 mulps xmm2,xmm1
 addps xmm2,[vc_one]
 mulps xmm0,xmm2
 ret

; xmm0 = x>0 -> xmm0 = log2(x)  (clobbers xmm1-xmm3)
vm_log2ps:
 movaps xmm1,xmm0
 psrld xmm1,23
 psubd xmm1,[vc_127i]
 cvtdq2ps xmm1,xmm1
 pand xmm0,[vc_mant]
 por xmm0,[vc_one]
 movaps xmm2,xmm0
 subps xmm0,[vc_one]
 addps xmm2,[vc_one]
 divps xmm0,xmm2                     ; t = (m-1)/(m+1)
 movaps xmm2,xmm0
 mulps xmm2,xmm2                     ; t2
 movaps xmm3,[vc_l7]
 mulps xmm3,xmm2
 addps xmm3,[vc_l5]
 mulps xmm3,xmm2
 addps xmm3,[vc_l3]
 mulps xmm3,xmm2
 addps xmm3,[vc_one]
 mulps xmm0,xmm3
 mulps xmm0,[vc_l2k]
 addps xmm0,xmm1
 ret

; xmm0 = x -> xmm0 = 2^x  (clobbers xmm1-xmm3)
vm_exp2ps:
 minps xmm0,[vc_p126]
 maxps xmm0,[vc_m126]
 movaps xmm1,xmm0
 roundps xmm1,xmm1,1                 ; floor
 subps xmm0,xmm1                     ; f
 cvtps2dq xmm1,xmm1
 pslld xmm1,23
 movaps xmm2,[vc_e6]
 mulps xmm2,xmm0
 addps xmm2,[vc_e5]
 mulps xmm2,xmm0
 addps xmm2,[vc_e4]
 mulps xmm2,xmm0
 addps xmm2,[vc_e3]
 mulps xmm2,xmm0
 addps xmm2,[vc_e2]
 mulps xmm2,xmm0
 addps xmm2,[vc_e1]
 mulps xmm2,xmm0
 addps xmm2,[vc_one]
 paddd xmm2,xmm1
 movaps xmm0,xmm2
 ret

; xmm0 = y, xmm1 = x -> xmm0 = atan2(y,x)   (clobbers xmm1-xmm5)
vm_atan2ps:
 movaps xmm4,xmm0                    ; y
 movaps xmm5,xmm1                    ; x
 movaps xmm2,xmm0
 andps xmm2,[vc_abs]                 ; |y|
 movaps xmm3,xmm1
 andps xmm3,[vc_abs]                 ; |x|
 movaps xmm0,xmm2
 maxps xmm0,xmm3                     ; mx
 minps xmm2,xmm3                     ; mn
 addps xmm0,[vc_tiny]
 divps xmm2,xmm0                     ; a in [0,1]
 movaps xmm0,xmm2
 mulps xmm0,xmm0                     ; s
 movaps xmm1,[vc_a1]
 mulps xmm1,xmm0
 addps xmm1,[vc_a2]
 mulps xmm1,xmm0
 addps xmm1,[vc_a3]
 mulps xmm1,xmm0
 mulps xmm1,xmm2
 addps xmm1,xmm2                     ; r
 ; if |y| > |x| : r = pi/2 - r
 movaps xmm2,xmm4
 andps xmm2,[vc_abs]
 movaps xmm3,xmm5
 andps xmm3,[vc_abs]
 cmpltps xmm3,xmm2                   ; |x| < |y|
 movaps xmm0,[vc_halfpi]
 subps xmm0,xmm1
 andps xmm0,xmm3
 andnps xmm3,xmm1
 orps xmm0,xmm3                      ; r
 ; if x < 0 : r = pi - r
 xorps xmm2,xmm2
 cmpltps xmm2,xmm5                   ; 0 < x  -> mask x>0
 movaps xmm1,[vc_pi]
 subps xmm1,xmm0
 andps xmm0,xmm2
 andnps xmm2,xmm1
 orps xmm0,xmm2
 ; sign of y
 andps xmm4,[vc_sign]
 xorps xmm0,xmm4
 ret

; 3D integer hash: ecx = corner offset (bits 0,1,2 = dx,dy,dz) -> xmm0 = [-1,1)
%macro HCORNER 3
 movdqa xmm0,[nz_ix]
 %if %1
 paddd xmm0,[vc_i1]
 %endif
 pmulld xmm0,[vc_n1]
 movdqa xmm1,[nz_iy]
 %if %2
 paddd xmm1,[vc_i1]
 %endif
 pmulld xmm1,[vc_n2]
 pxor xmm0,xmm1
 movdqa xmm1,[nz_iz]
 %if %3
 paddd xmm1,[vc_i1]
 %endif
 pmulld xmm1,[vc_n3]
 pxor xmm0,xmm1
 movdqa xmm1,xmm0
 psrld xmm1,13
 pxor xmm0,xmm1
 pmulld xmm0,[vc_h3]
 movdqa xmm1,xmm0
 psrld xmm1,15
 pxor xmm0,xmm1
 psrld xmm0,9
 por xmm0,[vc_expo]
 subps xmm0,[vc_m3]
 addps xmm0,xmm0
%endmacro

; xmm0,xmm1,xmm2 = x,y,z -> xmm0 = value noise in [-1,1]
vm_noise:
 roundps xmm3,xmm0,1
 subps xmm0,xmm3
 cvttps2dq xmm3,xmm3
 movdqa [nz_ix],xmm3
 roundps xmm3,xmm1,1
 subps xmm1,xmm3
 cvttps2dq xmm3,xmm3
 movdqa [nz_iy],xmm3
 roundps xmm3,xmm2,1
 subps xmm2,xmm3
 cvttps2dq xmm3,xmm3
 movdqa [nz_iz],xmm3
 ; smoothing f*f*(3-2f)
 %assign q 0
 %rep 3
  %if q==0
   movaps xmm3,xmm0
  %elif q==1
   movaps xmm3,xmm1
  %else
   movaps xmm3,xmm2
  %endif
  movaps xmm4,xmm3
  mulps xmm4,xmm4
  addps xmm3,xmm3
  movaps xmm5,[vc_three]
  subps xmm5,xmm3
  mulps xmm4,xmm5
  %if q==0
   movaps [nz_fx],xmm4
  %elif q==1
   movaps [nz_fy],xmm4
  %else
   movaps [nz_fz],xmm4
  %endif
  %assign q q+1
 %endrep
 HCORNER 0,0,0
 movaps [nz_c],xmm0
 HCORNER 1,0,0
 movaps [nz_c+16],xmm0
 HCORNER 0,1,0
 movaps [nz_c+32],xmm0
 HCORNER 1,1,0
 movaps [nz_c+48],xmm0
 HCORNER 0,0,1
 movaps [nz_c+64],xmm0
 HCORNER 1,0,1
 movaps [nz_c+80],xmm0
 HCORNER 0,1,1
 movaps [nz_c+96],xmm0
 HCORNER 1,1,1
 movaps [nz_c+112],xmm0
 movaps xmm6,[nz_fx]
 movaps xmm7,[nz_fy]
 ; x
 movaps xmm0,[nz_c]
 movaps xmm1,[nz_c+16]
 subps xmm1,xmm0
 mulps xmm1,xmm6
 addps xmm0,xmm1                     ; x00
 movaps xmm1,[nz_c+32]
 movaps xmm2,[nz_c+48]
 subps xmm2,xmm1
 mulps xmm2,xmm6
 addps xmm1,xmm2                     ; x10
 subps xmm1,xmm0
 mulps xmm1,xmm7
 addps xmm0,xmm1                     ; y0
 movaps xmm3,[nz_c+64]
 movaps xmm1,[nz_c+80]
 subps xmm1,xmm3
 mulps xmm1,xmm6
 addps xmm3,xmm1                     ; x01
 movaps xmm1,[nz_c+96]
 movaps xmm2,[nz_c+112]
 subps xmm2,xmm1
 mulps xmm2,xmm6
 addps xmm1,xmm2                     ; x11
 subps xmm1,xmm3
 mulps xmm1,xmm7
 addps xmm3,xmm1                     ; y1
 subps xmm3,xmm0
 mulps xmm3,[nz_fz]
 addps xmm0,xmm3
 ret

; fBm: fb_x/y/z = point, fb_n octaves, fb_rdg=1: "ridged"
; output xmm0: normalized fBm [-1,1] or ridged [0,1]
vm_fbm:
 xorps xmm0,xmm0
 movaps [fb_sum],xmm0
 movaps xmm0,[vc_one]
 movaps [fb_amp],xmm0
 xorps xmm0,xmm0
 movss [vc_nrm],xmm0
 xor ecx,ecx
.o:
 push ecx
 movaps xmm0,[fb_x]
 movaps xmm1,[fb_y]
 movaps xmm2,[fb_z]
 call vm_noise
 pop ecx
 cmp dword [fb_rdg],0
 je .nr
 andps xmm0,[vc_abs]
 movaps xmm1,[vc_one]
 subps xmm1,xmm0
 mulps xmm1,xmm1
 movaps xmm0,xmm1
 jmp .acc
.nr:
.acc:
 mulps xmm0,[fb_amp]
 addps xmm0,[fb_sum]
 movaps [fb_sum],xmm0
 movss xmm1,[vc_nrm]
 addss xmm1,[fb_amp]
 movss [vc_nrm],xmm1
 movaps xmm0,[fb_amp]
 mulps xmm0,[vc_half]
 movaps [fb_amp],xmm0
 ; p = p * lac + offset (decorrelation)
 movaps xmm0,[fb_x]
 mulps xmm0,[vc_lac]
 movaps [fb_x],xmm0
 movaps xmm0,[fb_y]
 mulps xmm0,[vc_lac]
 movaps [fb_y],xmm0
 movaps xmm0,[fb_z]
 mulps xmm0,[vc_lac]
 movaps [fb_z],xmm0
 inc ecx
 cmp ecx,[fb_n]
 jb .o
 movss xmm1,[vc_nrm]
 shufps xmm1,xmm1,0
 movaps xmm0,[fb_sum]
 divps xmm0,xmm1
 ret

; hash(x) -> [0,1)  : xmm0 -> xmm0
vm_hashps:
 roundps xmm0,xmm0,1
 cvttps2dq xmm0,xmm0
 pmulld xmm0,[vc_h1]
 movdqa xmm1,xmm0
 psrld xmm1,15
 pxor xmm0,xmm1
 pmulld xmm0,[vc_h2]
 movdqa xmm1,xmm0
 psrld xmm1,13
 pxor xmm0,xmm1
 psrld xmm0,9
 por xmm0,[vc_expo]
 subps xmm0,[vc_one]
 ret

; ---------------- interpreter ----------------
; input: esi = program; clean stack. Output: variables V_X.. updated.
vm_run:
 push ebx
 push esi
 mov ebx,vm_stk
 NEXT
vo_end:
 pop esi
 pop ebx
 ret
vo_nop:
 NEXT
vo_k:
 movss xmm0,[esi]
 add esi,4
 shufps xmm0,xmm0,0
 PUSH1 xmm0
 NEXT
vo_ld:
 movzx eax,byte [esi]
 inc esi
 shl eax,4
 movaps xmm0,[vm_var+eax]
 PUSH1 xmm0
 NEXT
vo_st:
 movzx eax,byte [esi]
 inc esi
 shl eax,4
 POP1 xmm0
 movaps [vm_var+eax],xmm0
 NEXT
vo_dup:
 TOP1 xmm0
 PUSH1 xmm0
 NEXT
vo_drop:
 sub ebx,16
 NEXT
vo_swap:
 movaps xmm0,[ebx-16]
 movaps xmm1,[ebx-32]
 movaps [ebx-16],xmm1
 movaps [ebx-32],xmm0
 NEXT
vo_over:
 movaps xmm0,[ebx-32]
 PUSH1 xmm0
 NEXT
vo_rot:                                 ; a b c -> b c a
 movaps xmm0,[ebx-48]
 movaps xmm1,[ebx-32]
 movaps xmm2,[ebx-16]
 movaps [ebx-48],xmm1
 movaps [ebx-32],xmm2
 movaps [ebx-16],xmm0
 NEXT
%macro BINOP 2
vo_%1:
 POP1 xmm1
 TOP1 xmm0
 %2 xmm0,xmm1
 SETTOP xmm0
 NEXT
%endmacro
BINOP add,addps
BINOP sub,subps
BINOP mul,mulps
BINOP div,divps
BINOP min,minps
BINOP max,maxps
vo_pow:
 POP1 xmm1                              ; b
 TOP1 xmm0                              ; a
 movaps [fb_x],xmm1
 maxps xmm0,[vc_tiny]
 call vm_log2ps
 mulps xmm0,[fb_x]
 call vm_exp2ps
 SETTOP xmm0
 NEXT
vo_mod:                                 ; a b -> a - b floor(a/b)
 POP1 xmm1
 TOP1 xmm0
 movaps xmm2,xmm0
 divps xmm2,xmm1
 roundps xmm2,xmm2,1
 mulps xmm2,xmm1
 subps xmm0,xmm2
 SETTOP xmm0
 NEXT
vo_atan2:                               ; y x -> angle
 POP1 xmm1
 TOP1 xmm0
 call vm_atan2ps
 SETTOP xmm0
 NEXT
vo_neg:
 TOP1 xmm0
 xorps xmm0,[vc_sign]
 SETTOP xmm0
 NEXT
vo_abs:
 TOP1 xmm0
 andps xmm0,[vc_abs]
 SETTOP xmm0
 NEXT
vo_sqrt:
 TOP1 xmm0
 maxps xmm0,[vc_zero]
 sqrtps xmm0,xmm0
 SETTOP xmm0
 NEXT
vo_sin:
 TOP1 xmm0
 call vm_sinps
 SETTOP xmm0
 NEXT
vo_cos:
 TOP1 xmm0
 addps xmm0,[vc_halfpi]
 call vm_sinps
 SETTOP xmm0
 NEXT
vo_exp:
 TOP1 xmm0
 mulps xmm0,[vc_log2e]
 call vm_exp2ps
 SETTOP xmm0
 NEXT
vo_log:
 TOP1 xmm0
 maxps xmm0,[vc_tiny]
 call vm_log2ps
 mulps xmm0,[vc_ln2]
 SETTOP xmm0
 NEXT
vo_floor:
 TOP1 xmm0
 roundps xmm0,xmm0,1
 SETTOP xmm0
 NEXT
vo_frac:
 TOP1 xmm0
 movaps xmm1,xmm0
 roundps xmm1,xmm1,1
 subps xmm0,xmm1
 SETTOP xmm0
 NEXT
vo_sat:
 TOP1 xmm0
 maxps xmm0,[vc_zero]
 minps xmm0,[vc_one]
 SETTOP xmm0
 NEXT
vo_smooth:
 TOP1 xmm0
 maxps xmm0,[vc_zero]
 minps xmm0,[vc_one]
 movaps xmm1,xmm0
 mulps xmm1,xmm1
 addps xmm0,xmm0
 movaps xmm2,[vc_three]
 subps xmm2,xmm0
 mulps xmm1,xmm2
 SETTOP xmm1
 NEXT
vo_sq:
 TOP1 xmm0
 mulps xmm0,xmm0
 SETTOP xmm0
 NEXT
vo_inv:
 TOP1 xmm1
 movaps xmm0,[vc_one]
 divps xmm0,xmm1
 SETTOP xmm0
 NEXT
vo_mix:                                 ; a b t -> a + (b-a) t
 POP1 xmm2
 POP1 xmm1
 TOP1 xmm0
 subps xmm1,xmm0
 mulps xmm1,xmm2
 addps xmm0,xmm1
 SETTOP xmm0
 NEXT
vo_clamp:                               ; x lo hi
 POP1 xmm2
 POP1 xmm1
 TOP1 xmm0
 maxps xmm0,xmm1
 minps xmm0,xmm2
 SETTOP xmm0
 NEXT
vo_noise:
 POP1 xmm2
 POP1 xmm1
 TOP1 xmm0
 call vm_noise
 SETTOP xmm0
 NEXT
vo_fbm:
 movzx eax,byte [esi]
 inc esi
 mov [fb_n],eax
 mov dword [fb_rdg],0
 jmp vo_fb2
vo_rdg:
 movzx eax,byte [esi]
 inc esi
 mov [fb_n],eax
 mov dword [fb_rdg],1
vo_fb2:
 POP1 xmm2
 POP1 xmm1
 TOP1 xmm0
 movaps [fb_x],xmm0
 movaps [fb_y],xmm1
 movaps [fb_z],xmm2
 call vm_fbm
 SETTOP xmm0
 NEXT
vo_hash:
 TOP1 xmm0
 call vm_hashps
 SETTOP xmm0
 NEXT
vo_rot2:                                ; x y a -> x' y'
 POP1 xmm0                              ; a
 movaps [fb_x],xmm0
 call vm_sinps
 movaps [fb_y],xmm0                     ; sin
 movaps xmm0,[fb_x]
 addps xmm0,[vc_halfpi]
 call vm_sinps
 movaps [fb_z],xmm0                     ; cos
 movaps xmm1,[ebx-16]                   ; y
 movaps xmm0,[ebx-32]                   ; x
 movaps xmm2,xmm0
 mulps xmm2,[fb_z]
 movaps xmm3,xmm1
 mulps xmm3,[fb_y]
 subps xmm2,xmm3                        ; x cos - y sin
 mulps xmm0,[fb_y]
 mulps xmm1,[fb_z]
 addps xmm0,xmm1                        ; x sin + y cos
 movaps [ebx-32],xmm2
 movaps [ebx-16],xmm0
 NEXT
vo_pol:                                 ; r a -> r cos a, r sin a
 POP1 xmm0                              ; a
 movaps [fb_x],xmm0
 call vm_sinps
 movaps [fb_y],xmm0
 movaps xmm0,[fb_x]
 addps xmm0,[vc_halfpi]
 call vm_sinps                          ; cos
 TOP1 xmm1                              ; r
 mulps xmm0,xmm1
 mulps xmm1,[fb_y]
 SETTOP xmm0
 PUSH1 xmm1
 NEXT
