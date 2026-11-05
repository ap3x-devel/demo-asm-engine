; ============================================================
;  fx_common.inc - common utilities (SSE + x87)
; ============================================================
%define MAXP 409600
global cam_roll, cam_brx, cam_bry, cam_bd, cam_broll, vsplit, vtx, col, npts, g_T, g_ST, g_DT, g_K, g_E, cam_rx, cam_ry, cam_d, psize, rng
global g_L, g_P, g_H, g_F

%macro FC 2              ; FC xmmN, 1.5   (loads a float constant, clobbers edx)
 mov edx,__float32__(%2)
 movd %1,edx
%endmacro

section .data
align 16
c0      dd 0.0
c255    dd 255.0
c2pi    dd 6.2831853
cpi     dd 3.14159265
c1o255  dd 0.00392157
c2m23   dd 1.1920929e-7
chalf   dd 0.5
cone    dd 1.0
ctwo    dd 2.0
c2m24   dd 5.9604645e-8
cin_rs  dd 0x2F6E2B1
cmone   dd -1.0
cin_dir dd 1.0
cin_cd  dd 1.0
cin_td  dd 1.0
%include "camtab.inc"
cthree  dd 3.0
; cinematic camera moves: rx0 rx1 ry0 ry1 d0 d1 roll  (one line per type, type = scene mod 8)
; title backgrounds: rx0 rx1 ry0 ry1 d0 d1 (landscape fly-over)
tit_tab:
 dd -2.0,-9.0,-18.0,16.0,17.0,7.5       ; title 1: approach to the mountains
 dd -14.0,-4.0,22.0,-14.0,7.0,12.5        ; title 2: crane pull-back above the fjord
 dd -5.0,-8.0,-26.0,26.0,11.0,6.0         ; title 3: lateral traverse and gentle dive
cin_tab:
 dd -8.0,10.0,-8.0,8.0,1.7,0.30,0.0      ; dive into the object
 dd 10.0,-6.0,10.0,-6.0,0.28,1.5,8.0     ; exit from the object, revealing pull-back
 dd -48.0,-2.0,-8.0,8.0,1.3,0.85,0.0    ; descending crane
 dd 0.0,-8.0,-10.0,10.0,1.1,0.65,10.0     ; wide orbit
 dd -20.0,15.0,12.0,-12.0,2.0,0.18,22.0   ; deep dive with roll
 dd 4.0,-40.0,10.0,-6.0,0.7,1.5,-8.0     ; ascending crane
 dd -34.0,-4.0,-12.0,12.0,1.6,0.35,14.0 ; diving spiral
 dd -6.0,-6.0,-10.0,0.0,0.5,1.15,-16.0   ; fast sweep

section .bss
alignb 16
vtx     resd 3*MAXP+16
col     resd MAXP+16
npts    resd 1
g_T     resd 1           ; global time (s)
g_ST    resd 1           ; time inside the scene (s)
g_DT    resd 1           ; delta time (s)
g_K     resd 1           ; kick drum impact 0..1
g_E     resd 1           ; global energy 0..1
cam_rx  resd 1           ; degrees
cam_ry  resd 1
cam_d   resd 1
cam_roll resd 1
cam_brx resd 1           ; background camera (independent from the object camera)
cam_bry resd 1
cam_bd  resd 1
cam_broll resd 1
vsplit  resd 1           ; number of background points (group 0) in vtx
psize   resd 1
rng     resd 1
tmpf    resd 4
g_L     resd 4           ; smoothed level of the 4 channels (0..1)
g_P     resd 4           ; smoothed pitch of the 4 channels
g_H     resd 4           ; attack envelope of the 4 channels
g_F     resd 1           ; current frame (50 Hz)

section .text

; xmm0 = angle -> xmm0 = sin, xmm1 = cos
sincos:
 movss [tmpf],xmm0
 fld dword [tmpf]
 fsincos
 fstp dword [tmpf]
 movss xmm1,[tmpf]
 fstp dword [tmpf]
 movss xmm0,[tmpf]
 ret

; xmm0 = x -> xmm0 = 2^x
fexp2:
 movss [tmpf],xmm0
 fld dword [tmpf]
 fld st0
 frndint
 fsub st1,st0
 fxch st1
 f2xm1
 fld1
 faddp st1,st0
 fscale
 fstp st1
 fstp dword [tmpf]
 movss xmm0,[tmpf]
 ret

; xorshift32 -> xmm0 in [0,1)    (clobbers eax, ecx)
rnd:
 mov eax,[rng]
 mov ecx,eax
 shl ecx,13
 xor eax,ecx
 mov ecx,eax
 shr ecx,17
 xor eax,ecx
 mov ecx,eax
 shl ecx,5
 xor eax,ecx
 mov [rng],eax
 shr eax,9
 cvtsi2ss xmm0,eax
 mulss xmm0,[c2m23]
 ret

; hash(eax) -> xmm0 in [0,1)     (clobbers eax, ecx)
hashf:
 imul eax,eax,0x9E3779B1
 mov ecx,eax
 shr ecx,15
 xor eax,ecx
 imul eax,eax,0x85EBCA6B
 mov ecx,eax
 shr ecx,13
 xor eax,ecx
 imul eax,eax,0xC2B2AE35
 shr eax,9
 cvtsi2ss xmm0,eax
 mulss xmm0,[c2m23]
 ret
