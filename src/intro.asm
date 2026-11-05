; ============================================================
;  PARTICLE SYSTEM - demo engine in x86 assembly (NASM, Windows 32 bits)
;  Music: ADPCM track embedded in the exe (see core/mus.inc),
;  decoded by the mixer written here.
;  Visuals: only GL points (GL_POINTS) computed by an SSE
;  virtual machine (core/vm.inc) from physical formulas.
;  Sync: the audio playback position drives every frame.
;  coder: Ivandoe
; ============================================================
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

%define NBUF  8                 ; ring of audio buffers
%define BUFN  4096              ; stereo frames per buffer
%define BUFB  (BUFN*4)

extern _ExitProcess@4
extern _GetSystemMetrics@4
extern _CreateWindowExA@48
extern _GetDC@4
extern _ShowCursor@4
extern _PeekMessageA@20
extern _DispatchMessageA@4
extern _GetAsyncKeyState@4
extern _ChoosePixelFormat@8
extern _SetPixelFormat@12
extern _SwapBuffers@4
extern _wglCreateContext@4
extern _wglMakeCurrent@8
extern _wglGetProcAddress@4
extern _glViewport@16
extern _glEnable@4
extern _glBlendFunc@8
extern _glClear@4
extern _glMatrixMode@4
extern _glLoadMatrixf@4
extern _glLoadIdentity@0
extern _glTranslatef@12
extern _glRotatef@16
extern _glPointSize@4
extern _glEnableClientState@4
extern _glVertexPointer@16
extern _glColorPointer@16
extern _glDrawArrays@12
extern _glBegin@4
extern _glEnd@0
extern _glVertex2f@8
extern _glColor4f@16
extern _waveOutOpen@24
extern _waveOutPrepareHeader@12
extern _waveOutWrite@12
extern _waveOutGetPosition@12
extern _Sleep@4
extern _glReadPixels@28
extern _glPixelStorei@8
extern _CreateFileA@28
extern _WriteFile@20
extern _CloseHandle@4
extern _CreatePipe@16
extern _SetHandleInformation@12
extern _CreateProcessA@40
extern _WaitForSingleObject@8

section .data
st_class db "static",0
x_wav    db "demo_music.wav",0
x_log    db "demo_ffmpeg.log",0
x_c1     db "ffmpeg -y -f rawvideo -pix_fmt rgb24 -s ",0
x_c2     db " -r ",0
x_c3     db " -i - -i demo_music.wav -vf vflip -c:v libx264 -preset medium -crf ",0
x_c4     db " -pix_fmt yuv420p -c:a aac -b:a 192k -shortest demo.mp4",0
align 4
wavh:   db "RIFF"
        dd 0
        db "WAVEfmt "
        dd 16
        dw 1,2
        dd 44100,176400
        dw 4,16
        db "data"
        dd 0
n_ppf    db "glPointParameterfv",0
n_ppf1   db "glPointParameterf",0
align 4
pfd:    dw 40,1
        dd 0x25
        db 0,32
        times 30 db 0
wfx:    dw 1,2                          ; PCM, stereo
        dd 44100,176400
        dw 4,16,0
proj:   dd 0.0,0.0,0.0,0.0
        dd 0.0,1.7320508,0.0,0.0
        dd 0.0,0.0,-1.0005003,-1.0
        dd 0.0,0.0,-0.2000500,0.0
inv720  dd 0.00138889
c_glow  dd 2.2                          ; halo: point size x2.9, alpha x0.3
; attenuation: size = ref_size * PTREF / distance  (c = 1/PTREF^2)
att     dd 0.0, 0.0, 0.027777778
c_one   dd 1.0

