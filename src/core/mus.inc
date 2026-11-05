; ============================================================
;  mus.inc - music playback and analysis tables
;  The track is stored as stereo 4-bit IMA ADPCM (1 byte per
;  stereo sample: left | right<<4, 44100 Hz) and decoded on the
;  fly in mod_mix. The 50 Hz tables (level, attack age, pitch,
;  instrument, row position) come from tools_aud/audana:
;  bass / mid / treble / total bands, tracked tempo and bars.
;  coder: Ivandoe
; ============================================================
%include "track.inc"
%define RATE   44100
%define SPF    882

global fr_lvl, fr_age, fr_pit, fr_ins, fr_pos, nfr, mod_init, mod_prepass, mod_mix, mod_reset

section .data
align 4
nfr     dd TRK_NFR
fr_lvl  incbin "t_lvl.bin"
fr_age  incbin "t_age.bin"
fr_pit  incbin "t_pit.bin"
fr_ins  incbin "t_ins.bin"
fr_pos  incbin "t_pos.bin"
align 4
%include "adp_tab.inc"
trk     incbin "track.adp"
section .bss
tk_pos  resd 1
tk_n    resd 1
ad_pl   resd 1
ad_il   resd 1
ad_pr   resd 1
ad_ir   resd 1
section .text

mod_init:
mod_prepass:
 ret
mod_reset:
 xor eax,eax
 mov [tk_pos],eax
 mov [ad_pl],eax
 mov [ad_il],eax
 mov [ad_pr],eax
 mov [ad_ir],eax
 ret

; one channel: edx = nibble (0..15), %1 = predictor, %2 = index; result in eax
%macro adch 2
 mov ecx,[%2]
 movsx eax,word [adp_step+ecx*2]
 mov ebx,eax
 shr ebx,3
 test edx,4
 jz %%a
 add ebx,eax
%%a:
 test edx,2
 jz %%b
 mov ebp,eax
 shr ebp,1
 add ebx,ebp
%%b:
 test edx,1
 jz %%c
 mov ebp,eax
 shr ebp,2
 add ebx,ebp
%%c:
 mov eax,[%1]
 test edx,8
 jz %%p
 sub eax,ebx
 jmp %%q
%%p:
 add eax,ebx
%%q:
 cmp eax,32767
 jle %%u
 mov eax,32767
%%u:
 cmp eax,-32768
 jge %%v
 mov eax,-32768
%%v:
 mov [%1],eax
 mov ebp,edx
 and ebp,7
 movsx ebp,byte [adp_idx+ebp]
 add ecx,ebp
 jns %%w
 xor ecx,ecx
%%w:
 cmp ecx,88
 jle %%x
 mov ecx,88
%%x:
 mov [%2],ecx
%endmacro

; mod_mix(buffer, number of stereo samples)
mod_mix:
 pushad
 mov edi,[esp+36]
 mov eax,[esp+40]
 mov [tk_n],eax
 mov esi,[tk_pos]
.l:
 cmp dword [tk_n],0
 je .d
 cmp esi,TRK_NFR*SPF
 jae .z
 movzx edx,byte [trk+esi]
 push edx
 and edx,15
 adch ad_pl,ad_il
 mov [edi],ax
 pop edx
 shr edx,4
 adch ad_pr,ad_ir
 mov [edi+2],ax
 jmp .n
.z:
 mov dword [edi],0
.n:
 add edi,4
 inc esi
 dec dword [tk_n]
 jmp .l
.d:
 mov [tk_pos],esi
 popad
 ret
