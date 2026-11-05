; ============================================================
;  morph.inc - morphing transitions, by GROUPS
;
;  The points of a scene form two groups: 0 = background landscape,
;  1 = main object. When the scene changes, the last frame of the outgoing
;  scene is kept (A, with its velocity). During MDUR seconds each
;  point of the incoming scene is pulled from the matching point of the
;  old one, group by group (landscape becomes landscape, object becomes
;  object): pairing by rank in Morton order (counting sort):
;      p = A + inertia + (B - A) * e(s) + dispersion * sin(pi s)
;  e = smootherstep(s), s = t / MDUR. Colors and camera are blended the same way.
; ============================================================
section .data
align 16
mo_inv  dd 0.3125               ; 1 / MDUR (set per scene)
mo_vmax dd 0.045
mo_vmin dd -0.045
mo_dec  dd -0.058894            ; log2(0.96): inertia damping
mo_gain dd 25.0                 ; 1 / (1 - 0.96)
mo_amp  dd 0.45                 ; amplitude of the dispersion cloud
mo_rot  dd 1.3                  ; swirl (rad) during the morph
mo_c3   dd 3.0
mo_kq   dd 0.7111111            ; 64 cells / 90 units  (domain -45..45)
mo_off  dd 45.0
mo_zoom dd 1.6
mo_fps  dd 50.0
mo_c6   dd 6.0
mo_c15  dd 15.0
mo_c10  dd 10.0
mo_c360 dd 360.0
mo_i360 dd 0.0027777778
mo_ir   dd 0.0416667            ; 1 / 24 (radius -> 0..1)
mo_hy   dd 6.0
mo_ih   dd 0.0833333            ; 1 / 12 (height -> 0..1)
align 16
; transition styles: swirl, dispersion, dilation, geyser, camera pull-back,
;                        per-point delay, delay mode (0 none, 1 height, 2 radius), 1 unused
mo_stab:
 dd 1.30, 0.45, 0.00, 0.0, 0.72, 0.00, 0, 0        ; 0: classic swirl + cloud
 dd 0.30, 0.20, 0.00, 0.0, 0.50, 0.45, 1, 0        ; 1: rising wave (bottom to top)
 dd 0.00, 0.12, 0.00, 0.0, 0.30, 0.50, 2, 0        ; 2: circular wave from the center
 dd 0.35, 0.20, 0.85, 0.0, 1.60, 0.00, 0, 0        ; 3: explosion
 dd -1.00,0.15,-0.70, 0.0, 0.10, 0.00, 0, 0        ; 4: implosion
 dd 0.00, 0.30, 0.00, 11.0,0.40, 0.30, 1, 0        ; 5: geyser
 dd -2.60,0.20, 0.00, 0.0, 0.90, 0.00, 0, 0        ; 6: fast counter-swirl
 dd 2.20, 0.10, 0.25, 0.0, 0.60, 0.40, 2, 0        ; 7: spiral + wave

section .bss
alignb 16
mv_A    resd 3*MAXP+16          ; last frame of the previous scene
mv_P    resd 3*MAXP+16          ; frame before that (for the velocity)
mv_T    resd 3*MAXP+16          ; copy of the incoming scene
mc_A    resd MAXP+16
mc_T    resd MAXP+16
mv_n    resd 1                  ; number of points of A
mv_on   resd 1
mo_n0   resd 1
mv_new  resd 1
mv_cam  resd 8                  ; rx, ry, d, point size
mg_n    resd 2                  ; group sizes of the current scene
mA_n    resd 2                  ; group sizes of A
mB_n    resd 2
mo_e    resd 1
mo_s    resd 1
mo_sw   resd 1
mo_g2   resd 1
mo_N    resd 1
mo_iA   resd 1
mo_iB   resd 1
mo_h    resd 4
mo_cs   resd 2
mo_rdy  resd 1
ms_src  resd 1
ms_n    resd 1
ms_dst  resd 1
ms_q    resd 3
mb_bA   resd 1
mb_bB   resd 1
mb_nA   resd 1
mb_nB   resd 1
mb_N    resd 1
mb_o    resd 1
mp_A    resd MAXP               ; sort permutation (Morton order) of A
mp_B    resd MAXP
mi_A    resd MAXP               ; pairing rank -> point of A
mi_B    resd MAXP
mp_key  resd MAXP
mp_cnt  resd 262144
fx_ps   resd 1
fx_t    resd 1
mo_ep   resd 1
mo_sp   resd 1
mo_sx   resd 1
mo_gyv  resd 1
mo_rs   resd 1
mo_zs   resd 1
mo_stk  resd 1
mo_stn  resd 1
mo_sty  resd 1

