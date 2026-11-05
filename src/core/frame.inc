; ============================================================
;  frame.inc - logic of one frame (independent from Windows/OpenGL)
;  input: [samp] = audio playback position (samples)
;  output: vtx/col/npts, camera, [fade]
; ============================================================
section .text

; ---------- derived from a layer (ebp) ----------
layer_setup:
 pushad
 mov eax,[ebp+LY_N]
 cvtsi2ss xmm0,eax
 movss xmm1,[cone]
 divss xmm1,xmm0
 movss [ebp+LY_INVN],xmm1
 mov ecx,[ebp+LY_NU]
 test ecx,ecx
 jnz .nu
 mov ecx,1
 mov [ebp+LY_NU],ecx
.nu:
 cvtsi2ss xmm2,ecx
 movss [ebp+LY_NUF],xmm2
 movss xmm1,[cone]
 divss xmm1,xmm2
 movss [ebp+LY_INVNU],xmm1
 mov eax,[ebp+LY_N]
 add eax,ecx
 dec eax
 xor edx,edx
 div ecx                               ; number of rows
 cvtsi2ss xmm3,eax
 movss xmm1,[cone]
 divss xmm1,xmm3
 movss [ebp+LY_INVNV],xmm1
 popad
 ret

; ---------- start of a scene: eax = index ----------
scene_begin:
 pushad
 imul esi,eax,SC_SIZE
 add esi,scn_tab
 movss xmm0,[cone]
 divss xmm0,[esi+SC_MORPH]
 movss [mo_inv],xmm0
 ; duration of the scene
 mov ecx,eax
 mov eax,[scn_f+ecx*4+4]
 sub eax,[scn_f+ecx*4]
 imul eax,eax,SPF
 cvtsi2ss xmm0,eax
 mulss xmm0,[inv44100]
 movss xmm1,[cone]
 divss xmm1,xmm0
 movss [g_dur],xmm1
 push esi
 lea esi,[esi+SC_CAM]
 mov edi,cam_cfg
 mov ecx,11
 rep movsd
 pop esi
 push esi
 lea esi,[esi+SC_BG]
 mov edi,lay_bg
 mov ecx,LY_SIZE/4
 rep movsd
 movss xmm0,[lay_bg+LY_GAIN]            ; brighter background
 mulss xmm0,[c_bgboost]
 movss [lay_bg+LY_GAIN],xmm0
 pop esi
 lea esi,[esi+SC_FG]
 mov edi,lay_fg
 mov ecx,LY_SIZE/4
 rep movsd
 mov ebp,lay_bg
 call layer_setup
 mov ebp,lay_fg
 call layer_setup
 mov eax,[lay_bg+LY_N]
 mov edx,[lay_fg+LY_N]
 mov [mg_n],eax
 mov [mg_n+4],edx
 mov dword [lay_bg+LY_VTX],vtx
 mov dword [lay_bg+LY_COL],col
 mov dword [lay_bg+LY_STATE],st_bg
 mov dword [lay_bg+LY_PALP],pal_bg
 lea ecx,[eax+eax*2]
 lea ecx,[vtx+ecx*4]
 mov [lay_fg+LY_VTX],ecx
 lea ecx,[col+eax*4]
 mov [lay_fg+LY_COL],ecx
 mov dword [lay_fg+LY_STATE],st_fg
 mov dword [lay_fg+LY_PALP],pal_fg
 add eax,edx
 mov [npts],eax
 mov eax,[lay_bg+LY_PAL]
 mov edi,pal_bg
 call mkgrad
 mov edi,pal_bg                     ; brighter ground
 call pal_lift
 mov eax,[lay_fg+LY_PAL]
 mov edi,pal_fg
 call mkgrad
 mov ebp,lay_bg
 call layer_params
 xor ecx,ecx
 call layer_pass
 mov ebp,lay_fg
 call layer_params
 xor ecx,ecx
 call layer_pass
 popad
 ret

