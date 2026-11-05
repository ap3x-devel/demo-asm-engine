; ============================================================
;  postfx.inc - fullscreen post-processing (GLSL 1.10 shader)
;  effects ported from the Cgeffects zip:
;    - circular / zoom blur (Blur_circular2): pulses on the kick drum
;    - glow (Blur256): soft halo around the particles
;    - pixelation (pixelate2) and posterization (posterize2): at the
;      start of each scene the image "resolves" from big blocks to sharp
;  If the driver has no shaders, or compilation fails,
;  the effect is simply disabled (the demo stays the same).
;  coder: Ivandoe
; ============================================================
extern _glGenTextures@8
extern _glBindTexture@8
extern _glTexImage2D@36
extern _glCopyTexSubImage2D@32
extern _glTexParameteri@12
extern _glDisable@4

section .data
align 4
fx_names:
 db "glCreateShader",0
 db "glShaderSource",0
 db "glCompileShader",0
 db "glCreateProgram",0
 db "glAttachShader",0
 db "glLinkProgram",0
 db "glUseProgram",0
 db "glGetUniformLocation",0
 db "glUniform2f",0
 db "glUniform4f",0
 db "glGetProgramiv",0
 db 0
fx_u_t  db "t",0
fx_u_r  db "r",0
fx_u_a  db "a",0
fx_u_b  db "b",0
fx_u_c  db "c",0
%include "postfx_src.inc"
%include "fxtab.inc"
fx_srcp dd fx_src
fx_h225 dd 0.5
fx_c24  dd 24.0
fx_i045 dd 2.2222222
fx_i07  dd 1.4285714
fx_c5   dd 5.0
fx_c79  dd 79.0
fx_z    dd 0.07
fx_b0   dd 0.50
fx_b1   dd 0.40
fx_one  dd 1.0
fx_s03  dd 0.30
fx_i06  dd 1.6666667
fx_zero dd 0.0

section .bss
alignb 4
fx_on   resd 1
fx_tex  resd 1
fx_prog resd 1
fx_la   resd 1
fx_lb   resd 1
fx_lc   resd 1
fx_c    resd 4
fx_b    resd 4
fx_lr   resd 1
fx_lt   resd 1
fx_ok   resd 1
fx_fn   resd 11                 ; GL function pointers (see fx_names)
fx_a    resd 4
fx_rf   resd 2

%define FX_CSH 0
%define FX_SSRC 1
%define FX_COMP 2
%define FX_CPRG 3
%define FX_ATT 4
%define FX_LINK 5
%define FX_USE 6
%define FX_GUL 7
%define FX_U2F 8
%define FX_U4F 9
%define FX_GPIV 10

section .text
; ---------- initialization (GL context already active; cfg_fx = 1) ----------
postfx_init:
 pushad
 mov dword [fx_on],0
 cmp dword [cfg_fx],0
 je .ret
 mov esi,fx_names
 xor ebx,ebx
.n:
 push esi
 call _wglGetProcAddress@4
 test eax,eax
 jz .ret                        ; missing function: no shader
 mov [fx_fn+ebx*4],eax
.sk:
 cmp byte [esi],0
 lea esi,[esi+1]
 jne .sk
 inc ebx
 cmp ebx,11
 jb .n
 ; capture texture (screen size)
 push fx_tex
 push 1
 call _glGenTextures@8
 push dword [fx_tex]
 push 0x0DE1
 call _glBindTexture@8
 push 0x2601
 push 0x2800
 push 0x0DE1
 call _glTexParameteri@12
 push 0x2601
 push 0x2801
 push 0x0DE1
 call _glTexParameteri@12
 push 0x812F
 push 0x2802
 push 0x0DE1
 call _glTexParameteri@12
 push 0x812F
 push 0x2803
 push 0x0DE1
 call _glTexParameteri@12
 push 0
 push 0x1401
 push 0x1907
 push 0
 push dword [scr_h]
 mov eax,[scr_w]
 add eax,eax
 push eax
 push 0x1907
 push 0
 push 0x0DE1
 call _glTexImage2D@36
 ; program
 push 0x8B30                    ; GL_FRAGMENT_SHADER
 call dword [fx_fn+FX_CSH*4]
 mov edi,eax
 push 0
 push fx_srcp
 push 1
 push edi
 call dword [fx_fn+FX_SSRC*4]
 push edi
 call dword [fx_fn+FX_COMP*4]
 call dword [fx_fn+FX_CPRG*4]
 mov [fx_prog],eax
 push edi
 push eax
 call dword [fx_fn+FX_ATT*4]
 push dword [fx_prog]
 call dword [fx_fn+FX_LINK*4]
 push fx_ok
 push 0x8B82                    ; GL_LINK_STATUS
 push dword [fx_prog]
 call dword [fx_fn+FX_GPIV*4]
 cmp dword [fx_ok],0
 je .ret                        ; compilation / link failure
 push fx_u_t
 push dword [fx_prog]
 call dword [fx_fn+FX_GUL*4]
 mov [fx_lt],eax
 push fx_u_r
 push dword [fx_prog]
 call dword [fx_fn+FX_GUL*4]
 mov [fx_lr],eax
 push fx_u_a
 push dword [fx_prog]
 call dword [fx_fn+FX_GUL*4]
 mov [fx_la],eax
 push fx_u_b
 push dword [fx_prog]
 call dword [fx_fn+FX_GUL*4]
 mov [fx_lb],eax
 push fx_u_c
 push dword [fx_prog]
 call dword [fx_fn+FX_GUL*4]
 mov [fx_lc],eax
 mov dword [fx_on],1