section .text

; ---------- quantization of one axis: eax = clamp(int((x + 45) * 64 / 90), 0, 63) ----------
%macro QAX 1
 movss xmm0,[esi+%1]
 addss xmm0,[mo_off]
 mulss xmm0,[mo_kq]
 cvttss2si eax,xmm0
 xor edx,edx
 test eax,eax
 cmovs eax,edx
 mov edx,63
 cmp eax,63
 cmova eax,edx
%endmacro

; ---------- Morton sort (6 bits per axis) by counting ----------
; inputs: ms_src (vertices), ms_n, ms_dst (output permutation)
msort:
 pushad
 mov edi,mp_cnt
 mov ecx,262144
 xor eax,eax
 rep stosd
 mov esi,[ms_src]
 mov ebx,[ms_n]
 xor ebp,ebp
 test ebx,ebx
 jz .fin
.k:
 QAX 0
 mov [ms_q],eax
 QAX 4
 mov [ms_q+4],eax
 QAX 8
 mov [ms_q+8],eax
 xor edx,edx
%assign b 0
%rep 6
 mov eax,[ms_q]
 shr eax,b
 and eax,1
 shl eax,3*b+2
 or edx,eax
 mov eax,[ms_q+4]
 shr eax,b
 and eax,1
 shl eax,3*b+1
 or edx,eax
 mov eax,[ms_q+8]
 shr eax,b
 and eax,1
 shl eax,3*b
 or edx,eax
%assign b b+1
%endrep
 mov [mp_key+ebp*4],edx
 inc dword [mp_cnt+edx*4]
 add esi,12
 inc ebp
 cmp ebp,ebx
 jb .k
 xor eax,eax
 xor ecx,ecx
.p:
 mov edx,[mp_cnt+ecx*4]
 mov [mp_cnt+ecx*4],eax
 add eax,edx
 inc ecx
 cmp ecx,262144
 jb .p
 mov edi,[ms_dst]
 xor ebp,ebp
.s:
 mov edx,[mp_key+ebp*4]
 mov eax,[mp_cnt+edx*4]
 inc dword [mp_cnt+edx*4]
 mov [edi+eax*4],ebp
 inc ebp
 cmp ebp,[ms_n]
 jb .s
.fin:
 popad
 ret

; builds mi_A / mi_B for the two groups: rank r -> point (r * n / N) in Morton order
mbuild:
 pushad
 mov dword [mb_o],0
 xor edi,edi                         ; group
.g:
 ; quantization scale of the group (background: +-128; object: +-6)
 test edi,edi
 jnz .qf
 mov dword [mo_off],__float32__(128.0)
 mov dword [mo_kq],__float32__(0.25)
 jmp .qd
.qf:
 mov dword [mo_off],__float32__(6.0)
 mov dword [mo_kq],__float32__(5.3333333)
.qd:
 ; --- A ---
 mov eax,[mA_n+edi*4]
 mov dword [mb_bA],0
 test edi,edi
 jz .a0
 mov edx,[mA_n]
 mov [mb_bA],edx
.a0:
 test eax,eax
 jnz .a1
 mov dword [mb_bA],0
 mov eax,[mv_n]                      ; empty group: draw from all of A
.a1:
 mov [mb_nA],eax
 ; --- B ---
 mov eax,[mB_n+edi*4]
 mov dword [mb_bB],0
 test edi,edi
 jz .b0
 mov edx,[mB_n]
 mov [mb_bB],edx
.b0:
 test eax,eax
 jnz .b1
 mov dword [mb_bB],0
 mov eax,[mo_N]
.b1:
 mov [mb_nB],eax
 cmp eax,[mb_nA]
 jae .n1
 mov eax,[mb_nA]