; ---------- camera: slow oscillations + pulse on the kick drum ----------
%macro CAMF 4                   ; base amp w phase -> xmm0
 movss xmm0,[g_ST]
 mulss xmm0,[cam_cfg+%3*4]
 FC xmm1,%4
 addss xmm0,xmm1
 call sincos
 mulss xmm0,[cam_cfg+%2*4]
 addss xmm0,[cam_cfg+%1*4]
%endmacro
camera_update:
 pushad
 CAMF 3,4,5,0.0
 movss [cam_ry],xmm0
 CAMF 0,1,2,1.3
 movss [cam_rx],xmm0
 CAMF 6,7,8,2.1
 xorps xmm1,xmm1                     ; no more zoom on the music
 movss xmm2,[cone]
 subss xmm2,xmm1
 mulss xmm0,xmm2
 movss [cam_d],xmm0
 mov eax,[cam_rx]
 mov [cam_brx],eax
 mov eax,[cam_ry]
 mov [cam_bry],eax
 mov eax,[cam_d]
 mov [cam_bd],eax
 cmp dword [kscene],3
 jb .nocin
 movss xmm0,[g_ST]
 mulss xmm0,[g_dur]
 minss xmm0,[cone]
 xorps xmm1,xmm1
 maxss xmm0,xmm1
 movss [cin_p],xmm0
 movss xmm2,xmm0
 FC xmm1,0.5
 subss xmm2,xmm1
 FC xmm1,12.0
 mulss xmm2,xmm1
 addss xmm2,[cam_bry]
 movss [cam_bry],xmm2           ; slow lateral drift of the background
 movss xmm2,[cone]
 FC xmm1,0.14
 mulss xmm1,xmm0
 subss xmm2,xmm1
 FC xmm1,1.05
 mulss xmm2,xmm1
 mulss xmm2,[cam_bd]
 movss [cam_bd],xmm2            ; slow forward dolly of the background
%macro LDP 0                              ; esi = camera profile of the current scene
 mov eax,[kscene]
 imul eax,eax,48
 lea esi,[cam_prof+eax]
%endmacro
 LDP
 movss xmm0,[esi]                      ; base elevation specific to the scene
 mulss xmm0,[cmone]                     ; (sign inversion applied further down)
 movss [cam_rx],xmm0
 movss xmm0,[esi+4]                    ; base heading specific to the scene
 movss [cam_ry],xmm0
 movss xmm0,[esi+8]                    ; distance scale
 mulss xmm0,[cam_d]
 movss xmm1,[cin_dg]                   ; dolly punch on the kick
 addss xmm1,[cone]
 mulss xmm0,xmm1
 movss xmm1,[esi+36]                   ; slow push over the duration of the scene
 mulss xmm1,[cin_p]
 addss xmm1,[cone]
 mulss xmm0,xmm1
 movss [cam_d],xmm0
 movss xmm0,[cin_p]
 mulss xmm0,[cpi]
 call sincos
 mulss xmm0,[c0]                    ; no more continuous roll
 movss [cam_roll],xmm0
 ; ---- global camera: on every kick it moves (fast jump to a new viewpoint), then stays still ----
 movss xmm0,[g_K]
 movss xmm1,[cin_pk]
 movss [cin_pk],xmm0
 FC xmm2,0.6
 comiss xmm0,xmm2
 jb .nok
 FC xmm2,0.4
 comiss xmm1,xmm2
 ja .nok
 LDP
 call cin_rand                      ; heading jump amplitude: min + range*random (per scene)
 LDP
 mulss xmm0,[esi+16]
 addss xmm0,[esi+12]
 mulss xmm0,[cin_dir]
 addss xmm0,[cin_tx]
 movss [cin_tx],xmm0
 LDP
 movss xmm1,[esi+20]                ; direction mode: 0 random, +-1 constant, 2 alternating
 comiss xmm1,[c0]
 jne .m1
 call cin_rand
 FC xmm2,0.5
 comiss xmm0,xmm2
 movss xmm1,[cone]
 jae .sg
 movss xmm1,[cmone]
 jmp .sg
.m1:
 FC xmm2,2.0
 comiss xmm1,xmm2
 jne .sg
 movss xmm1,[cin_dir]
 mulss xmm1,[cmone]
