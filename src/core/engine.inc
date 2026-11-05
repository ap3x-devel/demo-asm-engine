; ============================================================
;  engine.inc - two-layer scenes (landscape background + object), music sync
;
;  A scene = camera + layer 0 (landscape, far behind) + layer 1 (object).
;  Each layer is a VM program (see vm.inc) or a native routine.
;  Scenes start on (order, row) positions of the song: the start
;  frames are found in the fr_pos table from the pre-pass -> exact sync.
; ============================================================
; --- layer descriptor ---
%define LY_KIND  0
%define LY_N     4
%define LY_DOM   8
%define LY_NU    12
%define LY_INIT  16
%define LY_FRAME 20
%define LY_PAL   24
%define LY_GAIN  28
%define LY_SEED  32
%define LY_PSZ   36
%define LY_PRM   40
%define LY_VTX   136
%define LY_COL   140
%define LY_STATE 144
%define LY_PALP  148
%define LY_INVN  152
%define LY_NUF   156
%define LY_INVNU 160
%define LY_INVNV 164
%define LY_SIZE  176
; --- scene record ---
%define SC_ORDER 0
%define SC_ROW   4
%define SC_MORPH 8
%define SC_CAM   12
%define SC_BG    56
%define SC_FG    232
%define SC_SIZE  408

; point domains
%define DOM_LINE 0
%define DOM_GRID 1
%define DOM_RAND 2

%macro LAYER 10                 ; kind n dom nu init frame pal gain seed psz
 dd %1,%2,%3,%4,%5,%6,%7
 dd __float32__(%8)
 dd %9
 dd __float32__(%10)
%endmacro
%macro PRM 3                    ; base source gain   (source = variable number or -1)
 dd __float32__(%1), %2, __float32__(%3)
%endmacro
%macro NOPRM 0
 dd 0, -1, 0
%endmacro
%macro ENDLAYER 0
 times 10 dd 0
%endmacro
%macro SCENE 3                  ; order row morph_duration
 dd %1,%2
 dd __float32__(%3)
%endmacro
%macro CAMERA 11                ; rx0 rxA rxW  ry0 ryA ryW  d0 dA dW  point_size  kick_zoom
 %rep 11
  dd __float32__(%1)
  %rotate 1
 %endrep
%endmacro

%macro PAL5 15
 db %1,%2,%3, %4,%5,%6, %7,%8,%9, %10,%11,%12, %13,%14,%15, 0
%endmacro

section .data
align 16
c_att   dd 30.0
c_bright dd 2.0                         ; global brightness (point alpha)
c_cf     dd 0.78                        ; color compression: brightness floor
c_cb     dd 0.22
c_psk    dd 1.3
c_rel   dd 6.0
c_hk    dd 0.04
c_pk    dd 12.0
inv882  dd 0.0011337868
inv44100 dd 0.0000226757
inv255  dd 0.00392157
c_fi    dd 0.625
c_fo    dd 0.3333333
c_last  dd 0.02
c_dtmin dd 0.001
c_bgboost dd 1.0
c_dtmax dd 0.05
c_spin  dd 4.0
c_spin0 dd 0.6
c_pt02  dd 0.02

section .bss
alignb 16
lay_bg  resb LY_SIZE
lay_fg  resb LY_SIZE
st_bg   resd 8*(MAXP/2)+64
st_fg   resd 8*(MAXP/2)+64
pal_bg  resd 256
pal_fg  resd 256
cam_cfg resd 12
samp    resd 1
curf    resd 1
kscene  resd 1
bg_par  resd 1
cin_p   resd 1
cin_pk  resd 1
cin_tx  resd 1
cin_ty  resd 1
cin_cx  resd 1
cin_cy  resd 1
cin_ps  resd 1
cin_mv  resd 1
cin_vel resd 1
cin_or  resd 1
cin_dt  resd 1
cin_dg  resd 1
lastk   resd 1
tprev   resd 1
fade    resd 1
g_ff    resd 1
g_KD    resd 1
g_SN    resd 1
g_HT    resd 1
g_spin  resd 1
g_dur   resd 1
scn_f   resd 128
lp_prog resd 1
lp_mode resd 1
lp_ng   resd 1
lp_g    resd 1
lp_pv   resd 1
lp_pc   resd 1
lp_ly   resd 1
lp_c    resd 1
lp_sp   resd 1
mg_dst  resd 1
alignb 16
tx_i    resd 4
tx_j    resd 4
tx_x    resd 4
tx_y    resd 4
tx_z    resd 4