.n1:
 mov [mb_N],eax
 ; sort of A
 mov eax,[mb_bA]
 lea eax,[eax+eax*2]
 lea eax,[mv_A+eax*4]
 mov [ms_src],eax
 mov eax,[mb_nA]
 mov [ms_n],eax
 mov dword [ms_dst],mp_A
 call msort
 mov eax,[mb_bB]
 lea eax,[eax+eax*2]
 lea eax,[mv_T+eax*4]
 mov [ms_src],eax
 mov eax,[mb_nB]
 mov [ms_n],eax
 mov dword [ms_dst],mp_B
 call msort
 xor ebx,ebx
.r:
 mov eax,ebx
 mul dword [mb_nA]
 div dword [mb_N]
 mov eax,[mp_A+eax*4]
 add eax,[mb_bA]
 mov ecx,[mb_o]
 add ecx,ebx
 mov [mi_A+ecx*4],eax
 mov eax,ebx
 mul dword [mb_nB]
 div dword [mb_N]
 mov eax,[mp_B+eax*4]
 add eax,[mb_bB]
 mov [mi_B+ecx*4],eax
 inc ebx
 cmp ebx,[mb_N]
 jb .r
 mov eax,[mb_N]
 add [mb_o],eax
 inc edi
 cmp edi,2
 jb .g
 popad
 ret

; ---------- camera blend: cam = snap + (cam - snap) e ----------
%macro CAMB 3                   ; %1 = snapshot index, %2 = variable, %3 = 1 if angle
 movss xmm0,[%2]
 subss xmm0,[mv_cam+%1*4]
%if %3
 movss xmm1,xmm0
 mulss xmm1,[mo_i360]
 roundss xmm1,xmm1,0
 mulss xmm1,[mo_c360]
 subss xmm0,xmm1
%endif
 mulss xmm0,[mo_e]
 addss xmm0,[mv_cam+%1*4]
 movss [%2],xmm0
%endmacro

; ---------- one point component ----------
%macro MORPHC 1
 movss xmm0,[mv_A+ecx*4+%1*4]
 movss xmm1,xmm0
 subss xmm1,[mv_P+ecx*4+%1*4]
 minss xmm1,[mo_vmax]
 maxss xmm1,[mo_vmin]
 mulss xmm1,[mo_g2]
 addss xmm0,xmm1                ; a = A + inertia
 movss xmm1,[mv_T+edx*4+%1*4]
 subss xmm1,xmm0
 mulss xmm1,[mo_ep]
 addss xmm0,xmm1                ; a + (b - a) e
 movss xmm2,[mo_h+%1*4]
 subss xmm2,[chalf]
 mulss xmm2,[mo_sw]
 addss xmm0,xmm2                ; + dispersion
 movss [edi+%1*4],xmm0
%endmacro

%macro CAMFIX 2                  ; shifts the start heading by a multiple of 360: shortest path, then blends without re-wrapping (no jump)
 movss xmm0,[%2]
 subss xmm0,[mv_cam+%1*4]
 movss xmm1,xmm0
 mulss xmm1,[mo_i360]
 roundss xmm1,xmm1,0
 mulss xmm1,[mo_c360]
 addss xmm1,[mv_cam+%1*4]
 movss [mv_cam+%1*4],xmm1
%endmacro
morph_frame:
 pushad
 cmp dword [mv_new],0
 je .chk
 mov dword [mv_new],0
 cmp dword [mv_n],0
 je .chk
 mov dword [mv_on],1
 mov dword [mo_rdy],0
 CAMFIX 1,cam_ry
 CAMFIX 5,cam_bry