.sg:
 movss [cin_dir],xmm1
 call cin_rand                      ; new elevation: +-amplitude of the scene
 FC xmm2,0.5
 subss xmm0,xmm2
 addss xmm0,xmm0
 LDP
 mulss xmm0,[esi+24]
 movss [cin_ty],xmm0
 movss xmm0,[cin_cd]                ; dolly punch alternating forward/backward
 mulss xmm0,[cmone]
 movss [cin_cd],xmm0
 mulss xmm0,[esi+28]
 movss [cin_dt],xmm0
.nok:
 LDP
 movss xmm3,[g_DT]                  ; glide (per-scene speed) then stop
 mulss xmm3,[esi+32]
 minss xmm3,[cone]
 movss xmm0,[cin_dt]
 subss xmm0,[cin_dg]
 mulss xmm0,xmm3
 addss xmm0,[cin_dg]
 movss [cin_dg],xmm0
 movss xmm0,[cin_tx]
 subss xmm0,[cin_or]
 mulss xmm0,xmm3
 addss xmm0,[cin_or]
 movss [cin_or],xmm0
 addss xmm0,[cam_ry]
 movss [cam_ry],xmm0
 movss xmm0,[cin_ty]
 subss xmm0,[cin_cx]
 mulss xmm0,xmm3
 addss xmm0,[cin_cx]
 movss [cin_cx],xmm0
 addss xmm0,[cam_rx]
 movss [cam_rx],xmm0
 jmp .cinok
.nocin:
 mov dword [cam_roll],0
 movss xmm0,[g_ST]
 mulss xmm0,[g_dur]
 minss xmm0,[cone]
 xorps xmm1,xmm1
 maxss xmm0,xmm1
 movaps xmm1,xmm0
 mulss xmm1,xmm0
 mulss xmm0,[ctwo]
 movss xmm2,[cthree]
 subss xmm2,xmm0
 mulss xmm1,xmm2                    ; e
 mov eax,[kscene]
 imul eax,eax,24
 lea esi,[tit_tab+eax]
%macro TIN 3
 movss xmm0,[esi+%2]
 subss xmm0,[esi+%1]
 mulss xmm0,xmm1
 addss xmm0,[esi+%1]
 movss [%3],xmm0
%endmacro
 TIN 0,4,cam_brx
 TIN 8,12,cam_bry
 TIN 16,20,cam_bd
 movss xmm0,[cone]
 FC xmm2,0.05
 mulss xmm2,xmm1
 subss xmm0,xmm2
 mulss xmm0,[cam_cfg+24]
 movss [cam_d],xmm0                 ; text: fixed camera, small dolly
 xor eax,eax
 mov [cam_rx],eax
 mov [cam_ry],eax
.cinok:
 cmp dword [kscene],3                  ; never too close to the objects: minimum distance 6.0
 jb .nod
 movss xmm0,[cam_d]
 FC xmm2,6.0
 maxss xmm0,xmm2
 movss [cam_d],xmm0
.nod:
 movss xmm0,[cam_rx]                    ; GL: negative rx = camera BELOW the object -> invert (rx of the table = height)
 mulss xmm0,[cmone]
 cmp dword [kscene],3
 jb .tr
 FC xmm2,5.0                        ; the camera always stays above the ground (5 to 80 deg)
 maxss xmm0,xmm2
 FC xmm2,80.0
 minss xmm0,xmm2
.tr:
 movss [cam_rx],xmm0
 movss xmm0,[cam_brx]
 mulss xmm0,[cmone]
 FC xmm2,5.0
 maxss xmm0,xmm2
 movss [cam_brx],xmm0
 cmp dword [kscene],3                  ; scenes: global camera (background + objects)
 jb .titre
 mov eax,[cam_rx]
 mov [cam_brx],eax
 mov eax,[cam_ry]
 mov [cam_bry],eax
 mov eax,[cam_d]
 mov [cam_bd],eax
 mov eax,[cam_roll]
 mov [cam_broll],eax