section .bss
alignb 16
scr_w   resd 1
scr_h   resd 1
hdc     resd 1
hwo     resd 1
mmt     resd 4
msg     resd 12
col2    resd MAXP               ; attenuated colors for the halo pass
glppf   resd 1                  ; glPointParameterfv (may be NULL)
glppf1  resd 1                  ; glPointParameterf
whdr    resd 8*NBUF
pcm     resb BUFB*NBUF
endsmp  resd 1                  ; end sample (frames * 882 + margin)
lastr   resd 1                  ; sample of the last drawn frame (30/60 fps limit)
spfr    resd 1                  ; audio samples per video frame (44100 / fps)
cbase   resd 1
syoff   resd 1                  ; audio sync offset (samples): left / right arrows
dsel    resd 1                  ; color array used by draw_split
nwr     resd 1
hpipe   resd 1
hrd     resd 1
hlog    resd 1
hwav    resd 1
exi     resd 1                  ; current frame / number of export frames
exn     resd 1
sa      resd 3                  ; SECURITY_ATTRIBUTES
sinfo   resd 17                 ; STARTUPINFO
pinfo   resd 4                  ; PROCESS_INFORMATION
cmdl    resb 512
rawbuf  resb 2560*1440*3

%include "setup.inc"
%include "postfx.inc"

section .text

global _start, start
_start:
start:
 call _SetProcessDPIAware@0     ; real pixels (no Windows scaling)
 call setup_show                ; settings window (Run / Quit)
 call mod_init
 call mod_prepass
 call scenes_init
 call mod_reset
 imul eax,[nfr],SPF
 add eax,44100                  ; 1 s tail after the last frame
 mov [endsmp],eax
 ; ---------- fullscreen window + OpenGL context ----------
 call setup_mode               ; chosen size, display mode change if fullscreen
 mov dword [rect],0
 mov dword [rect+4],0
 mov eax,[cfg_w]
 mov [rect+8],eax
 mov eax,[cfg_h]
 mov [rect+12],eax
 mov esi,0x90000000             ; fullscreen: POPUP|VISIBLE
 cmp dword [cfg_fs],0
 jne .cw
 cmp dword [cfg_exp],0
 je .win
 mov dword [cfg_x],0            ; export: borderless window at (0,0), exact size
 mov dword [cfg_y],0
 jmp .cw
.win:
 mov esi,0x10C80000             ; window: CAPTION|SYSMENU|VISIBLE
 mov dword [cfg_x],0x80000000   ; CW_USEDEFAULT
 mov dword [cfg_y],0x80000000
 push 0
 push esi
 push rect
 call _AdjustWindowRect@12
.cw:
 mov eax,[rect+12]
 sub eax,[rect+4]
 mov ebx,[rect+8]
 sub ebx,[rect]
 push 0
 push 0
 push 0
 push 0
 push eax
 push ebx
 push dword [cfg_y]
 push dword [cfg_x]
 push esi
 push 0
 push st_class
 push 0
 call _CreateWindowExA@48
 mov [cfg_hwnd],eax
 mov ebx,[cfg_w]                ; drawing size (client area in windowed mode)
 mov [scr_w],ebx
 mov ebx,[cfg_h]
 mov [scr_h],ebx
 cmp dword [cfg_fs],0
 jne .fsz
 push rect
 push eax
 call _GetClientRect@8
 mov eax,[rect+8]
 mov [scr_w],eax
 mov eax,[rect+12]
 mov [scr_h],eax
.fsz:
 push dword [cfg_hwnd]
 call _GetDC@4
 mov [hdc],eax
 push pfd
 push dword [hdc]
 call _ChoosePixelFormat@8
 push pfd
 push eax
 push dword [hdc]
 call _SetPixelFormat@12
 push dword [hdc]
 call _wglCreateContext@4
 push eax
 push dword [hdc]
 call _wglMakeCurrent@8
 cmp dword [cfg_fs],0
 je .cur
 push 0
 call _ShowCursor@4