section .text
; ---------- gradient palette: edi = destination (256 dwords), eax = id ----------
mkgrad:
 pushad
 mov [mg_dst],edi
 shl eax,4
 lea esi,[gp_tab+eax]
 xor ebx,ebx
.l:
 mov ecx,ebx
 and ecx,63
 imul ecx,ecx,266305
 add ecx,65535
 shr ecx,16                          ; fq 0..256
 mov eax,ebx
 shr eax,6
 lea eax,[eax+eax*2]
 add eax,esi                         ; pointer to the start point
 xor ebp,ebp
 xor edx,edx
.c:
 movzx edi,byte [eax+edx]
 push eax
 movzx eax,byte [eax+edx+3]
 imul eax,ecx
 push ecx
 neg ecx
 add ecx,256
 imul edi,ecx
 pop ecx
 add eax,edi
 shr eax,8
 cmp eax,255
 jbe .ok
 mov eax,255
.ok:
 push ecx
 mov ecx,edx
 shl ecx,3
 shl eax,cl
 pop ecx
 or ebp,eax
 pop eax
 inc edx
 cmp edx,3
 jb .c
 mov edi,[mg_dst]
 mov [edi+ebx*4],ebp
 inc ebx
 cmp ebx,256
 jb .l
 popad
 ret
; ---------- music-related variables (once per frame) ----------
%macro SETV 2
 movss xmm0,[%2]
 shufps xmm0,xmm0,0
 movaps [vm_var+%1*16],xmm0
%endmacro
vars_update:
 pushad
 mov esi,[curf]
 shl esi,2
 movss xmm6,[g_DT]
 mulss xmm6,[c_att]
 minss xmm6,[cone]
 movss xmm7,[g_DT]
 mulss xmm7,[c_rel]
 minss xmm7,[cone]
 xorps xmm0,xmm0
 movss [g_KD],xmm0
 movss [g_SN],xmm0
 movss [g_HT],xmm0
 xor ebx,ebx
.c:
 movzx eax,byte [fr_lvl+esi+ebx]
 cvtsi2ss xmm0,eax
 mulss xmm0,[c1o255]
 movss [tmpf],xmm0                    ; level target
 movss xmm1,[g_L+ebx*4]
 movss xmm2,xmm0
 subss xmm2,xmm1
 movss xmm3,xmm7
 comiss xmm2,[c0]
 jbe .rl
 movss xmm3,xmm6
.rl:
 mulss xmm2,xmm3
 addss xmm1,xmm2
 movss [g_L+ebx*4],xmm1
 ; attack envelope: (1 - 0.04 age)^2
 movzx eax,byte [fr_age+esi+ebx]
 cvtsi2ss xmm0,eax
 addss xmm0,[g_ff]
 mulss xmm0,[c_hk]
 movss xmm2,[cone]
 subss xmm2,xmm0
 maxss xmm2,[c0]
 mulss xmm2,xmm2
 movss [g_H+ebx*4],xmm2
 ; smoothed pitch
 movss xmm0,[tmpf]
 comiss xmm0,[c_pt02]
 jbe .np
 movzx eax,byte [fr_pit+esi+ebx]
 cvtsi2ss xmm0,eax
 mulss xmm0,[c1o255]
 movss xmm1,[g_P+ebx*4]
 subss xmm0,xmm1
 movss xmm3,[g_DT]
 mulss xmm3,[c_pk]
 minss xmm3,[cone]
 mulss xmm0,xmm3
 addss xmm1,xmm0
 movss [g_P+ebx*4],xmm1