.titre:
 movss xmm0,[cam_cfg+36]
 mulss xmm0,[c_psk]
 movss [psize],xmm0
 movss xmm1,[g_K]
 FC xmm2,0.15
 mulss xmm1,xmm2
 addss xmm1,[cone]
 mulss xmm0,xmm1
 movss [fx_ps],xmm0
 popad
 ret

; xorshift32 -> xmm0 in [0,1)
cin_rand:
 mov eax,[cin_rs]
 mov edx,eax
 shl edx,13
 xor eax,edx
 mov edx,eax
 shr edx,17
 xor eax,edx
 mov edx,eax
 shl edx,5
 xor eax,edx
 mov [cin_rs],eax
 shr eax,8
 cvtsi2ss xmm0,eax
 mulss xmm0,[c2m24]
 ret

; ---------- start frames of each scene (from the song positions) ----------
scenes_init:
 pushad
 xor ebx,ebx                           ; scene
 xor ecx,ecx                           ; frame
.s:
 imul esi,ebx,SC_SIZE
 mov eax,[scn_tab+esi+SC_ORDER]
 shl eax,6
 add eax,[scn_tab+esi+SC_ROW]
.f:
 cmp ecx,[nfr]
 jae .e
 movzx edx,word [fr_pos+ecx*2]
 cmp edx,eax
 jae .e
 inc ecx
 jmp .f
.e:
 mov [scn_f+ebx*4],ecx
 inc ebx
 cmp ebx,SCN_N
 jb .s
 mov eax,[nfr]
 mov [scn_f+ebx*4],eax
 popad
 ret

frame_update:
 pushad
 mov eax,[samp]
 xor edx,edx
 mov ecx,SPF
 div ecx
 mov ebx,[nfr]
 dec ebx
 cmp eax,ebx
 jbe .fo
 mov eax,ebx
.fo:
 mov [curf],eax
 mov [g_F],eax
 cvtsi2ss xmm0,edx
 mulss xmm0,[inv882]
 movss [g_ff],xmm0
 ; current scene
 mov ecx,SCN_N-1
.fs:
 cmp eax,[scn_f+ecx*4]
 jae .fd
 dec ecx
 jns .fs
 xor ecx,ecx
.fd:
 mov [kscene],ecx
 mov eax,[scn_f+ecx*4]
 imul eax,eax,SPF
 mov edx,[samp]
 sub edx,eax
 cvtsi2ss xmm0,edx
 mulss xmm0,[inv44100]
 movss [g_ST],xmm0
 cvtsi2ss xmm1,dword [samp]
 mulss xmm1,[inv44100]
 movss xmm2,xmm1
 subss xmm2,[tprev]
 movss [tprev],xmm1
 maxss xmm2,[c_dtmin]
 minss xmm2,[c_dtmax]
 movss [g_DT],xmm2
 movss [g_T],xmm1
 call vars_update
 mov ecx,[kscene]
 cmp ecx,[lastk]
 je .same
 cmp dword [lastk],-1
 je .first
 mov dword [mv_new],1
.first:
 mov [lastk],ecx
 mov eax,ecx
 mov dword [bg_par],0
 call scene_begin
.same:
 mov eax,[bg_par]              ; the ground (300000 points) is only recomputed every other frame
 xor eax,1
 mov [bg_par],eax
 jz .skipbg
 mov ebp,lay_bg
 call layer_params
 mov ecx,1
 call layer_pass
.skipbg:
 mov ebp,lay_fg
 call layer_params
 mov ecx,1
 call layer_pass
 mov eax,[mg_n]
 add eax,[mg_n+4]
 mov [npts],eax
 call camera_update
 call morph_frame
 ; fade: start and end of the track
 movss xmm0,[g_T]
 mulss xmm0,[c_fi]
 movss xmm1,[cone]
 subss xmm1,xmm0
 cvtsi2ss xmm2,dword [nfr]
 mulss xmm2,[c_last]
 subss xmm2,[g_T]
 mulss xmm2,[c_fo]
 movss xmm3,[cone]
 subss xmm3,xmm2
 maxss xmm1,xmm3
 maxss xmm1,[c0]
 minss xmm1,[cone]
 movss [fade],xmm1
 popad
 ret