.cur:
 push dword [scr_h]
 push dword [scr_w]
 push 0
 push 0
 call _glViewport@16
 push 0x0BE2
 call _glEnable@4
 push 0x0B10
 call _glEnable@4
 push 1
 push 0x302
 call _glBlendFunc@8
 push 0x8074
 call _glEnableClientState@4
 push 0x8076
 call _glEnableClientState@4
 push vtx
 push 0
 push 0x1406
 push 3
 call _glVertexPointer@16
 push col
 push 0
 push 0x1401
 push 4
 call _glColorPointer@16
 ; ---------- point attenuation with distance (GL 1.4 extension) ----------
 push n_ppf
 call _wglGetProcAddress@4
 mov [glppf],eax
 push n_ppf1
 call _wglGetProcAddress@4
 mov [glppf1],eax
 cmp dword [glppf],0
 je .noatt
 push att
 push 0x8129                    ; GL_POINT_DISTANCE_ATTENUATION
 call dword [glppf]
.noatt:
 cmp dword [glppf1],0
 je .nomin
 push dword [c_one]
 push 0x8126                    ; GL_POINT_SIZE_MIN
 call dword [glppf1]
.nomin:
 call postfx_init                ; fullscreen effects (shader), if available
 ; projection matrix: f / aspect
 cvtsi2ss xmm0,dword [scr_w]
 cvtsi2ss xmm1,dword [scr_h]
 movss xmm2,[proj+20]
 mulss xmm2,xmm1
 divss xmm2,xmm0
 movss [proj],xmm2
 mov dword [lastr],0
 mov eax,44100                  ; samples per video frame
 xor edx,edx
 div dword [cfg_fps]
 mov [spfr],eax
 cmp dword [cfg_exp],0
 jne export_run
 ; ---------- audio: ring of NBUF buffers filled on the fly ----------
 push 0
 push 0
 push 0
 push wfx
 push -1
 push hwo
 call _waveOutOpen@24
 xor ebx,ebx
.ab:
 mov eax,ebx
 imul eax,BUFB
 add eax,pcm
 mov edx,ebx
 shl edx,5
 mov [whdr+edx],eax             ; lpData
 mov dword [whdr+edx+4],BUFB    ; dwBufferLength
 push BUFN
 push eax
 call mod_mix
 add esp,8
 call mute_buf
 mov eax,ebx
 shl eax,5
 add eax,whdr
 push 32
 push eax
 push dword [hwo]
 call _waveOutPrepareHeader@12
 push 32
 mov eax,ebx
 shl eax,5
 add eax,whdr
 push eax
 push dword [hwo]
 call _waveOutWrite@12
 inc ebx
 cmp ebx,NBUF
 jb .ab
 mov dword [lastk],-1
 mov dword [g_K],0
 mov dword [g_E],0
 mov dword [tprev],0

.frame:
 push dword [cfg_hwnd]       ; window closed (cross): quit
 call _IsWindow@4
 test eax,eax
 jz .quit
 push 1
 push 0
 push 0
 push 0
 push msg
 call _PeekMessageA@20
 push msg
 call _DispatchMessageA@4
 push 0x1B
 call _GetAsyncKeyState@4
 test ax,ax
 js .quit
 push 0x25                        ; left arrow: later frames, right arrow: earlier frames (sync adjustment)
 call _GetAsyncKeyState@4
 test ax,ax
 jns .nkl
 sub dword [syoff],44
.nkl:
 push 0x27
 call _GetAsyncKeyState@4
 test ax,ax
 jns .nkr
 add dword [syoff],44
.nkr:
 ; ---------- refills the finished buffers ----------
 xor ebx,ebx
.rf:
 mov edx,ebx
 shl edx,5
 test dword [whdr+edx+16],1     ; WHDR_DONE
 jz .rn
 mov eax,ebx
 imul eax,BUFB
 add eax,pcm
 push BUFN
 push eax
 call mod_mix
 add esp,8
 call mute_buf
 mov eax,ebx
 shl eax,5
 add eax,whdr
 push 32
 push eax
 push dword [hwo]
 call _waveOutWrite@12
.rn:
 inc ebx
 cmp ebx,NBUF
 jb .rf
 ; audio playback position (samples)
 mov dword [mmt],2              ; TIME_SAMPLES
 push 12
 push mmt
 push dword [hwo]
 call _waveOutGetPosition@12
 mov eax,[mmt+4]
 cmp dword [mmt],4              ; the driver answered in bytes
 jne .ok
 shr eax,2                      ; 16-bit stereo: 4 bytes per sample