.np:
 ; percussion derived from the instrument played on the channel
 movzx eax,byte [fr_ins+esi+ebx]
 movss xmm2,[g_H+ebx*4]
 cmp eax,8
 je .kk
 cmp eax,7
 je .kk
 cmp eax,11
 je .ks
 cmp eax,6
 je .ks
 cmp eax,9
 je .kh
 jmp .kn
.kk:
 maxss xmm2,[g_KD]
 movss [g_KD],xmm2
 jmp .kn
.ks:
 maxss xmm2,[g_SN]
 movss [g_SN],xmm2
 jmp .kn
.kh:
 maxss xmm2,[g_HT]
 movss [g_HT],xmm2
.kn:
 inc ebx
 cmp ebx,4
 jb .c
 ; global energy and kick drum
 movss xmm0,[g_L]
 addss xmm0,[g_L+4]
 addss xmm0,[g_L+8]
 addss xmm0,[g_L+12]
 FC xmm1,0.25
 mulss xmm0,xmm1
 movss [g_E],xmm0
 mov eax,[g_KD]
 mov [g_K],eax
 ; musical time rotation: accumulated phase
 movss xmm0,[g_E]
 mulss xmm0,[c_spin]
 addss xmm0,[c_spin0]
 mulss xmm0,[g_DT]
 addss xmm0,[g_spin]
 movss [g_spin],xmm0
 ; row phase: look for the bounds of the current row in fr_pos
 mov ecx,[curf]
 movzx edx,word [fr_pos+ecx*2]
 mov eax,ecx
.ra:
 test eax,eax
 jz .rb0
 cmp word [fr_pos+eax*2-2],dx
 jne .rb0
 dec eax
 jmp .ra
.rb0:
 mov edi,ecx
.rb:
 inc edi
 cmp edi,[nfr]
 jae .rbe
 cmp word [fr_pos+edi*2],dx
 je .rb
.rbe:
 mov esi,edi
 sub esi,eax                           ; row length (frames)
 mov edi,ecx
 sub edi,eax                           ; elapsed
 cvtsi2ss xmm0,edi
 addss xmm0,[g_ff]
 cvtsi2ss xmm1,esi
 divss xmm0,xmm1
 minss xmm0,[cone]
 movss [tx_x],xmm0                     ; rowph
 and edx,63
 cvtsi2ss xmm1,edx
 addss xmm1,xmm0
 movss [tx_y],xmm1                     ; continuous row
 FC xmm2,0.015625
 mulss xmm1,xmm2
 movss [tx_z],xmm1                     ; patph
 ; ---- writing into vm_var ----
 SETV V_T,g_ST
 SETV V_TT,g_T
 SETV V_DT,g_DT
 SETV V_ROWPH,tx_x
 SETV V_ROW,tx_y
 SETV V_PATPH,tx_z
 SETV V_E,g_E
 SETV V_KICK,g_KD
 SETV V_SNARE,g_SN
 SETV V_HAT,g_HT
 SETV V_SPIN,g_spin
 SETV V_L0,g_L
 SETV V_L1,g_L+4
 SETV V_L2,g_L+8
 SETV V_L3,g_L+12
 SETV V_H0,g_H
 SETV V_H1,g_H+4
 SETV V_H2,g_H+8
 SETV V_H3,g_H+12
 SETV V_P0,g_P
 SETV V_P1,g_P+4
 SETV V_P2,g_P+8
 SETV V_P3,g_P+12
 ; scene progress
 movss xmm0,[g_ST]
 mulss xmm0,[g_dur]
 minss xmm0,[cone]
 movss [tx_x],xmm0
 SETV V_PROG,tx_x
 popad
 ret

; ---------- layer parameters ebp -> V_A0..V_A7 ----------
layer_params:
 pushad
 xor ecx,ecx
.p:
 lea eax,[ecx+ecx*2]
 lea edx,[ebp+LY_PRM+eax*4]
 movss xmm0,[edx]
 mov eax,[edx+4]
 test eax,eax
 js .np
 shl eax,4
 movss xmm1,[vm_var+eax]
 mulss xmm1,[edx+8]
 addss xmm0,xmm1