.chk:
 cmp dword [mv_on],0
 je .save
 movss xmm0,[g_ST]
 mulss xmm0,[mo_inv]            ; s
 comiss xmm0,[cone]
 jae .done
 movss [mo_s],xmm0
 ; e = s^3 (s (6 s - 15) + 10)
 movss xmm1,xmm0
 mulss xmm1,[mo_c6]
 subss xmm1,[mo_c15]
 mulss xmm1,xmm0
 addss xmm1,[mo_c10]
 mulss xmm1,xmm0
 mulss xmm1,xmm0
 mulss xmm1,xmm0
 movss [mo_e],xmm1
 ; dispersion: amp sin(pi s)
 movss xmm0,[mo_s]
 mulss xmm0,[cpi]
 call sincos
 movss [mo_sp],xmm0             ; sin(pi s)
 ; transition style (belongs to the incoming scene): table mo_stab
 mov eax,[kscene]
 lea eax,[eax*4+eax]            ; *5
 and eax,7
 shl eax,5
 lea esi,[mo_stab+eax]
 movss xmm1,xmm0
 mulss xmm1,[esi+4]             ; dispersion amplitude
 movss [mo_sw],xmm1
 movss xmm1,xmm0
 mulss xmm1,[esi+8]             ; radial dilation / contraction
 movss [mo_sx],xmm1
 mulss xmm0,[esi+12]            ; vertical geyser
 movss [mo_gyv],xmm0
 movss xmm0,[esi]
 movss [mo_rs],xmm0             ; swirl speed
 movss xmm0,[esi+16]
 movss [mo_zs],xmm0             ; camera pull-back
 movss xmm0,[esi+20]
 movss [mo_stk],xmm0            ; maximum delay per point
 movss xmm1,[cone]
 subss xmm1,xmm0
 movss xmm0,[cone]
 divss xmm0,xmm1
 movss [mo_stn],xmm0
 mov eax,[esi+24]
 mov [mo_sty],eax               ; 0 none, 1 height, 2 radius
 ; inertia gain: 25 (1 - 0.96^f)  with f = elapsed frames
 movss xmm0,[g_ST]
 mulss xmm0,[mo_fps]
 mulss xmm0,[mo_dec]
 call fexp2
 movss xmm1,[cone]
 subss xmm1,xmm0
 mulss xmm1,[mo_gain]
 movss [mo_g2],xmm1
 ; copy of the incoming scene (B)
 mov ecx,[npts]
 mov esi,vtx
 mov edi,mv_T
 lea ecx,[ecx*3]
 rep movsd
 mov esi,col
 mov edi,mc_T
 mov ecx,[npts]
 rep movsd
 mov eax,[mg_n]
 mov [mB_n],eax
 mov eax,[mg_n+4]
 mov [mB_n+4],eax
 ; N per group = max(A, B) ; total
 xor edx,edx
 xor edi,edi
.nn:
 mov eax,[mA_n+edi*4]
 test eax,eax
 jnz .nn1
 mov eax,[mB_n+edi*4]
.nn1:
 cmp eax,[mB_n+edi*4]
 jae .nn2
 mov eax,[mB_n+edi*4]
.nn2:
 test edi,edi
 jnz .nn3
 mov [mo_n0],eax
.nn3:
 add edx,eax
 inc edi
 cmp edi,2
 jb .nn
 mov [mo_N],edx
 mov [npts],edx
 cmp dword [mo_rdy],0
 jne .rdy
 call mbuild
 mov dword [mo_rdy],1
.rdy:
 ; global swirl rotation: angle = mo_rot sin(pi s)
 movss xmm0,[mo_sp]
 mulss xmm0,[mo_rs]            ; swirl angle of the style
 call sincos
 movss [mo_cs],xmm1
 movss [mo_cs+4],xmm0
 mov edi,vtx
 mov esi,col
 xor ebx,ebx
.pt:
 lea eax,[ebx*3+5]
 call hashf
 movss [mo_h],xmm0
 lea eax,[ebx*3+6]
 call hashf
 movss [mo_h+4],xmm0
 lea eax,[ebx*3+7]
 call hashf
 movss [mo_h+8],xmm0
 mov eax,[mi_A+ebx*4]
 mov [mo_iA],eax
 lea ecx,[eax+eax*2]
 mov edx,[mi_B+ebx*4]
 mov [mo_iB],edx
 lea edx,[edx+edx*2]
 ; per-point easing: delay depending on the height / radius of the target (style)
 mov eax,[mo_sty]
 test eax,eax
 jz .nost
 movss xmm1,[mv_T+edx*4+4]
 cmp eax,1
 je .hgt
 movss xmm1,[mv_T+edx*4]
 mulss xmm1,xmm1
 movss xmm2,[mv_T+edx*4+8]
 mulss xmm2,xmm2
 addss xmm1,xmm2
 sqrtss xmm1,xmm1
 mulss xmm1,[mo_ir]
 jmp .gd
.hgt:
 addss xmm1,[mo_hy]
 mulss xmm1,[mo_ih]