.ok:
 add eax,[syoff]                  ; fine sync adjustment (arrow keys)
 jns .syp
 xor eax,eax
.syp:
 cmp eax,[endsmp]
 jae .quit
 mov edx,eax                    ; limited to 30 or 60 frames/s
 sub edx,[lastr]
 mov ecx,[spfr]
 sub ecx,60
 cmp edx,ecx
 jb .wait
 mov [lastr],eax
 mov [samp],eax
 call frame_update               ; scene, camera, morphing, fade
 call draw_frame
 push dword [hdc]
 call _SwapBuffers@4
 jmp .frame
.wait:
 push 1
 call _Sleep@4
 jmp .frame
.quit:
 push 0                         ; restores the display mode
 push 0
 call _ChangeDisplaySettingsA@8
 push 0
 call _ExitProcess@4

; ---------- draws one frame (input: state from frame_update) ----------
draw_frame:
 push 0x4000
 call _glClear@4
 push vtx
 push 0
 push 0x1406
 push 3
 call _glVertexPointer@16
 push 0x1701
 call _glMatrixMode@4
 push proj
 call _glLoadMatrixf@4
 push 0x1700
 call _glMatrixMode@4
 call _glLoadIdentity@0
 mov eax,[cam_d]
 xor eax,0x80000000
 push eax
 push 0
 push 0
 call _glTranslatef@12
 push 0x3F800000
 push 0
 push 0
 push dword [cam_roll]
 call _glRotatef@16
 push 0
 push 0
 push 0x3F800000
 push dword [cam_rx]
 call _glRotatef@16
 push 0
 push 0x3F800000
 push 0
 push dword [cam_ry]
 call _glRotatef@16
 ; halo colors: alpha * 0.3
 mov ecx,[npts]
 test ecx,ecx
 jz .nopts
 mov eax,[mg_n]                   ; size of the background group in vtx
 cmp dword [mv_on],0
 je .spl
 mov eax,[mo_n0]
.spl:
 cmp eax,ecx
 jbe .spl2
 mov eax,ecx
.spl2:
 mov [vsplit],eax
 mov esi,col
 mov edi,col2
.c2:
 mov eax,[esi]
 mov edx,eax
 and edx,0x00FFFFFF
 shr eax,24
 imul eax,eax,46
 shr eax,8
 shl eax,24
 or eax,edx
 mov [edi],eax
 add esi,4
 add edi,4
 dec ecx
 jnz .c2
 cmp dword [fx_on],0
 je .one
 mov dword [dsel],1             ; different effects: background and objects are drawn separately
 call draw_passes
 xor eax,eax
 call postfx_grab               ; background -> left half of the texture
 push 0x4000
 call _glClear@4
 mov dword [dsel],2
 call draw_passes
 mov eax,[scr_w]
 call postfx_grab               ; objects -> right half
 jmp .nopts
.one:
 mov dword [dsel],0
 call draw_passes
.nopts:
 ; ---------- black fade veil: identity matrices ----------
 push 0x1701
 call _glMatrixMode@4
 call _glLoadIdentity@0
 push 0x1700
 call _glMatrixMode@4
 call _glLoadIdentity@0
 call postfx_draw                ; zoom blur, glow, pixelation
 movss xmm0,[fade]
 comiss xmm0,[inv255]
 jb .nofade
 push 0x303
 push 0x302
 call _glBlendFunc@8
 push dword [fade]
 push 0
 push 0
 push 0
 call _glColor4f@16
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
 push 1
 push 0x302
 call _glBlendFunc@8
.nofade:
 ret

; ---------- view matrix from [esi] = rx, ry, d, roll ----------
setmv:
 call _glLoadIdentity@0
 mov eax,[esi+8]
 xor eax,0x80000000
 push eax
 push 0
 push 0
 call _glTranslatef@12
 push 0x3F800000
 push 0
 push 0
 push dword [esi+12]
 call _glRotatef@16
 push 0
 push 0
 push 0x3F800000
 push dword [esi]
 call _glRotatef@16
 push 0
 push 0x3F800000
 push 0
 push dword [esi+4]
 call _glRotatef@16
 ret