.np:
 shufps xmm0,xmm0,0
 mov eax,ecx
 shl eax,4
 movaps [vm_var+V_A0*16+eax],xmm0
 inc ecx
 cmp ecx,8
 jb .p
 popad
 ret

; ---------- compute lanes: u, v, w, r1, r2, idx of 4 points (group ecx) ----------
vm_setlanes:
 pushad
 mov [lp_g],ecx
 xor edi,edi                           ; lane
.l:
 mov eax,[lp_g]
 lea esi,[eax*4+edi]                   ; point index
 cvtsi2ss xmm0,esi
 movss [vm_var+V_IDX*16+edi*4],xmm0
 mov eax,[ebp+LY_DOM]
 cmp eax,DOM_GRID
 je .grid
 cmp eax,DOM_RAND
 je .rand
 ; line: u = (i + 0.5) / N
 addss xmm0,[chalf]
 mulss xmm0,[ebp+LY_INVN]
 movss [vm_var+V_U*16+edi*4],xmm0
 lea eax,[esi*8+esi]
 add eax,[ebp+LY_SEED]
 call hashf
 movss [vm_var+V_V*16+edi*4],xmm0
 jmp .rest
.grid:
 mov eax,esi
 xor edx,edx
 div dword [ebp+LY_NU]
 cvtsi2ss xmm0,edx
 addss xmm0,[chalf]
 mulss xmm0,[ebp+LY_INVNU]
 movss [vm_var+V_U*16+edi*4],xmm0
 cvtsi2ss xmm0,eax
 addss xmm0,[chalf]
 mulss xmm0,[ebp+LY_INVNV]
 movss [vm_var+V_V*16+edi*4],xmm0
 jmp .rest
.rand:
 lea eax,[esi*8+esi]
 add eax,[ebp+LY_SEED]
 add eax,101
 call hashf
 movss [vm_var+V_U*16+edi*4],xmm0
 lea eax,[esi*8+esi]
 add eax,[ebp+LY_SEED]
 add eax,202
 call hashf
 movss [vm_var+V_V*16+edi*4],xmm0
.rest:
 lea eax,[esi*8+esi]
 add eax,[ebp+LY_SEED]
 add eax,303
 call hashf
 movss [vm_var+V_W*16+edi*4],xmm0
 lea eax,[esi*8+esi]
 add eax,[ebp+LY_SEED]
 add eax,404
 call hashf
 movss [vm_var+V_R1*16+edi*4],xmm0
 lea eax,[esi*8+esi]
 add eax,[ebp+LY_SEED]
 add eax,505
 call hashf
 movss [vm_var+V_R2*16+edi*4],xmm0
 inc edi
 cmp edi,4
 jb .l
 popad
 ret

; ---------- emit the 4 computed points: vertices and colors ----------
vm_emit:
 pushad
 mov edi,[lp_pv]
 movaps xmm0,[vm_var+V_X*16]
 movaps [tx_i],xmm0
 movaps xmm0,[vm_var+V_Y*16]
 movaps [tx_j],xmm0
 movaps xmm0,[vm_var+V_Z*16]
 movaps [tx_x],xmm0
 xor ecx,ecx
