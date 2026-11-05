; ============================================================
;  title.inc - particle texts (native layer, kind 2)
;  5x7 font; each point aims at a text pixel, starts from a scattered
;  cloud, swirls, then gathers (delayed appearance from
;  left to right), sparkles and breathes on the kick drum.
;  coder: Ivandoe
; ============================================================
%define TT_MAX 32768

section .data
align 16
tt_letters db "ACDEIJKLMNOPRSTUVWY3@",0x80,0
tt_glyph:                               ; 7 rows of 5 bits per letter
 db 01110b,10001b,10001b,11111b,10001b,10001b,10001b   ; A
 db 01110b,10001b,10000b,10000b,10000b,10001b,01110b   ; C
 db 11110b,10001b,10001b,10001b,10001b,10001b,11110b   ; D
 db 11111b,10000b,10000b,11110b,10000b,10000b,11111b   ; E
 db 01110b,00100b,00100b,00100b,00100b,00100b,01110b   ; I
 db 00111b,00010b,00010b,00010b,00010b,10010b,01100b   ; J
 db 10001b,10010b,10100b,11000b,10100b,10010b,10001b   ; K
 db 10000b,10000b,10000b,10000b,10000b,10000b,11111b   ; L
 db 10001b,11011b,10101b,10101b,10001b,10001b,10001b   ; M
 db 10001b,11001b,10101b,10011b,10001b,10001b,10001b   ; N
 db 01110b,10001b,10001b,10001b,10001b,10001b,01110b   ; O
 db 11110b,10001b,10001b,11110b,10000b,10000b,10000b   ; P
 db 11110b,10001b,10001b,11110b,10100b,10010b,10001b   ; R
 db 01111b,10000b,10000b,01110b,00001b,00001b,11110b   ; S
 db 11111b,00100b,00100b,00100b,00100b,00100b,00100b   ; T
 db 10001b,10001b,10001b,10001b,10001b,10001b,01110b   ; U
 db 10001b,10001b,10001b,10001b,10001b,01010b,00100b   ; V
 db 10001b,10001b,10001b,10101b,10101b,11011b,10001b   ; W
 db 10001b,10001b,01010b,00100b,00100b,00100b,00100b   ; Y
 db 01110b,10001b,00001b,00110b,00001b,10001b,01110b   ; 3
 db 01110b,10001b,10111b,10101b,10110b,10000b,01110b   ; @
 db 00111b,01000b,11110b,01000b,11110b,01000b,00111b   ; euro
tt_s0 db "PARTICLE SYSTEM",0
tt_s1 db "IVANDOE",0
tt_s2 db "AP3D",0x80,"M@K",0
tt_c1 db "CODER",0
tt_c2 db "MUSIC",0
align 4
; text: main line, caption, main pitch, main y, caption pitch, caption y
tt_tab:
 dd tt_s0,0,       __float32__(0.115),__float32__(0.0),   0,0
 dd tt_s1,tt_c1,   __float32__(0.180),__float32__(-0.55), __float32__(0.075),__float32__(1.35)
 dd tt_s2,tt_c2,   __float32__(0.170),__float32__(-0.55), __float32__(0.075),__float32__(1.35)
tt_mhalf dd -0.5
tt_c95   dd 0.95
tt_c08   dd 0.8
tt_c18   dd 18.0
tt_c12   dd 12.0
tt_c055  dd 0.55
tt_c045  dd 0.45
tt_c006  dd 0.06
tt_c16   dd 1.0
tt_i22   dd 0.625
tt_c24   dd 2.4
tt_c25   dd 2.5
tt_c60   dd 60.0
tt_c012  dd 0.012
tt_c035  dd 0.35
tt_c04   dd 0.4
tt_c025  dd 0.25
tt_c075  dd 0.75
tt_c15   dd 15.0
tt_c6    dd 6.0
tt_c10   dd 10.0
tt_c005  dd 0.05
tt_c003  dd 0.03

section .bss
alignb 16
tt_org  resd 3*TT_MAX
tt_tgt  resd 3*TT_MAX
tt_dl   resd TT_MAX
tt_px   resd 3*1024
tt_n    resd 1
tt_L    resd 1
tt_pitch resd 1
tt_yc   resd 1
tt_x0   resd 1
tt_t    resd 1
tt_br   resd 1
tt_a    resd 1
tt_cs   resd 1
tt_sn   resd 1

section .text
; ---------- one text line: esi = string, [tt_pitch], [tt_yc] -> pixels in tt_px ----------
tt_line:
 pushad
 xor ecx,ecx
.len:
 cmp byte [esi+ecx],0
 je .lok
 inc ecx
 jmp .len
.lok:
 mov [tt_L],ecx
 lea eax,[ecx*2+ecx]
 add eax,eax
 dec eax
 cvtsi2ss xmm0,eax
 mulss xmm0,[tt_pitch]
 mulss xmm0,[tt_mhalf]
 movss [tt_x0],xmm0
 xor ebx,ebx