; ---------- two-step drawing: background (background camera) then objects (cinematic camera) ----------
; quality: one point out of 2^q_sh is drawn (vertex array stride), cbase = colors
draw_split:
 push esi
 cmp dword [dsel],2
 jae .o
 mov esi,cam_brx
 call setmv
 xor eax,eax                    ; start
 mov edx,[vsplit]               ; count
 call draw_range
.o:
 cmp dword [dsel],1
 je .f
 mov esi,cam_rx
 call setmv
 mov eax,[vsplit]
 mov edx,[npts]
 sub edx,eax
 jbe .f
 call draw_range
.f:
 pop esi
 ret

; eax = first point, edx = number of points
draw_range:

 push ebx
 mov ecx,[q_sh]
 shr edx,cl                     ; number of points drawn
 test edx,edx
 jz .fin
 mov ebx,eax
 lea eax,[eax+eax*2]
 lea eax,[vtx+eax*4]            ; vtx + start*12
 mov [tmpf+8],eax
 lea eax,[ebx*4]
 add eax,[cbase]
 mov [tmpf+12],eax              ; cbase + start*4
 push edx
 push dword [tmpf+8]
 mov eax,12
 shl eax,cl
 push eax                       ; vertex stride
 push 0x1406
 push 3
 call _glVertexPointer@16
 mov eax,4
 shl eax,cl
 push dword [tmpf+12]
 push eax                       ; color stride
 push 0x1401
 push 4
 call _glColorPointer@16
 pop edx
 push edx
 push 0
 push 0
 call _glDrawArrays@12
.fin:
 pop ebx
 ret

; ---------- MP4 export: frame by frame (time = frame number / fps), music as WAV, ffmpeg through a pipe ----------
; appends the string esi to edi
putstr:
 mov al,[esi]
 test al,al
 jz .e
 mov [edi],al
 inc esi
 inc edi
 jmp putstr
.e:
 ret
; appends the number eax (decimal) to edi
putnum:
 push ebx
 mov ebx,10
 xor ecx,ecx
.d:
 xor edx,edx
 div ebx
 add dl,'0'
 push edx
 inc ecx
 test eax,eax
 jnz .d
.w:
 pop edx
 mov [edi],dl
 inc edi
 loop .w
 pop ebx
 ret

export_run:
 ; ---------- 1) full music -> demo_music.wav ----------
 push 0
 push 0x80
 push 2                         ; CREATE_ALWAYS
 push 0
 push 0
 push 0x40000000                ; GENERIC_WRITE
 push x_wav
 call _CreateFileA@28
 mov [hwav],eax
 mov ecx,[endsmp]
 shl ecx,2
 mov [wavh+40],ecx
 add ecx,36
 mov [wavh+4],ecx
 push 0
 push nwr
 push 44
 push wavh
 push dword [hwav]
 call _WriteFile@20
 mov ebx,[endsmp]
.wl:
 mov esi,BUFN
 cmp ebx,esi
 jae .wn
 mov esi,ebx