.ret:
 popad
 ret

; ---------- applies the effect on the drawn image (before the fade veil) ----------
postfx_draw:
 cmp dword [fx_on],0
 je .ret
 ; parameters: zoom (kick), glow, posterization, pixelation
 movss xmm0,[g_K]
 movaps xmm1,xmm0
 mulss xmm1,[fx_z]
 movss [fx_a],xmm1
 mulss xmm0,[fx_b1]
 addss xmm0,[fx_b0]
 movss [fx_a+4],xmm0
 movss xmm0,[g_ST]
 movaps xmm1,xmm0
 mulss xmm1,[fx_i07]            ; posterization for 0.7 s
 comiss xmm1,[fx_one]
 jae .l0
 mulss xmm1,xmm1
 mulss xmm1,[fx_c79]
 addss xmm1,[fx_c5]
 jmp .l1
.l0:
 xorps xmm1,xmm1
.l1:
 movss [fx_a+8],xmm1
 movaps xmm1,xmm0
 mulss xmm1,[fx_i045]           ; pixelation for 0.45 s
 comiss xmm1,[fx_one]
 jae .p0
 mulss xmm1,xmm1
 cvtsi2ss xmm2,dword [scr_w]
 mulss xmm2,[fx_h225]
 mulss xmm1,xmm2
 addss xmm1,[fx_c24]
 jmp .p1
.p0:
 xorps xmm1,xmm1
.p1:
 movss [fx_a+12],xmm1
 ; b: ripple + twist (snare), halftone before the end of the scene, time
 movss xmm0,[g_SN]
 movss [fx_b],xmm0
 mulss xmm0,[fx_s03]
 movss [fx_b+8],xmm0
 movss xmm0,[g_T]
 movss [fx_b+12],xmm0
 movss xmm0,[g_K]               ; b.y = kick impulse
 movss [fx_b+4],xmm0
 ; image capture
 push 0x0BE2
 call _glDisable@4
 push dword [fx_tex]
 push 0x0DE1
 call _glBindTexture@8
 push 0x4000                    ; clear: the final image is entirely redrawn by the shader
 call _glClear@4
 push dword [fx_prog]
 call dword [fx_fn+FX_USE*4]
 cvtsi2ss xmm0,dword [scr_w]
 cvtsi2ss xmm1,dword [scr_h]
 movss [fx_rf],xmm0
 movss [fx_rf+4],xmm1
 push dword [fx_rf+4]
 push dword [fx_rf]
 push dword [fx_lr]
 call dword [fx_fn+FX_U2F*4]
 push dword [fx_a+12]
 push dword [fx_a+8]
 push dword [fx_a+4]
 push dword [fx_a]
 push dword [fx_la]
 call dword [fx_fn+FX_U4F*4]
 push dword [fx_b+12]
 push dword [fx_b+8]
 push dword [fx_b+4]
 push dword [fx_b]
 push dword [fx_lb]
 call dword [fx_fn+FX_U4F*4]
 mov ecx,[kscene]                ; scene-specific effect (table fxtab.inc)
 cmp ecx,SCN_N
 jb .kc
 xor ecx,ecx
.kc:
 shl ecx,4
 movups xmm0,[fx_tab+ecx]
 movups [fx_c],xmm0
 push dword [fx_c+12]
 push dword [fx_c+8]
 push dword [fx_c+4]
 push dword [fx_c]
 push dword [fx_lc]
 call dword [fx_fn+FX_U4F*4]
 push 7
 call _glBegin@4
 push 0xBF800000
 push 0xBF800000
 call _glVertex2f@8
 push 0xBF800000
 push 0x3F800000
 call _glVertex2f@8
 push 0x3F800000
 push 0x3F800000
 call _glVertex2f@8
 push 0x3F800000
 push 0xBF800000
 call _glVertex2f@8
 call _glEnd@0
 push 0
 call dword [fx_fn+FX_USE*4]
 push 0x0BE2
 call _glEnable@4
.ret:
 ret

; ---------- captures the drawn image into the texture (eax = x offset: 0 background, scr_w objects) ----------
postfx_grab:
 cmp dword [fx_on],0
 je .r
 push ebx
 mov ebx,eax
 push dword [fx_tex]
 push 0x0DE1
 call _glBindTexture@8
 push dword [scr_h]
 push dword [scr_w]
 push 0
 push 0
 push 0
 push ebx
 push 0
 push 0x0DE1
 call _glCopyTexSubImage2D@32
 pop ebx
.r:
 ret