.ch:
 cmp ebx,[tt_L]
 jae .done
 movzx eax,byte [esi+ebx]
 xor edx,edx
.f:
 cmp byte [tt_letters+edx],0
 je .next
 cmp [tt_letters+edx],al
 je .g
 inc edx
 jmp .f
.g:
 imul edx,edx,7
 xor ebp,ebp
.row:
 movzx eax,byte [tt_glyph+edx+ebp]
 xor edi,edi
.col:
 mov ecx,4
 sub ecx,edi
 bt eax,ecx
 jnc .skip
 lea ecx,[ebx*2+ebx]
 add ecx,ecx
 add ecx,edi
 cvtsi2ss xmm0,ecx
 addss xmm0,[chalf]
 mulss xmm0,[tt_pitch]
 addss xmm0,[tt_x0]
 mov ecx,3
 sub ecx,ebp
 cvtsi2ss xmm1,ecx
 mulss xmm1,[tt_pitch]
 addss xmm1,[tt_yc]
 mov ecx,[tt_n]
 cmp ecx,1023
 jae .skip
 lea ecx,[ecx*2+ecx]
 movss [tt_px+ecx*4],xmm0
 movss [tt_px+ecx*4+4],xmm1
 movss xmm2,[tt_pitch]
 movss [tt_px+ecx*4+8],xmm2
 inc dword [tt_n]
.skip:
 inc edi
 cmp edi,5
 jb .col
 inc ebp
 cmp ebp,7
 jb .row
.next:
 inc ebx
 jmp .ch
.done:
 popad
 ret

; ---------- init: ebp = layer, seed = text number ----------
txt_init:
 pushad
 mov dword [tt_n],0
 mov eax,[ebp+LY_SEED]
 imul eax,eax,24
 lea edi,[tt_tab+eax]
 mov esi,[edi]
 mov eax,[edi+8]
 mov [tt_pitch],eax
 mov eax,[edi+12]
 mov [tt_yc],eax
 call tt_line
 mov esi,[edi+4]
 test esi,esi
 jz .nocap
 mov eax,[edi+16]
 mov [tt_pitch],eax
 mov eax,[edi+20]
 mov [tt_yc],eax
 call tt_line
.nocap:
 xor ebx,ebx
.pt:
 cmp ebx,[ebp+LY_N]
 jae near .end
 mov eax,ebx
 xor edx,edx
 div dword [tt_n]
 imul edx,edx,12
 ; target: pixel + jitter
 lea eax,[ebx*8+ebx]
 add eax,11
 call hashf
 subss xmm0,[chalf]
 mulss xmm0,[tt_px+edx+8]
 mulss xmm0,[tt_c95]
 addss xmm0,[tt_px+edx]
 movss xmm3,xmm0
 lea eax,[ebx*8+ebx]
 add eax,22
 call hashf
 subss xmm0,[chalf]
 mulss xmm0,[tt_px+edx+8]
 mulss xmm0,[tt_c95]
 addss xmm0,[tt_px+edx+4]
 movss xmm4,xmm0
 lea eax,[ebx*8+ebx]
 add eax,33
 call hashf
 subss xmm0,[chalf]
 mulss xmm0,[tt_px+edx+8]
 mulss xmm0,[tt_c08]
 movss xmm5,xmm0
 lea ecx,[ebx*2+ebx]
 movss [tt_tgt+ecx*4],xmm3
 movss [tt_tgt+ecx*4+4],xmm4
 movss [tt_tgt+ecx*4+8],xmm5
 ; origin: scattered cloud
 lea eax,[ebx*8+ebx]
 add eax,44
 call hashf
 subss xmm0,[chalf]
 mulss xmm0,[tt_c18]
 movss xmm5,xmm0
 lea eax,[ebx*8+ebx]
 add eax,55
 call hashf
 subss xmm0,[chalf]
 mulss xmm0,[tt_c12]
 movss xmm6,xmm0
 lea eax,[ebx*8+ebx]
 add eax,66
 call hashf
 subss xmm0,[chalf]
 mulss xmm0,[tt_c18]
 lea ecx,[ebx*2+ebx]
 movss [tt_org+ecx*4+8],xmm0
 movss [tt_org+ecx*4],xmm5
 movss [tt_org+ecx*4+4],xmm6
 ; delay: a bit random + left-to-right sweep
 lea eax,[ebx*8+ebx]
 add eax,77
 call hashf
 mulss xmm0,[tt_c055]
 mulss xmm3,[tt_c006]
 addss xmm3,[chalf]
 maxss xmm3,[c0]
 minss xmm3,[cone]
 mulss xmm3,[tt_c045]
 addss xmm0,xmm3
 movss [tt_dl+ebx*4],xmm0
 inc ebx
 jmp .pt
.end:
 popad
 ret