.wn:
 push esi
 push pcm
 call mod_mix
 add esp,8
 push 0
 push nwr
 lea eax,[esi*4]
 push eax
 push pcm
 push dword [hwav]
 call _WriteFile@20
 sub ebx,esi
 jnz .wl
 push dword [hwav]
 call _CloseHandle@4
 ; ---------- 2) number of frames ----------
 mov eax,[endsmp]
 xor edx,edx
 div dword [spfr]
 mov [exn],eax
 ; ---------- 3) ffmpeg command line ----------
 mov edi,cmdl
 mov esi,x_c1
 call putstr
 mov eax,[scr_w]
 call putnum
 mov byte [edi],'x'
 inc edi
 mov eax,[scr_h]
 call putnum
 mov esi,x_c2
 call putstr
 mov eax,[cfg_fps]
 call putnum
 mov esi,x_c3
 call putstr
 mov ecx,[cfg_q]
 mov eax,[crf_tab+ecx*4]
 call putnum
 mov esi,x_c4
 call putstr
 mov byte [edi],0
 ; ---------- 4) pipe to ffmpeg ----------
 mov dword [sa],12
 mov dword [sa+4],0
 mov dword [sa+8],1             ; inheritable handles
 push 0
 push sa
 push hpipe
 push hrd
 call _CreatePipe@16            ; hrd = read end (ffmpeg), hpipe = write end (ours)
 push 0
 push 1
 push dword [hpipe]
 call _SetHandleInformation@12  ; our end is not inherited
 push 0
 push 0x80
 push 2
 push sa
 push 1                         ; FILE_SHARE_READ
 push 0x40000000
 push x_log
 call _CreateFileA@28
 mov [hlog],eax
 mov edi,sinfo
 xor eax,eax
 mov ecx,17
 rep stosd
 mov dword [sinfo],68
 mov dword [sinfo+44],0x100     ; STARTF_USESTDHANDLES
 mov eax,[hrd]
 mov [sinfo+56],eax
 mov eax,[hlog]
 mov [sinfo+60],eax
 mov [sinfo+64],eax
 push pinfo
 push sinfo
 push 0
 push 0
 push 0x08000000                ; CREATE_NO_WINDOW
 push 1                         ; handle inheritance
 push 0
 push 0
 push cmdl
 push 0
 call _CreateProcessA@40
 test eax,eax
 jnz .ffok
 push 0x10
 push msg_ttl
 push msg_noff
 push 0
 call _MessageBoxA@16
 jmp start.quit
.ffok:
 push dword [hrd]
 call _CloseHandle@4
 push 1
 push 0x0D05                    ; GL_PACK_ALIGNMENT
 call _glPixelStorei@8
 mov dword [lastk],-1
 mov dword [g_K],0
 mov dword [g_E],0
 mov dword [tprev],0
 mov dword [exi],0
 ; ---------- 5) frames ----------
.fr:
 mov eax,[exi]
 cmp eax,[exn]
 jae .fin
 push 1
 push 0
 push 0
 push 0
 push msg
 call _PeekMessageA@20
 push msg
 call _DispatchMessageA@4
 push 0x1B
 call _GetAsyncKeyState@4
 test ax,ax
 js .fin
 mov eax,[exi]
 imul eax,[spfr]
 mov [samp],eax
 call frame_update
 call draw_frame
 push rawbuf
 push 0x1401
 push 0x1907                    ; GL_RGB
 push dword [scr_h]
 push dword [scr_w]
 push 0
 push 0
 call _glReadPixels@28
 mov eax,[scr_w]
 imul eax,[scr_h]
 lea eax,[eax+eax*2]
 push 0
 push nwr
 push eax
 push rawbuf
 push dword [hpipe]
 call _WriteFile@20
 test eax,eax
 jz .fin                        ; ffmpeg stopped
 push dword [hdc]
 call _SwapBuffers@4
 inc dword [exi]
 jmp .fr
.fin:
 push dword [hpipe]
 call _CloseHandle@4            ; end of stream: ffmpeg finishes the MP4
 push -1
 push dword [pinfo]
 call _WaitForSingleObject@8
 push 0x40
 push msg_ttl
 push msg_done
 push 0
 call _MessageBoxA@16
 jmp start.quit

; halo then core of the points; [dsel]: 0 all, 1 background only, 2 objects only
draw_passes:
 cvtsi2ss xmm0,dword [scr_h]
 mulss xmm0,[inv720]
 mulss xmm0,[fx_ps]
 mulss xmm0,[q_ps]               ; low quality: bigger points (fewer of them)
 movss [tmpf],xmm0
 mulss xmm0,[c_glow]
 movss [tmpf+4],xmm0
 mov dword [cbase],col2
 push dword [tmpf+4]
 call _glPointSize@4
 call draw_split
 mov dword [cbase],col
 push dword [tmpf]
 call _glPointSize@4
 call draw_split
 ret