.v:
 mov eax,[tx_i+ecx*4]
 mov [edi],eax
 mov eax,[tx_j+ecx*4]
 mov [edi+4],eax
 mov eax,[tx_x+ecx*4]
 mov [edi+8],eax
 add edi,12
 inc ecx
 cmp ecx,4
 jb .v
 mov [lp_pv],edi
 ; colors: palette(C) with alpha = B * gain
 movaps xmm0,[vm_var+V_C*16]
 movss xmm4,[c_cf]
 shufps xmm4,xmm4,0
 mulps xmm0,xmm4
 movss xmm4,[c_cb]
 shufps xmm4,xmm4,0
 addps xmm0,xmm4
 movss xmm2,[c255]
 shufps xmm2,xmm2,0
 mulps xmm0,xmm2
 minps xmm0,xmm2
 maxps xmm0,[vc_zero]
 cvttps2dq xmm0,xmm0
 movdqa [tx_i],xmm0
 movaps xmm1,[vm_var+V_B*16]
 movss xmm3,[ebp+LY_GAIN]
 mulss xmm3,[c_bright]
 shufps xmm3,xmm3,0
 mulps xmm1,xmm3
 mulps xmm1,xmm2
 maxps xmm1,[vc_zero]
 minps xmm1,xmm2
 cvttps2dq xmm1,xmm1
 movdqa [tx_j],xmm1
 mov esi,[ebp+LY_PALP]
 mov edi,[lp_pc]
 xor ecx,ecx
.c:
 mov eax,[tx_i+ecx*4]
 mov edx,[esi+eax*4]
 and edx,0x00FFFFFF
 mov eax,[tx_j+ecx*4]
 shl eax,24
 or edx,eax
 mov [edi],edx
 add edi,4
 inc ecx
 cmp ecx,4
 jb .c
 mov [lp_pc],edi
 popad
 ret

; ---------- one pass (0 = init, 1 = frame) on layer ebp ----------
layer_pass:
 pushad
 mov [lp_ly],ebp
 mov eax,[ebp+LY_KIND]
 test eax,eax
 jz near .ret
 cmp eax,2
 je near .native
 mov esi,[ebp+LY_INIT+ecx*4]
 test esi,esi
 jz near .ret
 mov [lp_prog],esi
 mov [lp_mode],ecx
 mov eax,[ebp+LY_N]
 add eax,3
 shr eax,2
 mov [lp_ng],eax
 mov eax,[ebp+LY_VTX]
 mov [lp_pv],eax
 mov eax,[ebp+LY_COL]
 mov [lp_pc],eax
 mov dword [lp_c],0
.g:
 mov ebp,[lp_ly]
 mov ecx,[lp_c]
 call vm_setlanes
 mov eax,[lp_c]
 shl eax,7
 add eax,[ebp+LY_STATE]
 mov [lp_sp],eax
 cmp dword [lp_mode],0
 jne .ld
 xorps xmm0,xmm0
 xor edi,edi
.zz:
 movaps [vm_var+V_S0*16+edi],xmm0
 add edi,16
 cmp edi,128
 jb .zz
 jmp .run
.ld:
 mov edx,[lp_sp]
 xor edi,edi
.ll:
 movaps xmm0,[edx+edi]
 movaps [vm_var+V_S0*16+edi],xmm0
 add edi,16
 cmp edi,128
 jb .ll
.run:
 mov esi,[lp_prog]
 call vm_run
 mov edx,[lp_sp]
 xor edi,edi
.sl:
 movaps xmm0,[vm_var+V_S0*16+edi]
 movaps [edx+edi],xmm0
 add edi,16
 cmp edi,128
 jb .sl
.em:
 mov ebp,[lp_ly]
 call vm_emit
 inc dword [lp_c]
 mov eax,[lp_c]
 cmp eax,[lp_ng]
 jb .g
 jmp .ret
.native:
 test ecx,ecx
 jnz .nf
 call [ebp+LY_INIT]
 jmp .ret
.nf:
 call [ebp+LY_FRAME]
.ret:
 popad
 ret

; ---------- brightens a 256-entry palette (edi): components x2 (saturating) ----------
pal_lift:
 pushad
 mov ebp,256
.p:
 mov eax,[edi]
 mov esi,eax
 and esi,0xFF000000
 xor ebx,ebx
 xor ecx,ecx
.c:
 movzx edx,al
 add edx,edx
 cmp edx,255
 jbe .o
 mov edx,255
.o:
 shl edx,cl
 or ebx,edx
 shr eax,8
 add ecx,8
 cmp ecx,24
 jb .c
 or ebx,esi
 mov [edi],ebx
 add edi,4
 dec ebp
 jnz .p
 popad
 ret