; ---------- frame: ebp = layer ----------
txt_frame:
 pushad
 movss xmm0,[g_ST]
 movss [tt_t],xmm0
 movss xmm1,[g_H]
 mulss xmm1,[tt_c005]
 movss xmm2,[g_L]
 mulss xmm2,[tt_c003]
 addss xmm1,xmm2
 addss xmm1,[cone]
 movss xmm1,[cone]                  ; stable text: no more breathing on the music
 movss [tt_br],xmm1
 mov esi,[ebp+LY_VTX]
 mov edi,[ebp+LY_COL]
 xor ebx,ebx
.p:
 lea ecx,[ebx*2+ebx]
 ; s = clamp((t - 1.6 d) / 2.2) ; e = smootherstep(s)
 movss xmm0,[tt_t]
 movss xmm1,[tt_dl+ebx*4]
 mulss xmm1,[tt_c16]
 subss xmm0,xmm1
 mulss xmm0,[tt_i22]
 maxss xmm0,[c0]
 minss xmm0,[cone]
 movss xmm1,xmm0
 mulss xmm1,[tt_c6]
 subss xmm1,[tt_c15]
 mulss xmm1,xmm0
 addss xmm1,[tt_c10]
 mulss xmm1,xmm0
 mulss xmm1,xmm0
 mulss xmm1,xmm0
 movss xmm0,xmm1                          ; e
 ; position = org + (target - org) * e
 movss xmm1,[tt_tgt+ecx*4]
 movss xmm2,[tt_org+ecx*4]
 subss xmm1,xmm2
 mulss xmm1,xmm0
 addss xmm1,xmm2
 movss xmm3,[tt_tgt+ecx*4+4]
 movss xmm2,[tt_org+ecx*4+4]
 subss xmm3,xmm2
 mulss xmm3,xmm0
 addss xmm3,xmm2
 movss xmm4,[tt_tgt+ecx*4+8]
 movss xmm2,[tt_org+ecx*4+8]
 subss xmm4,xmm2
 mulss xmm4,xmm0
 addss xmm4,xmm2
 ; swirl around the y axis: angle (1-e)*2.4
 movss xmm5,[cone]
 subss xmm5,xmm0
 mulss xmm5,[tt_c24]
 movss [tt_a],xmm5
 fld dword [tt_a]
 fsincos
 fstp dword [tt_cs]
 fstp dword [tt_sn]
 movss xmm5,[tt_cs]
 movss xmm6,[tt_sn]
 movss xmm7,xmm1
 mulss xmm7,xmm5
 movss xmm2,xmm4
 mulss xmm2,xmm6
 addss xmm7,xmm2                          ; x'
 mulss xmm4,xmm5
 mulss xmm1,xmm6
 subss xmm4,xmm1                          ; z'
 movss xmm1,xmm7
 ; sparkle once placed
 movss xmm5,[tt_t]
 mulss xmm5,[tt_c25]
 movss xmm6,[tt_dl+ebx*4]
 mulss xmm6,[tt_c60]
 addss xmm5,xmm6
 movss [tt_a],xmm5
 fld dword [tt_a]
 fsincos
 fstp dword [tt_cs]
 fstp dword [tt_sn]
 movss xmm5,xmm0
 mulss xmm5,[tt_c012]
 movss xmm6,[tt_sn]
 mulss xmm6,xmm5
 addss xmm1,xmm6
 movss xmm6,[tt_cs]
 mulss xmm6,xmm5
 addss xmm3,xmm6
 ; breathing on the music
 movss xmm5,[tt_br]
 mulss xmm1,xmm5
 mulss xmm3,xmm5
 mulss xmm4,xmm5
 movss [esi],xmm1
 movss [esi+4],xmm3
 movss [esi+8],xmm4
 add esi,12
 ; color: gradient depending on x and point
 movss xmm1,[tt_tgt+ecx*4]
 mulss xmm1,[tt_c006]
 addss xmm1,[chalf]
 maxss xmm1,[c0]
 minss xmm1,[cone]
 mulss xmm1,[tt_c025]
 movss xmm2,[tt_dl+ebx*4]
 mulss xmm2,[tt_c04]
 addss xmm1,xmm2
 addss xmm1,[tt_c035]
 minss xmm1,[cone]
 mulss xmm1,[c255]
 cvttss2si eax,xmm1
 mov edx,[ebp+LY_PALP]
 mov eax,[edx+eax*4]
 and eax,0x00FFFFFF
 ; alpha: rises with e
 mulss xmm0,[tt_c075]
 addss xmm0,[tt_c025]
 mulss xmm0,[ebp+LY_GAIN]
 mulss xmm0,[c255]
 mulss xmm0,[c_bright]
 minss xmm0,[c255]
 cvttss2si edx,xmm0
 shl edx,24
 or eax,edx
 mov [edi],eax
 add edi,4
 inc ebx
 cmp ebx,[ebp+LY_N]
 jb .p
 popad
 ret
section .data
