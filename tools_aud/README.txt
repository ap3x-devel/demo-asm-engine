Regenerating the music and the sync tables (plain C, MinGW: gcc -O2 -o tool tool.c -lm)
 1. decode the MP3 to 44100 Hz stereo 16-bit PCM (ffmpeg -i song.mp3 -ac 2 -ar 44100 -f s16le song.s16)
 2. audana song.s16 ../music        -> t_lvl.bin t_age.bin t_pit.bin t_ins.bin t_pos.bin track.inc bars.txt
    (4 bands, attacks, tempo, beat tracking, bars)
 3. adpcm enc song.s16 ../music/track.adp ../music/adp_tab.inc
 4. schedule ../music/bars.txt starts.txt NFR > scenes.txt   (aligns the scenes on the bars)
 5. ../build.sh then upx --best --lzma bin/demo.exe
coder: Ivandoe