.gd:
 maxss xmm1,[c0]
 minss xmm1,[cone]
 mulss xmm1,[mo_stk]
 movss xmm0,[mo_s]
 subss xmm0,xmm1
 mulss xmm0,[mo_stn]
 maxss xmm0,[c0]
 minss xmm0,[cone]
 movss xmm1,xmm0
 mulss xmm1,[mo_c6]
 subss xmm1,[mo_c15]
 mulss xmm1,xmm0
 addss xmm1,[mo_c10]
 mulss xmm1,xmm0
 mulss xmm1,xmm0
 mulss xmm1,xmm0
 movss [mo_ep],xmm1
 jmp .stdn
.nost:
 movss xmm1,[mo_e]
 movss [mo_ep],xmm1
.stdn:
 MORPHC 0
 MORPHC 1
 MORPHC 2
 ; radial dilation / contraction and vertical geyser of the style
 movss xmm0,[mo_sx]
 movss xmm1,[edi]
 mulss xmm1,xmm0
 addss xmm1,[edi]
 movss [edi],xmm1
 movss xmm1,[edi+4]
 mulss xmm1,xmm0
 addss xmm1,[edi+4]
 movss [edi+4],xmm1
 movss xmm1,[edi+8]
 mulss xmm1,xmm0
 addss xmm1,[edi+8]
 movss [edi+8],xmm1
 movss xmm1,[mo_h+4]
 mulss xmm1,[mo_gyv]
 addss xmm1,[edi+4]
 movss [edi+4],xmm1
 ; swirl around the vertical axis
 movss xmm0,[edi]
 movss xmm1,[edi+8]
 movss xmm2,xmm0
 mulss xmm2,[mo_cs]
 movss xmm3,xmm1
 mulss xmm3,[mo_cs+4]
 subss xmm2,xmm3
 movss [edi],xmm2
 mulss xmm0,[mo_cs+4]
 mulss xmm1,[mo_cs]
 addss xmm0,xmm1
 movss [edi+8],xmm0
 ; color
 mov eax,[mo_iA]
 movd xmm0,[mc_A+eax*4]
 mov edx,[mo_iB]
 movd xmm1,[mc_T+edx*4]
 pmovzxbd xmm0,xmm0
 pmovzxbd xmm1,xmm1
 cvtdq2ps xmm0,xmm0
 cvtdq2ps xmm1,xmm1
 subps xmm1,xmm0
 movss xmm2,[mo_ep]
 shufps xmm2,xmm2,0
 mulps xmm1,xmm2
 addps xmm0,xmm1
 cvttps2dq xmm0,xmm0
 packusdw xmm0,xmm0
 packuswb xmm0,xmm0
 movd [esi],xmm0
 add edi,12
 add esi,4
 inc ebx
 cmp ebx,[mo_N]
 jb .pt
 ; camera and point size
 CAMB 0,cam_rx,1
 CAMB 1,cam_ry,0
 CAMB 2,cam_d,0
 CAMB 4,cam_brx,1
 CAMB 5,cam_bry,0
 CAMB 6,cam_bd,0
 CAMB 3,fx_ps,0
 movss xmm0,[mo_sp]
 mulss xmm0,[mo_zs]
 addss xmm0,[cam_d]
 movss [cam_d],xmm0
 popad
 ret
.done:
 mov dword [mv_on],0
.save:
 ; P <- A ; A <- current scene
 mov esi,mv_A
 mov edi,mv_P
 mov ecx,[mv_n]
 lea ecx,[ecx*3]
 rep movsd
 mov esi,vtx
 mov edi,mv_A
 mov ecx,[npts]
 lea ecx,[ecx*3]
 rep movsd
 mov esi,col
 mov edi,mc_A
 mov ecx,[npts]
 rep movsd
 mov eax,[npts]
 mov [mv_n],eax
 mov eax,[mg_n]
 mov [mA_n],eax
 mov eax,[mg_n+4]
 mov [mA_n+4],eax
 mov eax,[cam_rx]
 mov [mv_cam],eax
 mov eax,[cam_ry]
 mov [mv_cam+4],eax
 mov eax,[cam_d]
 mov [mv_cam+8],eax
 mov eax,[fx_ps]
 mov [mv_cam+12],eax
 mov eax,[cam_brx]
 mov [mv_cam+16],eax
 mov eax,[cam_bry]
 mov [mv_cam+20],eax
 mov eax,[cam_bd]
 mov [mv_cam+24],eax
 popad
 ret
