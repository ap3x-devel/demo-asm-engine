; Linux test bench: runs the REAL frame logic and exports the points
bits 32
section .text align=16
section .data align=16
section .bss  align=16
%include "core/mus.inc"
%include "core/fx_common.inc"
%include "core/vm.inc"
%include "core/morph.inc"
%include "core/engine.inc"
%include "core/scenes.inc"
%include "core/frame.inc"

section .bss
args    resd 4
hdr     resd 16
section .text
atoi:
 xor eax,eax
.l:
 movzx ecx,byte [esi]
 sub ecx,'0'
 cmp ecx,9
 ja .d
 imul eax,eax,10
 add eax,ecx
 inc esi
 jmp .l
.d:
 ret
wr:                             ; ecx=ptr edx=len
 mov eax,4
 mov ebx,1
 int 0x80
 ret
global _start
_start:
 mov ebp,esp
 mov esi,[ebp+8]
 call atoi
 mov [args],eax                 ; start frame
 mov esi,[ebp+12]
 call atoi
 mov [args+4],eax               ; end frame
 mov esi,[ebp+16]
 call atoi
 mov [args+8],eax               ; export step
 call mod_init
 call mod_prepass
 call scenes_init
 mov dword [lastk],-1
 mov dword [tprev],0
 mov ebx,[args]
.f:
 imul eax,ebx,SPF
 add eax,441
 mov [samp],eax
 mov [args+12],ebx
 call frame_update
 mov ebx,[args+12]
 mov eax,ebx
 sub eax,[args]
 xor edx,edx
 div dword [args+8]
 test edx,edx
 jz .exp
 cmp ebx,[args+4]
 jne .nx
.exp:
 mov [hdr],ebx
 mov eax,[npts]
 mov [hdr+4],eax
 mov eax,[cam_rx]
 mov [hdr+8],eax
 mov eax,[cam_ry]
 mov [hdr+12],eax
 mov eax,[cam_d]
 mov [hdr+16],eax
 mov eax,[fx_ps]
 mov [hdr+20],eax
 mov eax,[fade]
 mov [hdr+24],eax
 mov eax,[kscene]
 mov [hdr+28],eax
 mov eax,[mg_n]
 cmp dword [mv_on],0
 je .sp
 mov eax,[mo_n0]
.sp:
 mov [hdr+32],eax
 mov eax,[cam_brx]
 mov [hdr+36],eax
 mov eax,[cam_bry]
 mov [hdr+40],eax
 mov eax,[cam_bd]
 mov [hdr+44],eax
 mov eax,[cam_roll]
 mov [hdr+48],eax
 mov eax,[cin_or]
 mov [hdr+52],eax
 mov eax,[cin_tx]
 mov [hdr+56],eax
 mov eax,[g_DT]
 mov [hdr+60],eax
 mov ecx,hdr
 mov edx,64
 call wr
 mov ecx,vtx
 mov edx,[npts]
 lea edx,[edx+edx*2]
 shl edx,2
 call wr
 mov ecx,col
 mov edx,[npts]
 shl edx,2
 call wr
.nx:
 mov ebx,[args+12]
 inc ebx
 cmp ebx,[args+4]
 jbe .f
 mov eax,1
 xor ebx,ebx
 int 0x80
